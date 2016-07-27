#pragma once

#include <memory>

#include "NotifyNTFSChangedEventArgs.h"

// Implement this interface for listening file system changes. When an NTFS volume files changed
// (created, modified or deleted) |OnNTFSChanged| method will be called.

class NTFSChangeObserver {
   public:
    virtual ~NTFSChangeObserver() {
    }


    // Fired when file system changed. |args| provide the information what exactly has been changed.

    virtual void OnNTFSChanged(std::unique_ptr<NotifyNTFSChangedEventArgs> args){};
};
