// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Helpers.h"

#include "SearchQueryBuilder.h"

namespace indexer {

	namespace helpers {

		indexer_common::uSearchQuery CopyQuery(const indexer_common::SearchQuery& other) {
			indexer_common::SearchQueryBuilder cpy_builder;

			cpy_builder.SetSearchText(other.Text)
					   .SetSearchDirPath(other.SearchDirPath);

			if (other.MatchCase) cpy_builder.SetMatchCase();
			if (other.UseRegex) cpy_builder.SetUseRegex();

			cpy_builder.SetSizeFrom(other.SizeFrom).SetSizeTo(other.SizeTo);

			if (other.ExcludeHiddenAndSystem) cpy_builder.SetExcludeHiddenAndSystem();
			if (other.ExcludeFolders) cpy_builder.SetExcludeFolders();
			if (other.ExcludeFiles) cpy_builder.SetExcludeFiles();

			cpy_builder.SetCreationTimeFrom(other.CTimeFrom).SetCreationTimeTo(other.CTimeTo)
				.SetLastAccessTimeFrom(other.ATimeFrom).SetLastAccessTimeTo(other.ATimeTo)
				.SetModificationTimeFrom(other.MTimeFrom).SetModificationTimeTo(other.MTimeTo);

			return cpy_builder.Build();
		}

	} //namespace helpers

} // namespace indexer