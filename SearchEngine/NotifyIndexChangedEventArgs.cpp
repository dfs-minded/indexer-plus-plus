#include "NotifyIndexChangedEventArgs.h"

#include <string>

#include "FileInfo.h"

using namespace std;

NotifyIndexChangedEventArgs::NotifyIndexChangedEventArgs(vector<const FileInfo*>&& new_items,
                                                         vector<const FileInfo*>&& old_items,
                                                         vector<const FileInfo*>&& changed_items)
    : NewItems(new_items), OldItems(old_items), ChangedItems(changed_items) {
}

wstring NotifyIndexChangedEventArgs::ToWString() const {

    return L"IndexChangedEventArgs: New = " + to_wstring(NewItems.size()) + L"; Old = " + to_wstring(OldItems.size()) +
           L"; Changed = " + to_wstring(ChangedItems.size());
}
