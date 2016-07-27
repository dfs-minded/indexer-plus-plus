#pragma once

// Inherit from this class to listen volumes status change. Designed for ModelUpdater
// to pass the newStatus string to UI.

class StatusObserver {

   public:
    virtual void StatusChanged(const std::string& new_status) = 0;

    virtual ~StatusObserver() {
    }
};
