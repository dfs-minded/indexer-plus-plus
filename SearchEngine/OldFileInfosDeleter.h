#pragma once

#include <vector>

#include "Macros.h"

#include "Log.h"

class FileInfo;


// This class is used for destruction obsolete FileInfo objects when they are not needed anymore.
// It works like a smart pointer for FileInfos.

class OldFileInfosDeleter
{
public:
	OldFileInfosDeleter() = default;

	NO_COPY(OldFileInfosDeleter)

	~OldFileInfosDeleter();

	void AddItemsToDelete(std::vector<const FileInfo*>&& file_infos);

private:
	
	static Log& Logger();

	std::vector<std::vector<const FileInfo*>> file_infos_to_delete_;
};

