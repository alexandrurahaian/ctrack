//
// Created by alex on 10/2/26.
//

#ifndef CTRACK_TRAKCEDDATESHANDLER_H
#define CTRACK_TRAKCEDDATESHANDLER_H

#include <string>
#include <vector>
#include "../Objects/Date.h"

constexpr std::string_view DATES_FILE_PATH = "./dates.ctrack";

struct TrackedDate {
    std::string title;
    Date date;
    DateFormat dateFormat = DateFormat::DDMMYYYY;
};

inline std::vector<TrackedDate> trackedDates;

bool GetTrackedDates(int page = 0, int maxPerPage = 0, bool clearPrevious = false);
bool TrackDate(const TrackedDate& newDate, bool addToList = true);
bool UntrackDate(int targetIndex = -1, const std::string& title = "", bool removeFromList = true);
bool SaveTrackedDates();
int GetTrackedDateFromTitle(const std::string& title);
int GetTrackedPages(int maxEntriesPerPage);

#endif //CTRACK_TRAKCEDDATESHANDLER_H
