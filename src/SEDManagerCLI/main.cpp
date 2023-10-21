#include "Interactive.hpp"

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
    std::string_view device = argv[1];
    try {
        SEDManager app{ device };

        CLI::App cli;
        bool hasExited = false;

        AddCmdHelp(cli);
        AddCmdExit(cli, hasExited);
        AddCommandsInteractive(app, cli);

        while (!hasExited && std::cin.good()) {
            try {
                std::cout << "sedmanager";
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