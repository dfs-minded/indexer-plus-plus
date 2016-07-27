#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "WindowsWrapper.h"

#include "Macros.h"

// Provides USN records for testing purposes. This class |records_| are populated by USNJournalRecordsSerializer class.

class USNJournalRecordsProvider {
   public:
    USNJournalRecordsProvider(char drive_letter);

    NO_COPY(USNJournalRecordsProvider)

    ~USNJournalRecordsProvider();

    bool GetNextMFTRecords(LPVOID buffer, DWORD buffer_size, LPDWORD bytes_returned);

    void CopyMessagesToBuffer(LPVOID& buffer, LPDWORD& bytes_returned);

    static void ImitateWatchDelay();

    bool GetNextJournalRecords(LPVOID buffer, DWORD buffer_size, LPDWORD bytes_returned);

    void AddRecord(std::unique_ptr<USN_RECORD, std::function<void(USN_RECORD*)>> record);

    bool Empty() const;

    char DriveLetter() const;

   private:
    void FindLastGroupRecord();

    char drive_letter_;

    std::vector<std::unique_ptr<USN_RECORD, std::function<void(USN_RECORD*)>>> records_;

    int next_record_;

    // Finding groups of events. As an event considered group of close in time USN records.
    int last_group_record_;
};