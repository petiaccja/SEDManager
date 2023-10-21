#include "Interactive.hpp"
#include "Utility.hpp"

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <SEDManager/SEDManager.hpp>

#include <iostream>


void AddCmdHelp(CLI::App& cli) {
    auto cmd = cli.add_subcommand("help", "Print this help message.");
    cmd->callback([&] {
        std::cout << cli.get_formatter()->make_help(&cli, "", CLI::AppFormatMode::Normal) << std::endl;
    });
}

void AddCmdExit(CLI::App& cli, volatile bool& hasExited) {
    auto cmdExit = cli.add_subcommand("exit", "Exit the application.");
    cmdExit->callback([&] {
        hasExited = true;
    });
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: SEDManager <device>" << std::endl;
        return 1;
    }
    std::string_view devicePath = argv[1];
    try {
        const auto device = std::make_shared<NvmeDevice>(devicePath);
        const auto identity = device->IdentifyController();
        SEDManager app{ device };

        CLI::App cli;
        bool hasExited = false;

        AddCmdHelp(cli);
        AddCmdExit(cli, hasExited);
        interactive::AddCommands(app, cli);

        while (!hasExited && std::cin.good()) {
            try {
                std::string currentSPName = "?";
                std::vector<std::string> currentAuthNames;
                const auto currentSP = interactive::GetCurrentSP();
                if (currentSP) {
                    currentSPName = app.GetModules().FindName(*currentSP).value_or(to_string(*currentSP));
                }
                for (auto auth : interactive::GetCurrentAuthorities()) {
                    const std::string n = app.GetModules().FindName(auth).value_or(to_string(auth));
                    currentAuthNames.push_back(std::string(SplitName(n).back()));
                }

                std::cout << SplitName(currentSPName).back();
                if (currentSP) {
                    std::cout << "[ ";
                    if (currentAuthNames.empty()) {
                        std::cout << "Anybody";
                    }
                    else {
                        std::cout << Join(currentAuthNames, ", ");
                    }
                    std::cout << " ]";
                }
                std::cout << " @ " << identity.modelNumber;
                std::cout << "> ";
                std::string command;
                std::getline(std::cin, command);
                cli.parse(command, false);
            }
            catch (std::exception& ex) {
                std::cout << "Error: " << ex.what() << std::endl;
            }
        }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    return 0;
}