#include "MFTReaderFactory.h"

#include <memory>

#include "CommandlineArguments.h"
#include "CompilerSymb.h"

#include "MockMFTReader.h"
#include "RawMftReader.h"
#include "WinApiMftReader.h"

using namespace std;

unique_ptr<MFTReader> MFTReaderFactory::CreateReader(char drive_letter) {
    unique_ptr<MFTReader> reader;

#ifdef WIN_API_MFT_READ
    reader = make_unique<WinApiMFTReader>(drive_letter);

#else

    if (!CommandlineArguments::Instance().ReplayFileInfosPath.empty())
        reader = make_unique<MockMFTReader>();
    else
        reader = make_unique<RawMFTReader>(drive_letter);

#endif

    return reader;
}
