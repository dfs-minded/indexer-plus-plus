// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "ModelUpdater.h"

using namespace System::Threading;
// clang-format off

namespace CLIInterop 
{
	ModelUpdater::ModelUpdater(Model^ modelToCall)
	{
		model = modelToCall;
		lockObj = gcnew System::Object();
	}

	ModelUpdater::~ModelUpdater()
	{
		delete model;
	}

	// Called from SE thread.
	void ModelUpdater::OnNewSearchResult(indexer::pSearchResult searchResult, bool isNewQuery)
	{
		Monitor::Enter(lockObj);
		searchResults.push_back(searchResult);
		Monitor::Exit(lockObj);

		model->OnNewSearchResult(isNewQuery);
	}

	void ModelUpdater::StatusChanged(const std::string& newStatus) 
	{
		System::String^ status = gcnew System::String(newStatus.c_str());
		model->OnNewStatus(status);
	}

	// Called from UI thread.
	indexer::pSearchResult ModelUpdater::GetSearchResult() 
	{
		indexer::pSearchResult res = nullptr;
		Monitor::Enter(lockObj);

		if (!searchResults.empty()) 
		{
			res = searchResults.back();
			searchResults.clear();
		}

		Monitor::Exit(lockObj);

		return res;
	}
}