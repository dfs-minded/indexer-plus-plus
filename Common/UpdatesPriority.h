// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <map>

#include "typedefs.h"

namespace indexer_common {
	
	// Represents a filesystem changes updates priority in the application.

	enum class UpdatesPriority {
		REALTIME,
		NORMAL,
		BACKGROUND
	};

	const uint kMinTimeBetweenReadMs{ 500 };

	const std::map<indexer_common::UpdatesPriority, indexer_common::uint> kPriorytiToMinTimeBetweenReadMs{
		std::map<indexer_common::UpdatesPriority, indexer_common::uint>{
			{ indexer_common::UpdatesPriority::REALTIME, kMinTimeBetweenReadMs },
			{ indexer_common::UpdatesPriority::NORMAL, 10 * 1000 },
			{ indexer_common::UpdatesPriority::BACKGROUND, 60 * 1000 }
		} 
	};

} // namespace ntfs_reader