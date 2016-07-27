#pragma once

#using <WindowsBase.dll>
// clang-format off

namespace CLIInterop 
{
	class SearchEngineLogListener;

	public ref class DebugLogModel
	{
	public:
		DebugLogModel();

		~DebugLogModel();

		void OnNewMessage(System::String^ msg);

		void Clear();

		System::Collections::ObjectModel::ObservableCollection<System::String^>^ LogMessages;		

	private:
		SearchEngineLogListener* listener;

		System::Windows::Threading::Dispatcher^ dispatcher;
	};
}
