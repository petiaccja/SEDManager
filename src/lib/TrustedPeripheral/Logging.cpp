#include "Logging.hpp"

#include <Archive/TokenDebugArchive.hpp>
#include <Archive/Types/ValueToToken.hpp>

#include <filesystem>
#include <fstream>


std::ofstream& GetLogFile() {
    const auto path = std::filesystem::temp_directory_path() / "sedmgr.log";
    static std::ofstream file;
    if (!file.is_open()) {
        file.open(path, std::ios::out | std::ios::trunc);
    }
    return file;
}

static bool logging =
#ifndef NDEBUG
    true
#else
    false
#endif
    ;

void SetLogging(bool enabled) {
    logging = enabled;
}

void Log(std::string_view event, const ComPacket& request) {
    if (logging) {
        auto& file = GetLogFile();
        file << "#--------------------------------------------------------------" << std::endl;
        file << "# " << event << std::endl;
        file << "# Logging packet is not supported yet." << std::endl;
        file << "#--------------------------------------------------------------" << std::endl;
    }
}

void Log(std::string_view event, const Value& request) {
    if (logging) {
        auto& file = GetLogFile();

        file << "#--------------------------------------------------------------" << std::endl;
        file << "# " << event << std::endl;
        file << "#--------------------------------------------------------------" << std::endl;
        file << "\n";

        TokenDebugArchive ar(file);

        save_strip_list(ar, request);
        file << "\n";
        file << "\n";
    }
}