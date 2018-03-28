// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

namespace indexer_common {
	
	// Represents a filesystem changes updates priority in the application, namely how often do we want to
	// process updates from the filesystem.

	enum class FilesystemUpdatesPriority {
		REALTIME,
		NORMAL,
		BACKGROUND
	};

} // namespace indexer_common