#pragma once

#using <PresentationFramework.dll>

namespace CLIInterop
{
	public ref class CmdArgumentsParser
	{
	public:

		static void Init(System::Windows::StartupEventArgs^ e);

		// On the right click in windows explorer context menu on the folder, run Indexer++ with this directory filter.
                static System::String ^ FilterDirPath = System::String::Empty;

               private:
                literal System::String ^ FilterDirPathArgName = "filterDirPath";
         };
}