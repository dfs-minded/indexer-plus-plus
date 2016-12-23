// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <vcclr.h>
#include <string>

#include "Model.h"
#include "SearchResultObserver.h"
#include "StatusObserver.h"
// clang-format off

namespace CLIInterop {

	class ModelUpdater : public indexer::SearchResultObserver, public indexer::StatusObserver {

	  public:
		ModelUpdater(Model^ model);

		virtual ~ModelUpdater();

		virtual void OnNewSearchResult(indexer::pSearchResult searchResult, bool isNewQuery) override;

		virtual void StatusChanged(const std::string& newStatus) override;

		indexer::pSearchResult GetSearchResult();

	  private:
		gcroot<Model^> model;
		std::vector<indexer::pSearchResult> searchResults;
		gcroot<System::Object^> lockObj;
	};
}