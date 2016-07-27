#pragma once

#include <string>

enum class SortingProperty {

    SORT_NOTSUPPORTED_COLUMNS,
    SORT_NAME,
    SORT_SIZE,
    SORT_CREATION_TIME,
    SORT_LASTACCESS_TIME,
    SORT_LASTWRITE_TIME,
    SORT_PATH,
    SORT_EXTENSION,
    SORT_TYPE
};

SortingProperty PropertyNameToSortingPropertyEnum(std::string prop_name);

std::wstring SortingPropertyEnumToPropertyName(SortingProperty prop);
