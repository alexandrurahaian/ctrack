//
// Created by alex on 10/3/26.
//

#ifndef CTRACK_MONITOR_H
#define CTRACK_MONITOR_H

#include <string>
#include <vector>
#include "TrackedDatesHandler.h"

enum class MonitorType {
    DaysUntil = 0,
    DaysSince = 1,
};

enum class DateType {
    OnPassSwitchToSince = 0,
    MonthlyReoccurring = 1,
    YearlyReoccurring = 2,
    DailyReoccurring = 3,
};

struct Monitor {
    TrackedDate tDate {};
    MonitorType mType = MonitorType::DaysUntil;
    DateType dType = DateType::OnPassSwitchToSince;
};

constexpr std::string_view MONITOR_FILE_PATH = "./monitored.ctrack";

inline std::vector<Monitor> monitoredDates;

inline MonitorType GetMTypeFromInt(const int index) {
    switch (index) {
        default:
        case 0: return MonitorType::DaysUntil;
        case 1: return MonitorType::DaysSince;
    }
}

inline DateType GetDateTypeFromInt(const int index) {
    switch (index) {
        default:
        case 0: return DateType::OnPassSwitchToSince;
        case 1: return DateType::MonthlyReoccurring;
        case 2: return DateType::YearlyReoccurring;
        case 3: return DateType::DailyReoccurring;
    }
}

bool AddMonitor(const Monitor& monitor, bool addToList = true);
bool RemoveMonitor(const std::string& dateTitle, bool deleteFromList = true);
bool GetMonitored(int page = 0, int max = 0, bool clearPrevious = false);
void SortVector(int low, int high);
int GetMonitoredPages(int maxEntriesPerPage);



#endif //CTRACK_MONITOR_H
