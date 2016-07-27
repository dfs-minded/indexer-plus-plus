#include <fcntl.h>
#include <io.h>
#include <Shlobj.h>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "ConnectionManager.h"
#include "QueryFactory.h"
#include "SearchQuery.h"

using namespace std;

const int kMaxOutputLines = 50;

int wmain(int argc, wchar_t* argv[]) {
    wstring appName = argv[0];

    if (argc <= 1) {
        wcout << "Syntax " << appName << " [path...] [expression]\n";
        return 0;
    }

    wstring firstArg = argv[1];

    if (firstArg == L"-help" || firstArg == L"--help") {
        ifstream fs("../helpText.txt");
        stringstream buffer;
        buffer << fs.rdbuf();
        cout << buffer.str();
        return 0;
    }

	if (!IsUserAnAdmin())
	{
		wcout << "Please run console with administrator privileges and try again." << endl;
		return 0;
	}

    vector<wstring> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(wstring(argv[i]));
    }

    QueryFactory factory;
    wstring format;
    wstring outputPath;
    uSearchQuery query;

    try {
        query = factory.ParseInput(args, &format, &outputPath);
    } catch (range_error& e) {
        wcout << appName << e.what();
        return 0;
    } catch (invalid_argument& e) {
        wcout << appName << e.what();
        return 0;
    }

    ConnectionManager mgr;
    vector<wstring> res;
    bool ok = mgr.SendQuery(SerializeQuery(*query.get()), format, outputPath.empty() ? kMaxOutputLines : -1, &res);

    if (!ok) {
        wcerr << res[0] << endl;
        return 0;
    }

    if (!outputPath.empty()) {

        FILE* outputFile = _wfopen(const_cast<const wchar_t*>(outputPath.c_str()), L"w");
        _setmode(_fileno(outputFile), _O_U8TEXT);

        for (size_t i = 0; i < res.size(); ++i) {
            fwprintf(outputFile, L"%s\n", res[i].c_str());
        }

        fclose(outputFile);

        return 0;
    }

    if (res.size() > kMaxOutputLines) {
        cout << "Found " + to_string(res.size()) + "Objects. Listing first " + to_string(kMaxOutputLines);
    }

    if (res.size() == 0) wcout << "No files found";
    for (size_t i = 0; i < min(res.size(), kMaxOutputLines); ++i) {
        wcout << res[i] << endl;
    }

    return 0;
}
