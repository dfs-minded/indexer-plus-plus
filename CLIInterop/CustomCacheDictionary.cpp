#include "CustomCacheDictionary.h"

#include <algorithm>
#include <string>
#include <vector>
#include <utility>

#include "typedefs.h"

namespace CLIInterop
{

generic <typename TVal>
CustomCacheDictionary<TVal>::CustomCacheDictionary(int maxElements)
	: maxSize(maxElements),
	  nextTime(0),
	  cache(gcnew System::Collections::Generic::Dictionary<System::UInt64, ValueInfo<TVal>^>()) {
}

generic <typename TVal>
TVal CustomCacheDictionary<TVal>::TryGetValue(System::UInt64 key)
{
	ValueInfo<TVal>^ value_info;
	bool ok = cache->TryGetValue(key, value_info);
	if (ok) {
		value_info->LastTimeUsed = nextTime++;
		return value_info->StoredValue;
	}
	return TVal();
}

generic <typename TVal>
void CustomCacheDictionary<TVal>::Add(System::UInt64 key, TVal val)
{
	if (cache->Count == maxSize)
		RemoveOldElements();

	cache->Add(key, gcnew ValueInfo<TVal>(nextTime++, val));}

generic <typename TVal>
void CustomCacheDictionary<TVal>::ForEach(MyUnaryFunc^ func)
{
	for each (auto& key_val in cache)
		func(key_val.Value->StoredValue);
}

generic <typename TVal>
void CustomCacheDictionary<TVal>::Clear() {
	cache->Clear();
	nextTime = 0;
}

generic <typename TVal>
void CustomCacheDictionary<TVal>::RemoveOldElements()
{
	// Copy |LastTimeUsed| and IDs to |tmp| array.
	std::vector<std::pair<System::UInt64, int>> tmp(cache->Count);
	int i = 0;
	for each (auto& kv in cache)
	{
		tmp[i].first = kv.Value->LastTimeUsed;
		tmp[i].second = kv.Key;
		++i;
	}

	// Partially sort |tmp| array by finding median.
	std::nth_element(tmp.begin(), tmp.begin() + maxSize / 2, tmp.end());

	// Remove from |cache| all elements that are earlier to median in |tmp|.
	auto it_median = tmp.begin() + maxSize / 2;

	for (auto it = tmp.begin(); it != it_median; ++it)
		cache->Remove(it->second);
}

} // namespace CLIInterop 