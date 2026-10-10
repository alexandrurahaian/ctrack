//
// Created by alex on 10/3/26.
//
#include "../Headers/Monitor.h"

#include <algorithm>
#include <fstream>
#include <string>

static std::string GetStringFromMonitorStruct(const Monitor& mntr) {
    return ('[' + mntr.tDate.title + "] " + std::to_string(static_cast<int>(mntr.dType)) + ' ' + std::to_string(static_cast<int>(mntr.mType)));
}

struct MonitorResult {
    bool success = false;
    Monitor monitor;
};

static bool AlreadyMonitoring(const Monitor& monitor) {
    for (const Monitor& m : monitoredDates) {
        if (m.tDate.title == monitor.tDate.title)
            return true;
    }
    return false;
}

static MonitorResult GetMonitorFromString(const std::string& line) {
    Monitor fetchedM = {};
    fetchedM.dType = DateType::OnPassSwitchToSince;
    fetchedM.mType = MonitorType::DaysUntil;

    size_t openBracket = line.find('[');
    size_t closeBracket = line.find(']');

    if (openBracket == std::string::npos || closeBracket == std::string::npos || closeBracket <= openBracket) {
        error("Malformed input line: missing brackets []");
        return {.success = false, .monitor = fetchedM};
    }

    std::string title = line.substr(openBracket + 1, closeBracket - openBracket - 1);

    int tDateIndex = GetTrackedDateFromTitle(title);
    if (tDateIndex == -1) {
        error("Could not find tracked date in vector: " + title);
        return {.success = false, .monitor = fetchedM};
    }

    fetchedM.tDate = trackedDates.at(tDateIndex);

    size_t firstNumStart = line.find_first_not_of(" \t", closeBracket + 1);
    if (firstNumStart == std::string::npos) {
        error("Missing date type index after ']'");
        return {.success = false, .monitor = fetchedM};
    }

    size_t spaceAfterFirstNum = line.find_first_of(" \t", firstNumStart);
    if (spaceAfterFirstNum == std::string::npos) {
        error("Missing monitor type index");
        return {.success = false, .monitor = fetchedM};
    }

    size_t secondNumStart = line.find_first_not_of(" \t", spaceAfterFirstNum);
    if (secondNumStart == std::string::npos) {
        error("Missing monitor type index after space");
        return {.success = false, .monitor = fetchedM};
    }

    int dateTypeIndex = 0;
    int monitorTypeIndex = 0;

    try {
        dateTypeIndex = std::stoi(line.substr(firstNumStart, spaceAfterFirstNum - firstNumStart));
        monitorTypeIndex = std::stoi(line.substr(secondNumStart));
    }
    catch (...) {
        error("Failed to convert date or monitor index");
        return {.success = false, .monitor = fetchedM};
    }

    fetchedM.dType = GetDateTypeFromInt(dateTypeIndex);
    fetchedM.mType = GetMTypeFromInt(monitorTypeIndex);

    return {.success = true, .monitor = fetchedM};
}

static int Partition(const int low, const int high) {
    Monitor pivot = monitoredDates[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (monitoredDates[j].tDate.date <= pivot.tDate.date) {
            i++;
            std::swap(monitoredDates[i], monitoredDates[j]);
        }
    }

    std::swap(monitoredDates[i + 1], monitoredDates[high]);
    return i+1;
}

void SortVector(const int low, const int high) {
    if (monitoredDates.size() < 2) return;
    if (low < high) {
        const int pi = Partition(low, high);
        SortVector(low, pi-1);
        SortVector(pi+1, high);
    }
}

bool AddMonitor(const Monitor& monitor, bool addToList) {
    if (AlreadyMonitoring(monitor) && addToList) {
        warn(monitor.tDate.title + " is already being monitored.");
        return false;
    }

    std::ofstream monitorFile((MONITOR_FILE_PATH.data()), std::ios::app);
    if (!monitorFile.is_open()) {
        error("Unable to open monitor file for adding a new monitored date.");
        return false;
    }
    monitorFile << GetStringFromMonitorStruct(monitor) << '\n';
    monitorFile.close();

    if (addToList) {
        monitoredDates.push_back(monitor);
        SortVector(0, static_cast<int>(monitoredDates.size()) - 1);
    }

    return true;
}
bool RemoveMonitor(const std::string& dateTitle, bool deleteFromList) {
    std::ifstream oldMFile((MONITOR_FILE_PATH.data()));
    std::ofstream monitorFile("new_monitor.ctrack");
    if (!oldMFile.is_open()) {
        error("Unable to open monitor file for removing a monitored date.");
        return false;
    }
    if (!monitorFile.is_open()) {
        error("Unable to open new monitor file for updating monitored dates.");
        return false;
    }

    bool removed = false;

    std::string line;
    while (std::getline(oldMFile, line)) {
        if (!removed && !line.empty() && line[0] == '[') {
            std::string title = line.substr(1, line.find_first_of(']') - 1);
            if (title == dateTitle) {
                removed  = true;
                continue;
            }
        }
        monitorFile << line << '\n';
    }
    oldMFile.close();
    monitorFile.close();

    if (deleteFromList) {
        auto it = std::remove_if(monitoredDates.begin(), monitoredDates.end(),
    [&dateTitle](const Monitor& m) { return m.tDate.title == dateTitle; });
        monitoredDates.erase(it, monitoredDates.end());
        SortVector(0, static_cast<int>(monitoredDates.size()) - 1);
    }

    try {
        std::filesystem::rename("new_monitor.ctrack", (MONITOR_FILE_PATH.data()));
    }
    catch (std::filesystem::filesystem_error& e) {
        error("Failed to rename new monitor file to original file name: ");
        error(e.what());
        return false;
    }

    if (!removed) {
        warn("No date with title: " + dateTitle + " found to remove!");
        return false;
    }

    return true;
}
bool GetMonitored(const int page, const int max, bool clearPrevious) {
    std::ifstream file((MONITOR_FILE_PATH.data()));
    if (!file.is_open()) {
        error("Could not open monitor file.");
        return false;
    }

    if (clearPrevious) monitoredDates.clear();

    std::string line;
    int fetched = 0, currentPage = 0;
    while (std::getline(file, line)) {
        if (fetched >= max) break;
        if (line.empty() || line[0] != '[') continue;
        const MonitorResult& result = GetMonitorFromString(line);
        if (result.success) {
            if (currentPage == page) monitoredDates.push_back(result.monitor);
            fetched++;
            if (fetched >= max && currentPage < page) {
                currentPage++;
                fetched = 0;
            }
        }
        else warn("Failed to get monitor from line: " + line);
    }
    file.close();

    SortVector(0, static_cast<int>(monitoredDates.size()) - 1);
    return true;
}

int GetMonitoredPages(const int maxEntriesPerPage) {
    std::ifstream file((MONITOR_FILE_PATH.data()));
    if (!file.is_open()) {
        error("Could not open monitor file.");
        return false;
    }

    std::string line;
    int fetchedLines = 0, fetchedPages = 0;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] != '[') continue;
        const MonitorResult& result = GetMonitorFromString(line);
        if (result.success) {
            fetchedLines++;
            if (fetchedLines >= maxEntriesPerPage) {
                fetchedLines = 0;
                fetchedPages++;
            }
        }
    }

    if (fetchedLines > 0) fetchedPages++;

    return fetchedPages;
}