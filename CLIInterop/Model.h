// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#using <WindowsBase.dll>

#include "CompilerSymb.h"
#include "ConnectionManager.h"
#include "Log.h"
#include "SearchEngine.h"
#include "SearchResult.h"

#include "FileInfoWrapper.h"
#include "FileInfoWrapperFactory.h"
#include "ModelEnumerator.h"
#include "SearchQueryWrapper.h"
#include "WindowState.h"
// clang-format off

namespace CLIInterop 
{
	class ModelUpdater;
	enum SortFileInfoPropety;

	// The model layer for the GUI. Wraps the access to the C++ model and provides data virtualization, so that needed
	// managed objects created upon request from the underlying unmanaged objects.

	public ref class Model: 
		public System::Collections::IList,
		public System::Collections::Generic::IList<FileInfoWrapper^>,
		public System::Collections::Specialized::INotifyCollectionChanged, 
		public System::ComponentModel::INotifyPropertyChanged
	{
	public:

		Model(System::Collections::Generic::IEnumerable<System::Char>^ selectedDrives, IIconProvider^ icons_proovider, IThumbnailProvider^ thumbnails_provider);
		~Model();

		void Filter(SearchQueryWrapper^ query);

		void Sort(System::String^ propName, int direction);

		System::Collections::Generic::List<System::String^>^ Format(System::String^ format);

		void OnNewSearchResult(bool isNewQuery);

		void CheckUpdates();

		FileInfoWrapper^ GetFileInfoByPath(System::String^ path);

		void ResetThumbnailsCache();

		void OnDriveSelected(System::Char drive);

		void OnDriveUnselected(System::Char drive);

		void OnNewStatus(System::String^ newStatus);

		property System::String^ Status 
		{ 
			virtual System::String^ get() { return status; } 
			virtual void set(System::String^ val) { status = val; OnPropertyChanged("Status"); }
		}

		event System::Action<bool>^ NewSearchResult;

		void OnWindowStateChanged(WindowState state);
	private:

		// C++ types are not supported in managed C++ classes, so here we can use only object pointers.
		ModelEnumerator^ enumerator;
		System::Windows::Threading::Dispatcher^ dispatcher;

		indexer::pSearchResult* searchResult;
		indexer::SearchEngine* engine;

		ModelUpdater* modelUpdater;
		indexer_common::ConnectionManager* connectionMgr;

		size_t count;
		System::String^ status;
		indexer_common::Log* logger_;

		FileInfoWrapperFactory^ fileInfoFactory;


		#pragma region IList, IList<FileInfoWrapper^>
	
		public:
		
			#pragma region Misc
		
			virtual property Object^ SyncRoot
			{ 
				Object^ get() { return this; } 
			}
			virtual property bool IsSynchronized 
			{
				bool get() { return false; }
			}
			virtual property bool IsReadOnly 
			{
				bool get() { return false; }
			}
			virtual property bool IsFixedSize 
			{  
				bool get() { return false; }
			}

			#pragma endregion

			#pragma region do not need implementation

			virtual void Add(FileInfoWrapper^ item){ throw gcnew System::NotSupportedException; }
			virtual int Add(Object^ item){ throw gcnew System::NotSupportedException; }
			virtual void Clear(){ throw gcnew System::NotSupportedException; }
			virtual bool Contains(Object^ item){ return true; }
			virtual void CopyTo(cli::array<FileInfoWrapper^ >^ array, int arrayIndex){ throw gcnew System::NotSupportedException; }
			virtual void CopyTo(System::Array^ array, int arrayIndex){ throw gcnew System::NotSupportedException; }
			virtual void Insert(int index, FileInfoWrapper^ item){ throw gcnew System::NotSupportedException; }
			virtual void Insert(int index, Object^ item){ throw gcnew System::NotSupportedException; }
			virtual bool Remove(FileInfoWrapper^ item){ throw gcnew System::NotSupportedException; }
			virtual void Remove(Object^ item){ throw gcnew System::NotSupportedException; }
			virtual void RemoveAt(int index){ throw gcnew System::NotSupportedException; }

			#pragma endregion

			virtual int IndexOf(FileInfoWrapper^ item) { return item->Index; }
			virtual int IndexOf(Object^ item) { return IndexOf((FileInfoWrapper^)item); }
			virtual bool Contains(FileInfoWrapper^ item) { return true; }
	
			property int Count
			{ 
				virtual int get() { return count; }
				virtual void set(int val) { count = val; OnPropertyChanged("Count"); }
			}

			property FileInfoWrapper^ default[int]
			{
				virtual FileInfoWrapper^ get(int index)
				{		
					const indexer_common::FileInfo* fi = nullptr;
					if (index < static_cast<int>((*(*searchResult)->Files).size()))
						fi = (*(*searchResult)->Files)[index]; 
					return fileInfoFactory->GetFileInfoWrapper(fi, index);
				}
				virtual void set(int index, FileInfoWrapper^ value) { throw gcnew System::NotImplementedException; }
			}

			property Object^ default2[int]
			{
				virtual Object^ get(int index) = 
					System::Collections::IList::default::get { return default[index]; };
				virtual void set(int index, Object^ item) =
					System::Collections::IList::default::set { throw gcnew System::NotImplementedException; };
			}

		protected:

			virtual System::Collections::Generic::IEnumerator<FileInfoWrapper^>^ GetEnumerator() =
				System::Collections::Generic::IEnumerable<FileInfoWrapper^>::GetEnumerator { return enumerator; }

				virtual System::Collections::IEnumerator^ GetEnumerator2() =
				System::Collections::IEnumerable::GetEnumerator { return GetEnumerator(); }

		#pragma endregion


		#pragma region INotifyCollectionChanged

		public:
			virtual event System::Collections::Specialized::NotifyCollectionChangedEventHandler^ CollectionChanged;
		
		private:
			void OnCollectionChanged(System::Collections::Specialized::NotifyCollectionChangedAction action);
			void OnCollectionChanged(System::Collections::Specialized::NotifyCollectionChangedAction action,
				System::Collections::Generic::List<Object^>^ newItems);

		#pragma endregion


		#pragma region INotifyPropertyChanged

		public:
			virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;
	
		protected:
			virtual void OnPropertyChanged(System::String^ propertyName);

		#pragma endregion
	
	};

} // namespace CLIInterop 