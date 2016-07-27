#include "IndexerDateTime.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>

using namespace std;

const double IndexerDateTime::kSecondsPer100nsInterval = 100.0 * 1.E-9;

const uint IndexerDateTime::kTicksPerSecond = 10000000;

const uint64 IndexerDateTime::kEpochDifferenceInSecond = 11644473600;

IndexerDateTime* IndexerDateTime::AddSeconds(double value) {
    // According to msdn, it is not recommended that you add and subtract values from the SYSTEMTIME structure to obtain
    // relative times.
    // Instead, we should:

    // 1. Convert the SYSTEMTIME structure to a FILETIME structure.
    FILETIME filetime;
    SystemTimeToFileTime(&instance, &filetime);

    // 2. Copy the resulting FILETIME structure to a ULARGE_INTEGER structure.
    ULARGE_INTEGER largeInteger;
    memcpy(&largeInteger, &filetime, sizeof(largeInteger));

    // 3. Use normal 64-bit arithmetic on the ULARGE_INTEGER value.
    largeInteger.QuadPart += static_cast<uint64>(value / kSecondsPer100nsInterval);

    // Back to FILETIME.
    memcpy(&filetime, &largeInteger, sizeof(filetime));

    // Back to SYSTEMTIME.
    FileTimeToSystemTime(&filetime, &instance);

    return this;
}

IndexerDateTime* IndexerDateTime::AddMinutes(double value) {
    return AddSeconds(value * 60);
}

IndexerDateTime* IndexerDateTime::AddDays(double value) {
    return AddMinutes(value * 60 * 24);
}

IndexerDateTime* IndexerDateTime::Add(double value, IndexerDateTimeEnum timeType) {
    switch (timeType) {
        case IndexerDateTimeEnum::Seconds:
            return AddSeconds(value);
        case IndexerDateTimeEnum::Minutes:
            return AddMinutes(value);
        case IndexerDateTimeEnum::Days:
            return AddDays(value);
        default:
            return nullptr;
    }
}

bool IndexerDateTime::Equals(const IndexerDateTime& rhs, uint64 epsilon) const {
    auto t1 = Ticks();
    auto t2 = rhs.Ticks();

    if (t1 < t2) std::swap(t1, t2);

    return t1 - t2 < epsilon;
}


std::string IndexerDateTime::ToString() const {
    return to_string(instance.wYear) + "-" + to_string(instance.wMonth) + "-" + to_string(instance.wDay) + " " +
           to_string(instance.wHour) + ":" + to_string(instance.wMinute) + ":" + to_string(instance.wSecond);
}


std::wstring IndexerDateTime::ToWString() const {
    return to_wstring(instance.wYear) + L"-" + to_wstring(instance.wMonth) + L"-" + to_wstring(instance.wDay) + L" " +
           to_wstring(instance.wHour) + L":" + to_wstring(instance.wMinute) + L":" + to_wstring(instance.wSecond);
}

uint64 IndexerDateTime::Ticks() const {
    FILETIME filetime;
    SystemTimeToFileTime(&instance, &filetime);
    return FiletimeToUInt64(filetime);
}


uint IndexerDateTime::UnixSeconds() const {
    return WindowsTicksToUnixSeconds(Ticks());
}

IndexerDateTime* IndexerDateTime::Now() {
    auto dt      = new IndexerDateTime();
    dt->instance = SystemTimeNow();
    return dt;
}

uint64 IndexerDateTime::TicksNow() {
    return FiletimeToUInt64(FileTimeNow());
}

FILETIME IndexerDateTime::FileTimeNow() {
    FILETIME filetime_now;
    SYSTEMTIME systemtime_now;

    GetLocalTime(&systemtime_now);
    SystemTimeToFileTime(&systemtime_now, &filetime_now);

    return filetime_now;
}

SYSTEMTIME IndexerDateTime::SystemTimeNow() {
    SYSTEMTIME systemtime_now;
    GetLocalTime(&systemtime_now);

    return systemtime_now;
}


uint64 IndexerDateTime::TicksToSeconds(const uint64& ticks) {
    return static_cast<uint64>(ticks * kSecondsPer100nsInterval);
}

uint64 IndexerDateTime::FiletimeToUInt64(const FILETIME& filetime) {
    // return *((uint64*)(void*)&time); another solution
    return (((uint64)filetime.dwHighDateTime) << 32) + (uint64)filetime.dwLowDateTime;
}

const int64 lowBitMask = (1LL << 32) - 1;
// const int64 lowBitMask = 0xFFFFFFFF; other solution

FILETIME IndexerDateTime::TicksToFiletime(const uint64& ticks) {
    FILETIME ft;
    ft.dwHighDateTime = ticks >> 32;
    ft.dwLowDateTime  = ticks & lowBitMask;
    return ft;
}

uint64 IndexerDateTime::SystemtimeToUInt64(const SYSTEMTIME& systemtime) {
    FILETIME fileTime = SystemtimeToFiletime(systemtime);
    return FiletimeToUInt64(fileTime);
}

uint IndexerDateTime::WindowsTicksToUnixSeconds(uint64 windows_ticks) {
    return static_cast<uint>(windows_ticks / kTicksPerSecond - kEpochDifferenceInSecond);
}

uint IndexerDateTime::FiletimeToUnixTime(const FILETIME& filetime) {
    return WindowsTicksToUnixSeconds(FiletimeToUInt64(filetime));
}

FILETIME IndexerDateTime::SystemtimeToFiletime(const SYSTEMTIME& systemtime) {
    FILETIME fileTime;
    SystemTimeToFileTime(&systemtime, &fileTime);
    return fileTime;
}

SYSTEMTIME IndexerDateTime::FiletimeToSystemtime(const FILETIME& filetime) {
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&filetime, &systemTime);
    return systemTime;
}

tm IndexerDateTime::FiletimeToTM(const FILETIME& filetime) {
    struct tm res;

    SYSTEMTIME systemtime = FiletimeToSystemtime(filetime);

    memset(&res, 0, sizeof(struct tm));

    res.tm_mday = systemtime.wDay;
    res.tm_mon  = systemtime.wMonth - 1;
    res.tm_year = systemtime.wYear - 1900;

    res.tm_sec  = systemtime.wSecond;
    res.tm_min  = systemtime.wMinute;
    res.tm_hour = systemtime.wHour;

    mktime(&res);

    return res;
}

FILETIME IndexerDateTime::FiletimeToLocalFiletime(const FILETIME& filetime) {
    FILETIME res;
    FileTimeToLocalFileTime(&filetime, &res);
    return res;
}
