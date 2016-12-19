// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "FileInfoWrapper.h"
// clang-format off

namespace CLIInterop 
{
	ref struct ModelEnumerator: public System::Collections::Generic::IEnumerator<FileInfoWrapper^>
	{
	   public:

		property FileInfoWrapper^ Current 
		{ 
			virtual FileInfoWrapper^ get() = System::Collections::Generic::IEnumerator<FileInfoWrapper^>::Current::get 
			{ 
				return gcnew FileInfoWrapper(); 
			}
		}

		// This is required as IEnumerator<T> also implements IEnumerator.
		property Object^ Current2
		{
			virtual Object^ get() = System::Collections::IEnumerator::Current::get
			{
				return gcnew FileInfoWrapper();
			}
		};

		virtual bool MoveNext() = System::Collections::Generic::IEnumerator<FileInfoWrapper^>::MoveNext { return true; }
		virtual void Reset() = System::Collections::Generic::IEnumerator<FileInfoWrapper^>::Reset { }

		~ModelEnumerator(){};
	};
}