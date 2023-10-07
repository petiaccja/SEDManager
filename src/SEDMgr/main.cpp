#include <TPerLib/NvmeDevice.hpp>
#include <TPerLib/Serialization/Utility.hpp>
#include <TPerLib/Session.hpp>
#include <TPerLib/SessionManager.hpp>
#include <TPerLib/TrustedPeripheral.hpp>

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include <fstream>
#include <iostream>
#include <ranges>
#include <string_view>
#include <unordered_map>


std::string_view ConvertRawCharacters(std::ranges::contiguous_range auto&& r) {
    return std::string_view(std::ranges::begin(r), std::ranges::end(r));
}


std::string_view GetSSCName(const StorageClassFeatureDesc& sscDesc) {
    if (std::holds_alternative<Opal2FeatureDesc>(sscDesc)) {
        return "Opal V2";
    }
    else if (std::holds_alternative<Pyrite1FeatureDesc>(sscDesc)) {
        return "Pyrite V1";
    }
    else if (std::holds_alternative<Pyrite2FeatureDesc>(sscDesc)) {
        return "Pyrite V2";
    }
    else if (std::holds_alternative<OpaliteFeatureDesc>(sscDesc)) {
        return "Opalite";
    }
    else {
        return "Unkown storage subsystem class";
    }
}


std::string_view GetComIdStateStr(eComIdState state) {
    switch (state) {
        case eComIdState::INVALID: return "invalid";
        case eComIdState::INACTIVE: return "inactive";
        case eComIdState::ISSUED: return "issued";
        case eComIdState::ASSOCIATED: return "associated";
    }
    return "unknown";
};


void PrintCapabilities(const TPerDesc& desc) {
    std::cout << std::format("  {}", GetSSCName(desc.sscDesc)) << std::endl;

    if (desc.tperDesc) {
        std::cout << "  TPer:" << std::endl;
        std::cout << std::format("    ComID management supported: {}", desc.tperDesc->comIdMgmtSupported) << std::endl;
    }
    if (desc.lockingDesc) {
        std::cout << "  Locking:" << std::endl;
        std::cout << std::format("    Locking supported:          {}", desc.lockingDesc->lockingSupported) << std::endl;
        std::cout << std::format("    Locking enabled:            {}", desc.lockingDesc->lockingEnabled) << std::endl;
        std::cout << std::format("    Locked:                     {}", desc.lockingDesc->locked) << std::endl;
        std::cout << std::format("    Media encryption supported: {}", desc.lockingDesc->mediaEncryption) << std::endl;
        std::cout << std::format("    Shadow MBR done:            {}", desc.lockingDesc->mbrDone) << std::endl;
        std::cout << std::format("    Shadow MBR enabled:         {}", desc.lockingDesc->mbrEnabled) << std::endl;
        std::cout << std::format("    Shadow MBR supported:       {}", desc.lockingDesc->mbrSupported) << std::endl;
    }
    if (!std::holds_alternative<std::monostate>(desc.sscDesc)) {
        std::cout << "  SSC:" << std::endl;
        std::visit([](const auto& arg) {
            if constexpr (!std::is_convertible_v<decltype(arg), std::monostate>) {
                std::cout << std::format("    Base ComID:                 {}", arg.baseComId) << std::endl;
                std::cout << std::format("    Num ComIDs:                 {}", arg.numComIds) << std::endl;
                std::cout << std::format("    Initial C_PIN:              {}", arg.initialCPinSidIndicator) << std::endl;
                std::cout << std::format("    C_PIN revert behavior:      {}", arg.cPinSidRevertBehavior) << std::endl;
            }
        },
                   desc.sscDesc);
    }
}


void PrintProperies(const std::unordered_map<std::string, uint32_t>& properties) {
    std::cout << "TPer properties: " << std::endl;
    for (const auto& [name, value] : properties) {
        std::cout << std::format("  {} = {}", name, value) << std::endl;
    }
    std::cout << std::endl;
}


std::vector<std::byte> ReadPassword(std::string_view prompt) {
    std::string_view password = getpass(prompt.data());
    const auto bytes = std::as_bytes(BytesView(password));
    return { bytes.begin(), bytes.end() };
}


class App {
public:
    App(std::string_view device);

