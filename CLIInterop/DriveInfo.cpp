#include "DriveInfo.h"

namespace CLIInterop 
{
	DriveInfo::DriveInfo(System::String^ VolumeLabel, char VolumeName) :
		Label(VolumeLabel), Name(VolumeName)
	{
	}
}