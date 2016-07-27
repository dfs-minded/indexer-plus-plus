#pragma once

namespace CLIInterop 
{
	public ref class DriveInfo
	{
	public:
		DriveInfo(System::String^ Label, char Name);

		System::String^ Label; 

		System::Char Name;
	};
}
