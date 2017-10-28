// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

namespace indexer_common {
	
	// Represents a filesystem changes updates priority in the application.

	enum class UpdatesPriority {
		REALTIME,
		NORMAL,
		BACKGROUND
	};

} // namespace ntfs_reader