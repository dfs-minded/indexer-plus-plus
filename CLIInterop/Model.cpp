// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "Model.h"

#include <algorithm>
#include <list>
#include <memory>
#include <string>

#include "AsyncLog.h"
#include "EmptyLog.h"
#include "OneThreadLog.h"
#include "ConnectionManager.h"
#include "FileInfoComparatorFactory.h"
#include "../Common/Helpers.h"
#include "IndexManagersContainer.h"
#include "OutputFormatter.h"
#include "QueryProcessor.h"

#include "InteropHelper.h"
#include "ModelUpdater.h"
#include "SystemConfigFlagsWrapper.h"

using namespace std;

using namespace indexer_common;
using namespace indexer;

using namespace System;
using namespace ComponentModel;
using namespace Collections::Specialized;
using namespace System::Windows::Threading;
using namespace System::Collections::Generic;
// clang-format off

namespace CLIInterop
{
	Model::Model(System::Collections::Generic::IEnumerable<System::Char>^ selectedDrives, IIconProvider^ icons_proovider, IThumbnailProvider^ thumbnails_provider) :
		enumerator(gcnew ModelEnumerator()),
		searchResult(new pSearchResult()),
		fileInfoFactory(gcnew FileInfoWrapperFactory(250, icons_proovider, thumbnails_provider)),
		count(0)
	{
		GET_LOGGER

		dispatcher = Dispatcher::CurrentDispatcher;
		modelUpdater = new ModelUpdater(this);
		engine = new SearchEngine(modelUpdater);

		IndexManagersContainer::Instance().RegisterStatusChangeObserver(modelUpdater);

		list<char> drives;
		for each(char drive in selectedDrives)
		{
			IndexManagersContainer::Instance().AddDrive(drive);
		}

	#ifdef COMMANDLINE_SERVER
		connectionMgr = new ConnectionManager();
		pIQueryProcessor qproc(new QueryProcessor());
		connectionMgr->CreateServer(qproc);
	#endif
	}

	Model::~Model()
	{
		IndexManagersContainer::Instance().UnregisterStatusChangeObserver(modelUpdater);
		delete modelUpdater;

		delete engine;
		delete searchResult;
		delete enumerator;

	#ifdef COMMANDLINE_SERVER
		delete connectionMgr;
		pIQueryProcessor qproc(new QueryProcessor());
		connectionMgr->CreateServer(qproc);
	#endif

	};

	void Model::Filter(SearchQueryWrapper^ queryWrapper)
	{
		logger_->Info(METHOD_METADATA + L"Called.");
		auto query = queryWrapper->ToUnmanagedQuery();
		engine->SearchAsync(std::move(query));
	}

	List<String^>^ Model::Format(String^ format)
	{
		OutputFormatter fmt((*searchResult)->Files.get(), Helper::ToWstring(format));
		auto output = fmt.Format();

		auto res = gcnew List<String^>();
		for (size_t i = 0; i < output->size(); ++i)
		{
			res->Add(Helper::ToSystemString((*output)[i]));
		}
		return res;
	}

	void Model::OnDriveSelected(Char drive)
	{
		logger_->Info(METHOD_METADATA + L"Called.");
		IndexManagersContainer::Instance().AddDrive(static_cast<char>(drive));
	}

	void Model::OnDriveUnselected(Char drive)
	{
		logger_->Info(METHOD_METADATA + L"Called.");
		IndexManagersContainer::Instance().RemoveDrive(static_cast<char>(drive));
	}

	void Model::OnNewSearchResult(bool isNewQuery)
	{
		if (dispatcher->CheckAccess())
		{
			logger_->Debug(METHOD_METADATA + L"Called in dispatcher thread.");

			pSearchResult res = modelUpdater->GetSearchResult();
			if (res == nullptr) return;

			// TODO: not to clear cache on update, device some mechanism to check which elements were deleted.
			fileInfoFactory->Clear(); // UpdateCachedItems();
			*searchResult = res;
			Count = (*searchResult)->Files->size();

			logger_->Debug(METHOD_METADATA + L"SearchResult size = " + to_wstring((*searchResult)->Files->size()));
			
			NewSearchResult(isNewQuery);
			OnCollectionChanged(NotifyCollectionChangedAction::Reset);
		}
		else
		{
			dispatcher->BeginInvoke(
				DispatcherPriority::Normal,
				gcnew Action<bool>(this, &Model::OnNewSearchResult), isNewQuery);
		}
	}

	void Model::OnNewStatus(String^ status)
	{
		if (dispatcher->CheckAccess())
		{
			Status = status;
		}
		else
		{
			dispatcher->BeginInvoke(
				DispatcherPriority::Normal,
				gcnew Action<String^>(this, &Model::OnNewStatus),
				status);
		}
	}

	void Model::CheckUpdates()
	{
	#ifdef SINGLE_THREAD
		IndexManagersContainer::Instance().CheckUpdates();
	#endif
	}

	void Model::Sort(String^ propName, int direction)
	{
		string str;
		Helper::ToUnmanagedString(propName, str);
		engine->Sort(str, direction);
		OnCollectionChanged(NotifyCollectionChangedAction::Reset);
	}

	void Model::OnCollectionChanged(NotifyCollectionChangedAction action)
	{
		if (dispatcher->CheckAccess())
			CollectionChanged(this, gcnew NotifyCollectionChangedEventArgs(action));
		else
		{
			dispatcher->BeginInvoke(
				DispatcherPriority::Normal,
				gcnew Action<NotifyCollectionChangedAction>(this, &Model::OnCollectionChanged),
				action);
		}
	}

	void Model::OnCollectionChanged(NotifyCollectionChangedAction action, List<Object^>^ newItems)
	{
		if (dispatcher->CheckAccess())
			CollectionChanged(this, gcnew NotifyCollectionChangedEventArgs(action, newItems));
		else
		{
			dispatcher->BeginInvoke(
				DispatcherPriority::Normal,
				gcnew Action<NotifyCollectionChangedAction>(this, &Model::OnCollectionChanged),
				action);
		}
	}

	#pragma region INotifyPropertyChanged

	void Model::OnPropertyChanged(String^ propertyName)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(propertyName));
	}

	#pragma endregion

	FileInfoWrapper^ Model::GetFileInfoByPath(System::String^ path)
	{
		auto unmanaged_path = Helper::ToU16string(path);
		return gcnew FileInfoWrapper(IndexManagersContainer::Instance().GetFileInfoByPath(unmanaged_path), -1);
	}

	void Model::ResetThumbnailsCache()
	{
		fileInfoFactory->Clear();
	}

} // namespace CLIInterop