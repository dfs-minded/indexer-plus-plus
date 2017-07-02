#pragma once

namespace CLIInterop 
{

generic <typename TVal>
public ref class CustomCacheDictionary
{
  public:
	CustomCacheDictionary(int maxElements);

	TVal TryGetValue(System::UInt64 key);

	void Add(System::UInt64 key, TVal val);

	delegate void MyUnaryFunc(TVal val);

	void ForEach(MyUnaryFunc^ func);

	void Clear();

  private:
	void RemoveOldElements();

	int maxSize;

	int nextTime;

	generic <typename TVal>
	ref class ValueInfo 
	{
		public:
			ValueInfo(int lastTimeUsed, TVal value) 
				: LastTimeUsed(lastTimeUsed),
				  StoredValue(value) {}

			int LastTimeUsed;
			TVal StoredValue;
	};

	// int in tuple stores values "last time used number".
	System::Collections::Generic::Dictionary<System::UInt64, ValueInfo<TVal>^>^ cache;
};

} // namespace CLIInterop