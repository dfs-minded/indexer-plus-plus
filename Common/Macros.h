// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "CompilerSymb.h"

namespace indexer_common {

#define NO_COPY(classname)                \
        classname(const classname&) = delete; \
        classname& operator=(const classname&) = delete;


#ifdef SINGLE_THREAD

#define NEW_MUTEX
#define DELETE_MUTEX

#define PLOCK
#define PUNLOCK

#define PLOCK_GUARD
#define UNIQUE_LOCK
#define UNIQUE_UNLOCK

#define NOTIFY_ONE SearchWorker();

#else

#define NEW_MUTEX mtx_ = new std::mutex();
#define DELETE_MUTEX delete mtx_;

#define PLOCK mtx_->lock();
#define PUNLOCK mtx_->unlock();

#define PLOCK_GUARD std::lock_guard<std::mutex> lock(*mtx_);
#define UNIQUE_LOCK std::unique_lock<std::mutex> lock(*mtx_);
#define UNIQUE_UNLOCK lock.unlock();

#define NOTIFY_ONE conditional_var_->notify_one();

#endif  // SINGLE_THREAD

} // namespace indexer_common