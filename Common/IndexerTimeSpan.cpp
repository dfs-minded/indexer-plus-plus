#include "IndexerTimeSpan.h"

#include "IndexerDateTime.h"

using namespace std;

IndexerTimeSpan::IndexerTimeSpan(uint64 tiks) {
    auto tmp = IndexerDateTime::TicksToFiletime(tiks);
    instance = IndexerDateTime::FiletimeToSystemtime(tmp);
}

uint64 IndexerTimeSpan::Tiks() const {
    return IndexerDateTime::SystemtimeToUInt64(instance);
}

unique_ptr<IndexerTimeSpan> IndexerTimeSpan::Diff(uint64 lhs, uint64 rhs) {
    if (rhs > lhs) swap(lhs, rhs);

    return make_unique<IndexerTimeSpan>(lhs - rhs);
}
