//
// Created by alex on 10/2/26.
//
#include "../Headers/TrackedDatesHandler.h"

#include <string>
#include <fstream>
#include "../Objects/Date.h"
#include "../Headers/Terminal.h"
#include "../Headers/Monitor.h"
#include <algorithm>

static std::string GetStringFromTrackDate(const TrackedDate& date) {
    return ('[' + date.title + "] " + date.date.asString(DateFormat::DDMMYYYY, true) + ' ' + getFormatAsStr(date.dateFormat));
}

static bool AlreadyExists(const TrackedDate& date) {
    for (const TrackedDate& d : trackedDates) {
        if (d.title == date.title)
            return true;
    }
    return false;
}

static TrackedDate GetTrackedDateFromString(const std::string& line) {
    TrackedDate date;

    size_t start = line.find(']', 1); size_t end = line.length() - 1;
    date.title = line.substr(1, start - 1);
    start = line.find_first_of(' ', start);
    end = line.find_first_of(' ', start);
    std::string dateStr = line.substr(start + 1, end - start - 1);
    start = line.find_last_of(' ');
    std::string r = line.substr(start + 1);

    date.dateFormat = getFormatAsDateFormat(line.substr(start + 1));
    date.date = Date::getDateFromString(dateStr);
    return date;
}

bool GetTrackedDates(const int page, const int maxPerPage, bool clearPrevious) {
    std::ifstream file((DATES_FILE_PATH.data()));

    if (!file.is_open()) {
        error("Failed to open file. Cannot fetch dates.");
        return false;
    }

    if (clearPrevious) trackedDates.clear();

    int fetched = 0, currentPage = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (fetched >= maxPerPage) break;
        if (line.empty() || (line.length() >= 2 && line[1] == '[')) continue;

        if (line[0] == '[') {
            fetched++;
            if (currentPage == page) trackedDates.push_back(GetTrackedDateFromString(line));
            else if (currentPage < page && fetched >= maxPerPage) {
                currentPage++;
                fetched = 0;
            }
        }
    }

    return true;
}

int GetTrackedPages(const int maxPerPage) {
    std::ifstream file((DATES_FILE_PATH.data()));

    if (!file.is_open()) {
        error("Failed to open file. Cannot fetch dates.");
        return false;
    }

    int fetchedPages = 0, fetchedLines = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || (line.length() >= 2 && line[1] == '[')) continue;

        if (line[0] == '[') {
            fetchedLines++;
            if (fetchedLines >= maxPerPage) {
                fetchedPages++;
                fetchedLines = 0;
            }
        }
    }
    if (fetchedLines > 0) fetchedPages++;
    return fetchedPages;
}

bool TrackDate(const TrackedDate& newDate, bool addToList) {
    if (AlreadyExists(newDate) && addToList) {
        warn(newDate.title + " already exists. Choose a different title.");
        return false;
    }

    std::ofstream file(DATES_FILE_PATH.data(), std::ios::app);
    if (!file.is_open()) {
        error("Failed to open dates file to add new date.");
        return false;
    }
    file << GetStringFromTrackDate(newDate) << '\n';
    file.close();

    if (addToList) trackedDates.push_back(newDate);

    return true;
}

bool UntrackDate(int targetIndex, const std::string& title, bool removeFromList) {
    std::ifstream oldFile(DATES_FILE_PATH.data());
    std::ofstream newFile("new_dates.ctrack");

    if (!oldFile.is_open()) {
        error("Failed to open old dates file!");
        return false;
    }
    if (!newFile.is_open()) {
        error("Failed to open new dates file!");
        return false;
    }

    int index = 0;
    bool removed = false;
    std::string line;
    while (std::getline(oldFile, line)) {
        if (line.empty() || line[0] != '[') continue;
        if (!removed) {
            if (targetIndex > -1 && index == targetIndex) {
                removed = true;
                continue;
            }
            std::string lineTitle = line.substr(1, line.find_first_of(']') - 1);
            if (lineTitle == title) {
                removed = true;
                continue;
            }
        }

        newFile << line << '\n';
        if (targetIndex > -1) index++;
    }
    oldFile.close();
    newFile.close();

    if (removeFromList) {
        auto it = std::remove_if(trackedDates.begin(), trackedDates.end(),
    [&title](const TrackedDate& m) { return m.title == title; });
        trackedDates.erase(it, trackedDates.end());
    }

    for (const Monitor& monitor : monitoredDates) {
        if (monitor.tDate.title == title) {
            RemoveMonitor(title, removeFromList);
            break;
        }
    }

    try {
        std::filesystem::rename("new_dates.ctrack", DATES_FILE_PATH.data());
    }
    catch (std::filesystem::filesystem_error& e) {
        std::string errorMsg = "Failed to remove date: ";
        errorMsg.append(e.what());
        error(errorMsg);

        return false;
    }

    if (!removed) {
        warn("Could not find date to remove.");
        return false;
    }

    return true;
}

int GetTrackedDateFromTitle(const std::string& title) {
    int index = 0;
    for (TrackedDate& date : trackedDates) {
        if (date.title == title) {
            return index;
        }
        index++;
    }

    return -1;
}