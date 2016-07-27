#pragma once

#include <tchar.h>
#include <map>
#include <string>
#include <vector>


#include "IndexerDateTime.h"
#include "Macros.h"
#include "SearchQuery.h"

class QueryFactory {

   public:
    QueryFactory() = default;

    NO_COPY(QueryFactory)

    ~QueryFactory() = default;

    uSearchQuery ParseInput(const std::vector<std::wstring>& args, std::wstring* format, std::wstring* outputPath);

   private:
    void SetType(const std::wstring& type, bool* excludeFiles, bool* excludeFolders);

    void SetSize(const std::wstring& sizeText, int* sizeFrom, int* sizeTo);

    int TryParseSize(const std::wstring& text);

    void SetTime(const std::wstring& timeText, uint* timeFrom, uint* timeTo, IndexerDateTimeEnum timeType);

	// TODO: remove std::map at right-hand side after stopping supporing VS 2013.
	std::map<wchar_t, int> multipliers = std::map<wchar_t, int>({ { L'k', 1 }, { L'M', 1024 }, { L'G', 1024 * 1024 } });
};
