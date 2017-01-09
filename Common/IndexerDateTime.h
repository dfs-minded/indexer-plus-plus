// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>
#include "WindowsWrapper.h"

#include "Macros.h"
#include "typedefs.h"

namespace indexer_common {

    enum class DateTimeEnum { Seconds, Minutes, Days };

    class EXPORT IndexerDateTime {
       public:
        IndexerDateTime() = default;

        NO_COPY(IndexerDateTime)

        IndexerDateTime* AddSeconds(double value);
        IndexerDateTime* AddMinutes(double value);
        IndexerDateTime* AddDays(double value);
        IndexerDateTime* Add(double value, DateTimeEnum timeType);

        uint64 Ticks() const;
        uint UnixSeconds() const;

        double Seconds() const {
            return instance.wSecond;
        }
        double Minutes() const {
            return instance.wMinute;
        }
        double Hours() const {
            return instance.wHour;
        }
        double Days() const {
            return instance.wDay;
        }

        bool Equals(const IndexerDateTime& rhs, uint64 epsilon) const;
        std::string ToString() const;
        std::wstring ToWString() const;

        static IndexerDateTime* Now();
        static uint64 TicksNow();
        static FILETIME FileTimeNow();
        static SYSTEMTIME SystemTimeNow();

        static int64 MinTicks() {
            return 0;
        }
        static int64 MaxTicks() {
            return 1LL << 63;
        }

        static uint64 TicksToSeconds(const uint64& ticks);
        static uint64 FiletimeToUInt64(const FILETIME& filetime);
        static FILETIME TicksToFiletime(const uint64& ticks);
        static uint64 SystemtimeToUInt64(const SYSTEMTIME& systemtime);

        // Converts Windows ticks (filetime represented in this ticks) to the Unix timestamp.
        static uint WindowsTicksToUnixSeconds(uint64 windows_ticks);

        static uint FiletimeToUnixTime(const FILETIME& filetime);

        // Converts a system time to file time format. System time is based on Coordinated Universal Time (UTC).
        static FILETIME SystemtimeToFiletime(const SYSTEMTIME& systemtime);

        // Converts a file time to system time format. System time is based on Coordinated Universal Time (UTC).
        static SYSTEMTIME FiletimeToSystemtime(const FILETIME& fileTime);

        static FILETIME FiletimeToLocalFiletime(const FILETIME& filetime);

        // TM is based on UTC.
        static tm FiletimeToTM(const FILETIME& filetime);

       private:
        SYSTEMTIME instance;

        static const double kSecondsPer100nsInterval;

        static const uint kTicksPerSecond;

        static const uint64 kEpochDifferenceInSecond;
    };

    typedef std::shared_ptr<IndexerDateTime> pDateTime;

} // namespace indexer_common