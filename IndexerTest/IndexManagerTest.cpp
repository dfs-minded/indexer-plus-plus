// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include <memory>

#include "Helpers.h"

#include "CommandlineArguments.h"
#include "IndexManagersContainer.h"
#include "MockIndexChangeObserver.h"
#include "MockSearchResultObserver.h"
#include "SearchEngine.h"
#include "SearchQueryBuilder.h"

namespace indexer {

    class IndexManagerTest : public testing::Test {

       public:
        void SetUp() override {
            u_default_query = indexer_common::SearchQueryBuilder().Build();
			engine_ = std::make_unique<SearchEngine>(&mock_search_res_observer);

            // Search engine can change NotifyIndexChangedEventArgs and cause side effects.
            IndexManagersContainer::Instance().UnregisterIndexChangeObserver(engine_.get());

            IndexManagersContainer::Instance().RegisterIndexChangeObserver(&mock_index_change_observer);
        }

        void TearDown() override {
            IndexManagersContainer::Instance().UnregisterIndexChangeObserver(&mock_index_change_observer);
        }

        char drive_letter_ = 'Z';
		indexer_common::uSearchQuery u_default_query;
        std::unique_ptr<SearchEngine> engine_;
        MockIndexChangeObserver mock_index_change_observer;
        MockSearchResultObserver mock_search_res_observer;
    };

	// Test create
    TEST_F(IndexManagerTest, SimpleFileCreate) {

		using indexer_common::CommandlineArguments;

        CommandlineArguments::Instance().RawMFTPath = L"SerializedRawMFT/Disk_Z_RawMFT_11_25_16.txt";
        CommandlineArguments::Instance().ReplayUSNRecPath = L"SerializedUSNRecordsFiles/OneAction/Create.txt";

        IndexManagersContainer::Instance().AddDrive(drive_letter_);

        // The first call of CheckUpdates() loads MFT.

        // Uncomment #define SINGLE_THREAD in CompilerSymblos.h before compilation!
        const_cast<IndexManager*>(IndexManagersContainer::Instance().GetIndexManager(drive_letter_))->CheckUpdates();
        EXPECT_EQ(107, mock_index_change_observer.IndexChangedArgs->NewItems.size());

        // The second call loads USN journal messages from Create.txt.
        const_cast<IndexManager*>(IndexManagersContainer::Instance().GetIndexManager(drive_letter_))->CheckUpdates();
        EXPECT_EQ(1, mock_index_change_observer.IndexChangedArgs->NewItems.size());  // New file must be marked as added.
        EXPECT_EQ(0, mock_index_change_observer.IndexChangedArgs->OldItems.size());
        EXPECT_EQ(0, mock_index_change_observer.IndexChangedArgs->ChangedItems.size());

        auto actual = mock_index_change_observer.IndexChangedArgs->NewItems[0]->GetName();
        // TODO: problem with VS unicode literals compilation.
        auto expected = __L__(L"Новый Text Document.txt");
        EXPECT_EQ(expected, actual);
    }

} // namespace indexer