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
