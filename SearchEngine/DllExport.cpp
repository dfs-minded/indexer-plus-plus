// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "DllExport.h"

#include "SystemConfigFlags.h"
#include "ConnectionManager.h"
#include "../Common/Helpers.h"
#include "SearchQuery.h"

namespace indexer {

	void DllExport()
	{
		indexer_common::SystemConfigFlags::Instance();
		indexer_common::ConnectionManager cm;
		indexer_common::SearchQuery q;
		indexer_common::SerializeQuery(q);

		indexer_common::helpers::DirExist(L"");
		indexer_common::helpers::WStringToString(L"");
	}

} // namespace indexer
