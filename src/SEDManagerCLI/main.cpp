#include "Interactive.hpp"
#include "Utility.hpp"

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <SEDManager/SEDManager.hpp>
#include <rang.hpp>

#include <algorithm>
#include <iostream>


class MainApp {
public:
    MainApp() {
        m_guided = m_cli.add_option("-g,--guided", m_guidedName, "Guided sessions walk you through the configuration process step by step.");
        m_guided->default_val(std::string{});
        m_interactive = m_cli.add_flag("-i,--interactive", "Interactive sessions allow you to manually inspect and configure tables.")->excludes(m_guided);
        m_device = m_cli.add_option("device", m_devicePath, "The path to the device you want to configure.")->required();
    }
    MainApp(const MainApp&) = delete;
    MainApp(MainApp&) = delete;
    MainApp& operator=(const MainApp&) = delete;
    MainApp& operator=(MainApp&) = delete;

    int Run(int argc, char* argv[]) {
        try {
            m_cli.parse(argc, argv);

            if (!*m_guided && !*m_interactive) {
                throw CLI::CallForHelp();
            }

            if (*m_guided) {
                std::cout << "No guided sessions available yet." << std::endl;
                return 0;
            }
            else {
                const auto device = std::make_shared<NvmeDevice>(m_devicePath);
                const auto identity = device->IdentifyController();
                std::cout << rang::fg::yellow << "Drive: "
                          << rang::fg::reset << identity.modelNumber
                          << rang::style::reset << std::endl;
                SEDManager manager(device);
                Interactive session(manager);
                return session.Run();
            }
        }
        catch (CLI::CallForHelp&) {
            std::cout << m_cli.help() << std::endl;
        }
        catch (std::exception& ex) {
            std::cout << "Error: " << ex.what() << std::endl;
        }
        return -1;
    }

private:
    CLI::App m_cli;
    std::string m_guidedName;
    std::string m_devicePath;
    CLI::Option* m_guided;
    CLI::Option* m_interactive;
    CLI::Option* m_device;
};



int main(int argc, char* argv[]) {
    MainApp mainApp;
    return mainApp.Run(argc, argv);
}