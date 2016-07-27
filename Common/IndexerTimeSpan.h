#pragma once

#include <memory>
#include "WindowsWrapper.h"

#include "typedefs.h"

class IndexerTimeSpan {
   public:
    explicit IndexerTimeSpan(uint64 tiks);

    ~IndexerTimeSpan() = default;

    uint64 Tiks() const;

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

    // Finds difference between two nanosecond time representations and returns a IndexerTimeSpan object.
    static std::unique_ptr<IndexerTimeSpan> Diff(uint64 lhs, uint64 rhs);

   private:
    SYSTEMTIME instance;
};