    std::vector<std::byte> GetMSID();
    void ChangeSIDPassword(std::span<const std::byte> currentPassword, std::span<const std::byte> newPassword);
    void Revert(std::span<const std::byte> psidPassword);
    const TPerDesc& GetCapabilities() const;
    std::unordered_map<std::string, uint32_t> GetProperties();

private:
    std::shared_ptr<NvmeDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    TPerDesc m_capabilities;
};


App::App(std::string_view device) {
    m_device = std::make_shared<NvmeDevice>(device);
    m_tper = std::make_shared<TrustedPeripheral>(m_device);
    m_capabilities = m_tper->GetDesc();
    const auto comIdState = m_tper->VerifyComId();
    if (comIdState != eComIdState::ISSUED && comIdState != eComIdState::ASSOCIATED) {
        throw std::runtime_error("failed to acquire valid ComID");
    }
    m_sessionManager = std::make_shared<SessionManager>(m_tper);
}


std::vector<std::byte> App::GetMSID() {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin);
    return session.base.Get<std::vector<std::byte>>(opal::eTableRow_C_PIN::C_PIN_MSID, 3);
}


void App::ChangeSIDPassword(std::span<const std::byte> currentPassword, std::span<const std::byte> newPassword) {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin, currentPassword, eAuthorityId::SID);
    session.base.Set(opal::eTableRow_C_PIN::C_PIN_SID, 3, newPassword);
}


void App::Revert(std::span<const std::byte> psidPassword) {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin, psidPassword, opal::eAuthorityId::PSID);
    session.opal.Revert(opal::eSecurityProvider::Admin);
}


const TPerDesc& App::GetCapabilities() const {
    return m_tper->GetDesc();
}


std::unordered_map<std::string, uint32_t> App::GetProperties() {
    const std::unordered_map<std::string, uint32_t> hostProperties = {
        { "MaxPackets", 1 },
        { "MaxSubpackets", 1 },
        { "MaxMethods", 1 },
        { "MaxComPacketSize", 65536 },
        { "MaxIndTokenSize", 65536 },
        { "MaxAggTokenSize", 65536 },
        { "ContinuedTokens", 0 },
        { "SequenceNumbers", 0 },
        { "AckNAK", 0 },
        { "Asynchronous", 0 },
    };

    auto [tperProps, hostProps] = m_sessionManager->Properties(hostProperties);
    return tperProps;
}


int main() {
    try {
        App app{ "/dev/nvme1" };

        CLI::App cliApp;
        bool hasExited = false;

        auto cmdHelp = cliApp.add_subcommand("help", "Print this help message.");
        auto cmdGetMsid = cliApp.add_subcommand("get-msid", "Print the MSID password.");
        auto cmdChangeSidPw = cliApp.add_subcommand("change-sid-pw", "Change the SID password.");
        auto cmdRevert = cliApp.add_subcommand("revert", "Revert the device to Original Manufacturing State.");
        auto cmdExit = cliApp.add_subcommand("exit", "Exit the application.");

        cmdHelp->callback([&] {
            std::cout << cliApp.get_formatter()->make_help(&cliApp, "", CLI::AppFormatMode::Normal) << std::endl;
        });

        cmdGetMsid->callback([&] {
            try {
                const auto msid = app.GetMSID();
                std::cout << "MSID: " << std::string_view(reinterpret_cast<const char*>(msid.data()), msid.size()) << std::endl;
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdChangeSidPw->callback([&] {
            try {
                const auto currentPw = ReadPassword("Current SID password:");
                const auto newPw = ReadPassword("New SID password:");
                const auto newPwAgain = ReadPassword("New SID password again:");
                if (newPw != newPwAgain) {
                    std::cout << "Passwords don't match. Try again." << std::endl;
                }
                else {
                    app.ChangeSIDPassword(currentPw, newPw);
                }
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdRevert->callback([&] {
            try {
                const auto password = ReadPassword("PSID password: ");
                app.Revert(password);
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdExit->callback([&] {
            hasExited = true;
        });

        while (!hasExited) {
            try {
            std::cout << "sedmgr> ";
            std::string command;
            std::getline(std::cin, command);
            cliApp.parse(command, false);
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}