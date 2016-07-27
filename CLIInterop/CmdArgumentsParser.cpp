#include "CmdArgumentsParser.h"

#include <string>
#include <vector>

#include "CommandlineArguments.h"

#include "InteropHelper.h"

namespace CLIInterop
{
	void CmdArgumentsParser::Init(System::Windows::StartupEventArgs^ e)
	{
            std::vector<std::wstring> args;

            for (int i = 0; i < e->Args->Length; ++i) {
                System::String ^ arg = e->Args[i]->ToString();

                if (arg->Contains(FilterDirPathArgName))
                    FilterDirPath = arg->Split('=')[1];
                else
                    args.push_back(InteropHelper::ToWstring(arg));
            }

            CommandlineArguments::Instance().Parse(args);
        }
}