#include "SortingProperty.h"

using namespace std;

SortingProperty PropertyNameToSortingPropertyEnum(string prop_name) {

    if (prop_name == "Name") return SortingProperty::SORT_NAME;
    if (prop_name == "Path") return SortingProperty::SORT_PATH;
    if (prop_name == "Size") return SortingProperty::SORT_SIZE;
    if (prop_name == "Date created") return SortingProperty::SORT_CREATION_TIME;
    if (prop_name == "Date modified") return SortingProperty::SORT_LASTWRITE_TIME;
    if (prop_name == "Date accessed") return SortingProperty::SORT_LASTACCESS_TIME;
    if (prop_name == "Extension") return SortingProperty::SORT_EXTENSION;
    if (prop_name == "Type") return SortingProperty::SORT_TYPE;

    return SortingProperty::SORT_NOTSUPPORTED_COLUMNS;
}

wstring SortingPropertyEnumToPropertyName(SortingProperty prop) {

    if (prop == SortingProperty::SORT_NAME) return L"Name";
    if (prop == SortingProperty::SORT_PATH) return L"Path";
    if (prop == SortingProperty::SORT_SIZE) return L"Size";
    if (prop == SortingProperty::SORT_CREATION_TIME) return L"Date created";
    if (prop == SortingProperty::SORT_LASTWRITE_TIME) return L"Date modified";
    if (prop == SortingProperty::SORT_LASTACCESS_TIME) return L"Date accessed";
    if (prop == SortingProperty::SORT_EXTENSION) return L"Extension";
    if (prop == SortingProperty::SORT_TYPE) return L"Type";

    return L"Not supported column";
}
