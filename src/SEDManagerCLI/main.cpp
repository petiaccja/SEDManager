#include "Interactive.hpp"
#include "PBA.hpp"
#include "Utility.hpp"

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <SEDManager/SEDManager.hpp>
#include <rang.hpp>

#include <algorithm>
#include <iostream>


using namespace sedmgr;


class MainApp {
public:
    MainApp() {
        m_guided = m_cli.add_option("-g,--guided", m_guidedName, "Guided sessions walk you through the configuration process step by step.");
        m_interactive = m_cli.add_flag("-i,--interactive", "Interactive sessions allow you to manually inspect and configure tables.");
        m_pba = m_cli.add_flag("--pba", "Perform pre-boot authentication by finding locked devices and asking for passwords to unlock.");

        m_guided->excludes(m_interactive);
        m_guided->excludes(m_pba);
        m_interactive->excludes(m_pba);

        m_device = m_cli.add_option("device", m_devicePath, "The path to the device you want to configure.");
        m_guided->default_val(std::string{});
        m_guided->needs(m_device);
        m_interactive->needs(m_device);
    }
    MainApp(const MainApp&) = delete;
    MainApp(MainApp&) = delete;
    MainApp& operator=(const MainApp&) = delete;
    MainApp& operator=(MainApp&) = delete;

    int Run(int argc, char* argv[]) {
        try {
            m_cli.parse(argc, argv);

            if (*m_guided) {
                std::cout << "No guided sessions available yet." << std::endl;
                return 0;
            }
            else if (*m_pba) {
                PBA session;
                return session.Run();
            }
            else if (*m_interactive) {
                const auto device = std::make_shared<NvmeDevice>(m_devicePath);
                const auto identity = device->IdentifyController();
                std::cout << rang::fg::yellow << "Drive: "
                          << rang::fg::reset << identity.modelNumber
                          << rang::style::reset << std::endl;
                SEDManager manager(device);
                Interactive session(manager);
                return session.Run();
            }
            else {
                throw CLI::CallForHelp();
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
    CLI::Option* m_pba;
};


int main(int argc, char* argv[]) {
    MainApp mainApp;
    return mainApp.Run(argc, argv);
}