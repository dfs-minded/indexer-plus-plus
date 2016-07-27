#include "NotifyNTFSChangedEventArgs.h"

#include <string>

using namespace std;

wstring NotifyNTFSChangedEventArgs::ToWString() const {
    return wstring(L"NTFSChangedEventArgs: ") + L"; Created = " + to_wstring(CreatedItems.size()) + L"; Deleted = " +
           to_wstring(DeletedItems.size()) + L"; Changed = " + to_wstring(ChangedItems.size());
}
