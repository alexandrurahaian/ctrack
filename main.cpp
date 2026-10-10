#include <iostream>
#include <algorithm>
#include "Headers/ConfigurationHandler.h"
#include "Headers/TrackedDatesHandler.h"
#include "Headers/Monitor.h"

void OutputWatermark() {
    std::cout << R"(
           mmmmmmmm                      mm
           """##"""                      ##
  m#####m     ##      m#####m  mm#####m  ## m##"
 ##"    "     ##      " mmm##  ##mmmm "  ##m##
 ##           ##     m##"""##   """"##m  ##"##m
 "##mmmm#     ##     ##mmm###  #mmmmm##  ##  "#m
   """""      ""      """" ""   """"""   ""   """

                                                  )" << '\n';
}

// TODO: Rework commands using objects.

int main() {
    int currentPage = 0, maxPages = 999;
    CONFIG = GetConfiguration();

    OutputWatermark();
    GetTrackedDates(0, CONFIG.maxEntriesPerPage);
    GetMonitored(0, CONFIG.maxEntriesPerPage);

    int monitorPages = GetMonitoredPages(CONFIG.maxEntriesPerPage), trackedPages = GetTrackedPages(CONFIG.maxEntriesPerPage);

    setOutColour((std::to_string(trackedDates.size()) + " tracked dates loaded."), OutputColour::BrightWhite, OutputColour::Green, {}, {}, true, 1);
    std::cout << "Run 'help' to get started.\n";
    std::string option;
    while (true) {
        std::cout << ">>: ";
        std::getline(std::cin, option);

        if (option == "exit" || option == "e") {
            std::string o;
            std::cout << "Are you sure you want to quit? (y/n): ";
            std::getline(std::cin, o);
            if (o == "y" || o == "Y") return 0;
        }
        else if (option == "help") {
            std::cout << R"(
add --> adds a new date.
remove --> removes the date that matches the title provided.
list --> displays a list of all the saved dates.
listm --> displays a list of all monitored dates.
monitor --> adds the specified date to the monitored dates.
unmonitor --> removes the specified date from the monitored dates.
status --> provides a status of all the monitored dates.
pagea --> advances a page forward.
pageb --> advances a page backward.
pager --> resets the page back to the first page.
gpage --> gets the current page.
exit/e --> closes the program after confirming.
help --> displays a list of all valid actions.
)";
            std::cout << '\n';
        }
        else if (option == "add") {
            std::string title, dateFormatStr, dateStr;
            title.reserve(16); dateFormatStr.reserve(10); dateStr.reserve(10);
            std::cout << "Date title (max 16): ";
            std::getline(std::cin, title);
            std::cout << "Date format (DD-MM-YY/DD-MM-YYYY/MM-DD-YY/MM-DD-YYYY/YY-MM-DD/YYYY-MM-DD\n: ";
            std::getline(std::cin, dateFormatStr);
            std::cout << "Date (input date in DD-MM-YYYY:hh-mm-ss format regardless of the previously chosen date format)\n: ";
            std::getline(std::cin, dateStr);

            std::string rawDateF = dateFormatStr;
            dateFormatStr.clear();

            for (const char& c : rawDateF) {
                if (c != '-') dateFormatStr += c;
            }

            TrackedDate d;
            d.title = std::move(title);
            d.dateFormat = getFormatAsDateFormat(dateFormatStr);
            d.date = Date::getDateFromString(dateStr);

            if (TrackDate(d))
                setFOutColour(("Successfully tracked date: [" + d.title + "] ").append(rawDateF).append(" - ").append(dateStr), OutputColour::BrightGreen, {}, true, 1);
        }
        else if (option == "remove") {
            std::string title;
            std::cout << "Title of the date to delete: ";
            std::getline(std::cin, title);

            if (UntrackDate(-1, title))
                setFOutColour(("Successfully deleted tracked date with title: " + title), OutputColour::BrightBlue, {}, true, 1);
        }
        else if (option == "list") {
            int dates = static_cast<int>(trackedDates.size());
            std::cout << "Displaying a maximum of " << CONFIG.maxEntriesPerPage << " per page.\n";

            for (int i = 0; i < CONFIG.maxEntriesPerPage; i++) {
                if (i > dates - 1) break;
                std::cout << "[" << trackedDates[i].title << "] " << trackedDates[i].date.asString(trackedDates[i].dateFormat) << '\n';
            }
            std::cout << "Page " << (trackedPages != 0 ? currentPage + 1 : 0) << " / " << trackedPages << '\n';
        }
        else if (option == "listm") {
            int dates = static_cast<int>(monitoredDates.size());
            std::cout << "Displaying a maximum of " << CONFIG.maxEntriesPerPage << " per page.\n";

            for (int i = 0; i < CONFIG.maxEntriesPerPage; i++) {
                if (i > dates - 1) break;
                int trackedDateIndex = GetTrackedDateFromTitle(monitoredDates[i].tDate.title);
                std::cout << '[' << monitoredDates[i].tDate.title << "] " << trackedDates[trackedDateIndex].date.asString(trackedDates[trackedDateIndex].dateFormat) << '\n';
            }
            std::cout << "Page " << (monitorPages != 0 ? currentPage + 1 : 0) << " / " << monitorPages << '\n';
        }
        else if (option == "monitor") {
            int dateIndex, dateType, monitorType;

            try {
                std::cout << "Countdown type (0 - days until, 1 - days since): ";
                if (!(std::cin >> monitorType) || (monitorType != 0 && monitorType != 1)) {
                    throw std::invalid_argument("Invalid monitor type, must be either 0 or 1");
                }

                std::cout << "What should happen when the date is reached?\n"
                          << "(0 - switch to days since, 1 - reoccur monthly, 2 - reoccur yearly, 3 - reoccur daily): ";
                if (!(std::cin >> dateType) || (dateType < 0 || dateType > 3)) {
                    throw std::invalid_argument("Invalid date type, must be between 0 and 3");
                }

                std::cout << "Target date index: ";
                if (!(std::cin >> dateIndex) || dateIndex < 0 || dateIndex >= static_cast<int>(trackedDates.size())) {
                    std::string maxIdx = trackedDates.empty() ? "0" : std::to_string(trackedDates.size() - 1);
                    throw std::invalid_argument("Invalid index, must be between 0 and " + maxIdx);
                }
            }
            catch (const std::exception& e) {
                warn(e.what());

                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << ">>: ";
                continue;
            }

            Monitor monitor;
            const TrackedDate& tDate = trackedDates.at(dateIndex);

            monitor.tDate = tDate;
            monitor.dType = GetDateTypeFromInt(dateType);
            monitor.mType = GetMTypeFromInt(monitorType);

            if (AddMonitor(monitor))
                setFOutColour("Monitoring date: " + tDate.date.asString(tDate.dateFormat, true), OutputColour::BrightBlue, {}, true, 1);
        }
        else if (option == "unmonitor") {
            std::string title;
            std::cout << "Title of the date to unmonitor: ";
            std::getline(std::cin, title);
            if (RemoveMonitor(title))
                setOutColour("Successfully unmonitored date: " + title, OutputColour::BrightGreen, {}, {}, {}, true, 1);
        }
        else if (option == "status") {
            const Date localDate = Date::getCurrentDate();
            std::cout << localDate.asString(DateFormat::DDMMYYYY, true) << '\n';
            std::cout << localDate.getYear() << '\n';
            int dates = static_cast<int>(monitoredDates.size());
            std::cout << dates << '\n';
            int pages = dates / CONFIG.maxEntriesPerPage;
            if (pages <= 0) pages = 1;

            std::cout << "Displaying a maximum of " << CONFIG.maxEntriesPerPage << " per page.\n";
            std::cout << "Total pages: " << pages << '\n';

            for (int i = 0; i < monitoredDates.size(); i++) {
                std::cout << '[' + std::to_string(i) + ']' << ' ';
                Monitor& date = monitoredDates[i];
                std::cout << "--- " + date.tDate.title + " ---: ";

                if (date.mType == MonitorType::DaysUntil) {
                    if (date.tDate.date > localDate) {
                        const Date& res = Date::getTimeUntilDate(localDate, date.tDate.date);
                        setOutColour("", OutputColour::BrightCyan);
                        std::cout << "TIME UNTIL " + date.tDate.date.asString(date.tDate.dateFormat, true) + ": " << res.getHour() << " hours " << res.getMinute() << " minutes " << res.getSecond() << " seconds " << res.getDay() << " days " << res.getMonth() << " months " << res.getYear() << " years\n";
                    }
                    else if (date.tDate.date <= localDate) {
                        Date since;
                        if (date.dType == DateType::OnPassSwitchToSince) {
                            std::cout << date.tDate.date.getYear() << '\n';
                            std::cout << "Switching to since..\n";
                            date.mType = MonitorType::DaysSince;
                            // update ONLY the file.
                            RemoveMonitor(date.tDate.title, false);
                            AddMonitor(date, false);

                            std::cout << date.tDate.date.getYear() << '\n';
                            since = Date::getTimeSinceDate(date.tDate.date, localDate);
                        }
                        else if (date.dType == DateType::DailyReoccurring || date.dType == DateType::MonthlyReoccurring || date.dType == DateType::YearlyReoccurring) {
                            UntrackDate(-1, date.tDate.title, false);

                            if (date.dType == DateType::DailyReoccurring) date.tDate.date.setDay(date.tDate.date.getDay() + 1);
                            else if (date.dType == DateType::MonthlyReoccurring) date.tDate.date.setMonth(date.tDate.date.getMonth() + 1);
                            else if (date.dType == DateType::YearlyReoccurring) date.tDate.date.setYear(date.tDate.date.getYear() + 1);

                            TrackDate(date.tDate, false);
                            since = Date::getTimeUntilDate(localDate, date.tDate.date);
                            setOutColour("", OutputColour::BrightCyan);
                            std::cout << "TIME UNTIL " + date.tDate.date.asString(date.tDate.dateFormat, true) + ": " << since.getHour() << " hours " << since.getMinute() << " minutes " << since.getSecond() << " seconds " << since.getDay() << " days " << since.getMonth() << " months " << since.getYear() << " years\n";
                            continue;
                        }
                        setOutColour("", OutputColour::BrightYellow);
                        std::cout << "TIME SINCE " + date.tDate.date.asString(date.tDate.dateFormat, true) + ": " << since.getHour() << " hours " << since.getMinute() << " minutes " << since.getSecond() << " seconds " << since.getDay() << " days " << since.getMonth() << " months " << since.getYear() << " years\n";
                    }
                }
                else if (date.mType == MonitorType::DaysSince) {
                    if (date.tDate.date < localDate) {
                        const Date& since = Date::getTimeSinceDate(date.tDate.date, localDate);
                        setOutColour("", OutputColour::BrightYellow);
                        std::cout << "TIME SINCE " + date.tDate.date.asString(date.tDate.dateFormat, true) + ": " << since.getHour() << " hours " << since.getMinute() << " minutes " << since.getSecond() << " seconds " << since.getDay() << " days " << since.getMonth() << " months " << since.getYear() << " years\n";
                    }
                }
                resetColour();
                if (i >= CONFIG.maxEntriesPerPage) break;
            }
        }
        else if (option == "page") {
            std::string Spg;
            std::cout << "Page number: ";
            std::getline(std::cin, Spg);

            try {
                currentPage = std::clamp(std::stoi(Spg), 0, maxPages);
            }
            catch (...) {}
            GetMonitored(currentPage, CONFIG.maxEntriesPerPage, true);
            GetTrackedDates(currentPage, CONFIG.maxEntriesPerPage, true);
            std::cout << "Current page: " << currentPage + 1<< '\n';
        }
        else if (option == "pagea") {
            currentPage = std::clamp(currentPage + 1, 0, maxPages);
            GetMonitored(currentPage, CONFIG.maxEntriesPerPage, true);
            GetTrackedDates(currentPage, CONFIG.maxEntriesPerPage, true);
            std::cout << "Current page: " << currentPage + 1<< '\n';
        }
        else if (option == "pageb") {
            currentPage = std::clamp(currentPage - 1, 0, maxPages);
            GetMonitored(currentPage, CONFIG.maxEntriesPerPage, true);
            GetTrackedDates(currentPage, CONFIG.maxEntriesPerPage, true);
            std::cout << "Current page: " << currentPage + 1 << '\n';
        }
        else if (option == "pager") {
            if (currentPage == 0) continue;
            std::string op;
            std::cout << "Are you sure you wish to return to page 1? (y/n): ";
            std::getline(std::cin, op);
            if (op == "y" || op == "Y") {
                currentPage = 0;
                GetMonitored(currentPage, CONFIG.maxEntriesPerPage, true);
                GetTrackedDates(currentPage, CONFIG.maxEntriesPerPage, true);
            }
        }
        else if (option == "gpage") {
            std::cout << "Current page is: " << currentPage + 1<< '\n';
        }
        else if (!option.empty()) warn("Unknown: " + option + " Run 'help' for a list of commands.");

        std::cin.clear();
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        //std::cout << ">>: ";
    }

    return 0;
}