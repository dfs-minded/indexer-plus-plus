#pragma once

#include "CompilerSymb.h"

#define NO_COPY(classname) classname(const classname&) = delete;\
						   classname& operator= (const classname&) = delete;


#ifdef SINGLE_THREAD

#define NEW_LOCKER
#define NEW_RECURSIVE_LOCKER
#define DELETE_LOCKER
#define PLOCK_GUARD
#define P_RECURSIVE_LOCK_GUARD
#define PLOCK
#define UNIQUE_LOCK
#define UNLOCK
#define PUNLOCK
#define UNLOCK_AND_NOTIFY_ONE SearchWorker();
#define NOTIFY_ONE SearchWorker();

#else

#define NEW_LOCKER locker_ = new mutex();
#define NEW_RECURSIVE_LOCKER locker_ = new recursive_mutex();
#define DELETE_LOCKER delete locker_;

#define LOCK locker_.lock();
#define PLOCK locker_->lock();
#define PLOCK_GUARD lock_guard<mutex> lock(*locker_);
#define P_RECURSIVE_LOCK_GUARD lock_guard<recursive_mutex> lock(*locker_);

#define UNIQUE_LOCK unique_lock<mutex> locker_(*mtx_);

#define UNLOCK locker_.unlock();
#define PUNLOCK locker_->unlock();
#define UNLOCK_AND_NOTIFY_ONE locker_.unlock(); conditional_var_->notify_one();
#define NOTIFY_ONE conditional_var_->notify_one();

#endif //SINGLE_THREAD
