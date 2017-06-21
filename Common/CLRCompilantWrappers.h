#pragma once

#include <atomic>
#include <mutex>

#include "Macros.h"

// Structures in this file provide wrappers for /clr or /clr:pure non-compilable types.

namespace indexer_common {

    struct BoolAtomicWrapper : public std::atomic<bool> {
        explicit BoolAtomicWrapper(bool val) {
            this->store(val);
        }

        NO_COPY(BoolAtomicWrapper)
    };

    struct MutexWrapper : public std::mutex {
        NO_COPY(MutexWrapper)
    };

} // namespace indexer_common