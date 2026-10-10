//
// Created by alex on 9/25/26.
//

#ifndef CTRACK_DATE_H
#define CTRACK_DATE_H

#include <vector>
#include <ctime>
#include <cstring>

#include "../Headers/ConfigurationHandler.h"
#include "../Headers/Terminal.h"

#include <iostream>

class Date {
private:
    int year = -1, month = -1, day = -1, hour = -1, minute = -1, second = -1;
    static std::string padZero(int value, size_t width = 2) {
        std::string s = std::to_string(value);
        if (s.length() < width) {
            s.insert(0, width - s.length(), '0');
        }
    return s;
    }
public:
    Date() = default;
    Date(int _y, int _m, int _d, int _h = 0, int _min = 0, int _s = 0) {
        if (_y > 0 && _y <= 9999) year = _y;
        if (_m >= 1 && _m <= 12) month = _m;
        if (_h >= 0 && _h <= 23) hour = _h;
        if (_min >= 0 && _min <= 59) minute = _min;
        if (_s >= 0 && _s <= 59) second = _s;

        if (month != -1 && year != -1) {
            int maxDays = daysInMonths()[month - 1];
            if (_d >= 1 && _d <= maxDays) {
                day = _d;
            } else if (!isLeapYear() && month == 2 && _d == 29) {
                warn("Attempted to set February 29th on non-leap year " + std::to_string(year) + ". Setting to 28th.");
                day = 28;
            }
        }

        if (year == -1 || month == -1 || day == -1 || hour == -1 || minute == -1 || second == -1) {
            error("Attempted to make invalid date with at least 1 invalid date parameter!");
            return;
        }
    }
    static Date getCurrentDate() {
        Date d;

        const time_t currentTime = time(nullptr);
        const tm* local_time = localtime(&currentTime);

        d.day = local_time->tm_mday;
        d.year = local_time->tm_year + 1900;
        d.month = local_time->tm_mon + 1;
        d.hour = local_time->tm_hour;
        d.minute = local_time->tm_min;
        d.second = local_time->tm_sec;

        return d;
    }

    static Date getDateFromString(const std::string& ogLine) {
        size_t colonPos = ogLine.find(':');
        if (colonPos == std::string::npos || colonPos < 8 || ogLine.length() < colonPos + 9) {
            error("Invalid line format, missing date-time separator: " + ogLine);
            return {};
        }

        Date d{};

        try {
            d.hour   = std::stoi(ogLine.substr(colonPos + 1, 2));
            d.minute = std::stoi(ogLine.substr(colonPos + 4, 2));
            d.second = std::stoi(ogLine.substr(colonPos + 7, 2));

            size_t yearStart = ogLine.rfind('-', colonPos - 1) + 1;
            size_t yearLen   = colonPos - yearStart;

            d.year = std::stoi(ogLine.substr(yearStart, yearLen));
            if (d.year < 100) {
                d.year += (d.year >= 70) ? 1900 : 2000;
            }

            size_t monthStart = ogLine.rfind('-', yearStart - 2) + 1;
            d.month = std::stoi(ogLine.substr(monthStart, 2));
            d.day = std::stoi(ogLine.substr(monthStart - 3, 2));
        }
        catch (...) {
            error("Could not parse date-time integers from line: " + ogLine);
            return {};
        }

        return d;
    }


    bool isLeapYear() const {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }
    static bool isLeapYearStatic(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    std::vector<int> daysInMonths() const {
        std::vector days = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (isLeapYear()) days[1] = 29;
        return days;
    }

    static std::vector<int> daysInMonthsStatic(int year) {
        std::vector days = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (isLeapYearStatic(year)) days[1] = 29;
        return days;
    }

    static Date getTimeUntilDate(const Date& start, Date end) {
        Date tU;

        tU.second = end.second - start.second;
        if (tU.second < 0) {
            tU.second += 60;
            end.minute--;
        }

        tU.minute = end.minute - start.minute;
        if (tU.minute < 0) {
            tU.minute += 60;
            end.hour--;
        }
        tU.hour = end.hour - start.hour;
        if (tU.hour < 0) {
            tU.hour += 24;
            end.day--;
        }
        if (end.day < start.day) {
            int prevMonth = end.month - 1;
            int prevYear = end.year;
            if (prevMonth < 1) {
                prevMonth = 12;
                prevYear--;
            }
            end.day += daysInMonthsStatic(prevYear)[prevMonth - 1];
            end.month--;
            if (end.month < 1) {
                end.month += 12;
                end.year--;
            }
        }
        tU.day = end.day - start.day;


        if (end.month < start.month) {
            end.month += 12;
            end.year--;
        }
        tU.month = end.month - start.month;
        tU.year  = end.year - start.year;

        return tU;
    }

    static Date getTimeSinceDate(const Date& start, const Date end) {
        return getTimeUntilDate(start, end);
    }

    long long getUnixTimestamp() const {
        tm t = {};

        t.tm_year = year - 1900;
        t.tm_mon  = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min  = minute;
        t.tm_sec  = second;

        return timegm(&t);
    }

    std::string asString(DateFormat dateFormat, bool includeTime = false) const {
        std::string result;
        result.reserve(20);

        bool isTwoDigitYear = (dateFormat == DateFormat::DDMMYY ||
                               dateFormat == DateFormat::MMDDYY ||
                               dateFormat == DateFormat::YYMMDD);

        std::string _year = isTwoDigitYear ? padZero(year % 100, 2) : padZero(year, 4);
        std::string _month = padZero(month, 2);
        std::string _day = padZero(day, 2);
        std::string _hour = padZero(hour, 2);
        std::string _minute = padZero(minute, 2);
        std::string _second = padZero(second, 2);

        switch (dateFormat) {
            default:
            case DateFormat::DDMMYY:
            case DateFormat::DDMMYYYY:
                result += _day + '-' + _month + '-' + _year;
                break;
            case DateFormat::MMDDYY:
            case DateFormat::MMDDYYYY:
                result += _month + '-' + _day + '-' + _year;
                break;
            case DateFormat::YYMMDD:
            case DateFormat::YYYYMMDD:
                result += _year + '-' + _month + '-' + _day;
                break;
        }

        if (includeTime) {
            result += ':' + _hour + '-' + _minute + '-' + _second;
        }

        return result;
    }

    int getDay() const {return day;};
    int getMonth() const {return month;}
    int getYear() const {return year;}
    int getHour() const {return hour;}
    int getMinute() const {return minute;}
    int getSecond() const {return second;}

    void setDay(int d) {day = d;}
    void setMonth(int m) {month = m;}
    void setYear(int y) {year = y;}

    // operators
    Date operator-(const Date& d2) const {
        return {
            year - d2.year,
              month - d2.month,
              day - d2.day,
              hour - d2.hour,
              minute - d2.minute,
              second - d2.second
          };
    }

    bool operator>(const Date& d2) const {
        return getUnixTimestamp() > d2.getUnixTimestamp();
    }

    bool operator>=(const Date& d2) const {
        return getUnixTimestamp() >= d2.getUnixTimestamp();
    }

    bool operator<=(const Date& d2) const {
        return getUnixTimestamp() <= d2.getUnixTimestamp();
    }

    bool operator<(const Date& d2) const {
        return getUnixTimestamp() < d2.getUnixTimestamp();
    }

    bool operator==(const Date& d2) const {
        return getUnixTimestamp() == d2.getUnixTimestamp();
    }
};


#endif //CTRACK_DATE_H
