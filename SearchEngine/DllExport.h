// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

namespace indexer {

	// In order to export symblos from Common.lib, they must be used in the destination dll/exe.
	// Do not call this function. You can actually, but it does not make any sense.
	void DllExport();

} // namespace indexer
