#include "Utility.hpp"

#include <iostream>
#include <stdexcept>


#ifdef __linux__
    #include <unistd.h>
#elif defined(_WIN32)
    #include <conio.h>
#endif


using namespace sedmgr;


std::vector<std::byte> GetPassword(std::string_view prompt) {
#ifdef __linux__
    std::string_view password = getpass(prompt.data());
    const auto bytes = std::as_bytes(std::span(password));
    return { bytes.begin(), bytes.end() };
#elif defined(_WIN32)
    std::cout << prompt;
    std::vector<std::byte> password;
    char ch;
    while (true) {
        ch = _getch();
        if (ch != '\r' && ch != '\n') {
            password.push_back(static_cast<std::byte>(ch));
        }
        else {
            break;
        }
    }
    while (0 < std::cin.readsome(&ch, 1))
    {}
    std::cout << std::endl;
    return password;
#else
    static_assert(false, "Password reading is not implemented for platform.");
#endif
}


void FlushCin() {
    char ch;
    while (0 != std::cin.readsome(&ch, 1))
    {
        // Empty
    }
}


std::string GetMultiline(std::string_view terminator) {
    std::string text;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == terminator) {
            break;
        }
        text += line;
        text += '\n';
    }
    return text;
}


std::optional<UID> ParseObjectRef(EncryptedDevice& app, std::string_view nameOrUid, std::optional<UID> sp) {
    const auto maybeUid = app.GetModules().FindUid(nameOrUid, sp);
    if (maybeUid) {
        return *maybeUid;
    }
    try {
        return stouid(nameOrUid);
    }
    catch (...) {
        // Fallthrough
    }
    return std::nullopt;
}


std::string FormatObjectRef(EncryptedDevice& app, UID uid, std::optional<UID> sp) {
    const auto maybeName = app.GetModules().FindName(uid, sp);
    if (maybeName) {
        return *maybeName;
    }
    return to_string(uid);
}


std::vector<std::string_view> SplitName(std::string_view name) {
    std::vector<std::string_view> sections;
    size_t pos = 0;
    while (pos <= name.size()) {
        const auto start = pos;
        pos = name.find("::", pos);
        sections.push_back(name.substr(start, pos));
        pos = std::max(pos + 2, pos);
    }
    return sections;
}


std::string FormatTable(std::span<const std::string> columns, std::span<const std::vector<std::string>> rows) {
    const size_t numColumns = columns.size();

    std::vector<size_t> columnWidths(columns.size(), 0);

    for (size_t columnIdx = 0; columnIdx < numColumns; ++columnIdx) {
        columnWidths[columnIdx] = std::max(std::size(columns[columnIdx]), columnWidths[columnIdx]);
        for (const auto& row : rows) {
            assert(row.size() == numColumns);
            columnWidths[columnIdx] = std::max(std::size(row[columnIdx]), columnWidths[columnIdx]);
        }
    }

    std::stringstream ss;
    std::vector<std::string> padded;
    for (size_t columnIdx = 0; columnIdx < numColumns; ++columnIdx) {
        const auto& cell = columns[columnIdx];
        const auto width = columnWidths[columnIdx];
        padded.push_back(cell + std::string(width - cell.size(), ' '));
    }
    ss << "| " << Join(padded, " | ") << " |" << std::endl;

    padded.clear();
    for (size_t columnIdx = 0; columnIdx < numColumns; ++columnIdx) {
        const auto width = columnWidths[columnIdx];
        padded.push_back(std::string(width, '-'));
    }
    ss << "+-" << Join(padded, "-+-") << "-+" << std::endl;

    for (const auto& row : rows) {
        padded.clear();
        for (size_t columnIdx = 0; columnIdx < numColumns; ++columnIdx) {
            const auto& cell = row[columnIdx];
            const auto width = columnWidths[columnIdx];
            padded.push_back(cell + std::string(width - cell.size(), ' '));
        }
        ss << "| " << Join(padded, " | ") << " |" << std::endl;
    }

    return ss.str();
}