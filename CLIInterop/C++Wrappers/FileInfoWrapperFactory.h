#pragma once

#include "FileInfoWrapper.h"
// clang-format off

namespace CLIInterop
{
	ref class FileInfoWrapperFactory
	{
		public:
            explicit FileInfoWrapperFactory(int max_elements, IIconProvider^ icons_provider, IThumbnailProvider^ thumbs_provider);

            FileInfoWrapper^ GetFileInfoWrapper(const FileInfo* fi, int index);

			void UpdateCachedItems();

            void Clear();

		private:

			void RemoveOldElements();

			int max_elements_;

			int next_time_;

			// From FileInfo object address to corresponding wrapper.
			System::Collections::Generic::Dictionary<System::Int64, FileInfoWrapper^>^ cache_;
	};
}