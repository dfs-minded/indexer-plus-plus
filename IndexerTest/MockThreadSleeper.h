// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "typedefs.h"
#include "INTFSChangesUpdatesTimer.h"

namespace indexer_test {

class MockThreadSleeper : public ntfs_reader::ISleeper {
  public:
	MOCK_CONST_METHOD1(Sleep, void(indexer_common::uint time_ms));
	MOCK_CONST_METHOD0(GetTickCount, indexer_common::uint64());
};

} // namespace indexer_test
