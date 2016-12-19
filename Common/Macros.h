// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "CompilerSymb.h"

#define NO_COPY(classname)                \
    classname(const classname&) = delete; \
    classname& operator=(const classname&) = delete;


#ifdef SINGLE_THREAD

#define NEW_MUTEX
#define DELETE_MUTEX
#define PLOCK_GUARD
#define PLOCK
#define UNIQUE_LOCK
#define UNLOCK
#define PUNLOCK
#define UNLOCK_AND_NOTIFY_ONE SearchWorker();
#define NOTIFY_ONE SearchWorker();

#else

#define NEW_MUTEX locker_ = new mutex();
#define DELETE_MUTEX delete locker_;

#define LOCK locker_.lock();
#define PLOCK locker_->lock();
#define PLOCK_GUARD lock_guard<mutex> lock(*locker_);

#define UNIQUE_LOCK unique_lock<mutex> locker_(*locker_);

#define UNLOCK locker_.unlock();
#define PUNLOCK locker_->unlock();
#define UNLOCK_AND_NOTIFY_ONE \
    locker_.unlock();         \
    conditional_var_->notify_one();
#define NOTIFY_ONE conditional_var_->notify_one();

#endif  // SINGLE_THREAD
