#include "TestsHelperFunctions.h"

using namespace std;

SearchQuery* CopyQuery(const SearchQuery& other) {

    auto copied =
        new SearchQuery(other.Text, other.SearchDirPath, other.MatchCase, other.SizeFrom, other.SizeTo,
                        other.ExcludeHiddenAndSystem, other.ExcludeFolders, other.ExcludeFiles, other.CTimeFrom,
                        other.CTimeTo, other.ATimeFrom, other.ATimeTo, other.MTimeFrom, other.MTimeTo);

    return copied;
}
