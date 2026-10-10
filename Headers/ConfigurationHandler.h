//
// Created by alex on 10/2/26.
//

#ifndef CTRACK_CONFIGURATIONHANDLER_H
#define CTRACK_CONFIGURATIONHANDLER_H

#include <string_view>
#include <fstream>
#include "Terminal.h"
#include <filesystem>

static std::string_view CONFIGURATION_FILE_PATH = "./settings.cfg";

enum class DateFormat {
    DDMMYY,
    DDMMYYYY,
    MMDDYY,
    MMDDYYYY,
    YYYYMMDD,
    YYMMDD
};

struct CONFIGURATION {
    DateFormat format = DateFormat::DDMMYY;
    int maxEntriesPerPage = 15;
    std::string monitorFPath = "./monitored.ctrack";
    std::string trackedFPath = "./dates.ctrack";
};

inline CONFIGURATION CONFIG;

inline std::string getFormatAsStr(const DateFormat& format) {
    switch (format) {
        case DateFormat::DDMMYY:
            return "DDMMYY";
        case DateFormat::DDMMYYYY:
            return "DDMMYYYY";
        case DateFormat::MMDDYY:
            return "MMDDYY";
        case DateFormat::MMDDYYYY:
            return "MMDDYYYY";
        case DateFormat::YYMMDD:
            return "YYMMDD";
        case DateFormat::YYYYMMDD:
            return "YYYYMMDD";
        default:
            return "DDMMYYYY";
    }
}

inline DateFormat getFormatAsDateFormat(const std::string& format) {
    if (format == "DDMMYY")
        return DateFormat::DDMMYY;
    if (format == "DDMMYYYY")
        return DateFormat::DDMMYYYY;
    if (format == "MMDDYY")
        return DateFormat::MMDDYY;
    if (format == "MMDDYYYY")
        return DateFormat::MMDDYYYY;
    if (format == "YYMMDD")
        return DateFormat::YYMMDD;
    if (format == "YYYYYMMDD")
        return DateFormat::YYYYMMDD;

    return DateFormat::DDMMYY;
}

inline void HandleConfig(const std::string& name, const std::string& value, CONFIGURATION& cfg) {
    if (name == "date_format") {
        cfg.format = getFormatAsDateFormat(value);
    }
    else if (name == "max_dates_per_page") {
        try {
            cfg.maxEntriesPerPage = std::stoi(value);
        }
        catch (...) {
            error("Could not set max entries per page, defaulting to 15: " + value + " is not a valid number.");
            cfg.maxEntriesPerPage = 15;
        }
    }
    else if (name == "dates_file_path") cfg.trackedFPath = value;
    else if (name == "monitor_file_path") cfg.monitorFPath = value;
}

inline CONFIGURATION GetConfiguration() {
    std::ifstream cfg(static_cast<std::string>(CONFIGURATION_FILE_PATH));
    if (!cfg.is_open()) {
        error("Could not open configuration file, using default configs.");
        return {};
    }

    CONFIGURATION config;
    std::string line;
    while (std::getline(cfg, line)) {
        if (line.find('=') == std::string::npos) continue;

        size_t equalPos = line.find('=');
        std::string configName = line.substr(0, equalPos - 1);
        std::string configValue = line.substr(equalPos + 1);

        HandleConfig(configName, configValue, config);
    }
    cfg.close();
    return config;
}

inline void HandleSave(const CONFIGURATION& newCfg,  std::ofstream& file) {
    file << "date_format=" << getFormatAsStr(newCfg.format) << '\n';
    file << "max_dates_per_page=" << newCfg.maxEntriesPerPage << '\n';

    file.close();
}

inline bool SaveConfiguration(const CONFIGURATION& config) {
    std::ofstream tCfg("temp_set.cfg");
    if (!tCfg.is_open()) {
        error("Failed to open temporary settings file! Aborted.");
        return false;
    }

    HandleSave(config, tCfg);
    if (tCfg.is_open()) tCfg.close();

    try {
        std::filesystem::rename("temp_set.cfg", "settings.cfg");
    }
    catch (const std::filesystem::filesystem_error& e) {
        error("Failed to overwrite settings.cfg: " + std::string(e.what()));
        return false;
    }

    return true;
}


#endif //CTRACK_CONFIGURATIONHANDLER_H
