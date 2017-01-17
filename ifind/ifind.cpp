// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <fcntl.h>
#include <io.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <shlobj.h>
#include <string>
#include <vector>

#include "ConnectionManager.h"
#include "UserArgsParser.h"
#include "SearchQueryBuilder.h"

#include "HelpText.h"

using std::wcout;
using std::endl;
using std::wstring;

namespace {

const size_t kMaxOutputLines = 50;

} // namespace 

int wmain(int argc, wchar_t* argv[]) {

    _setmode(_fileno(stdout), _O_U16TEXT);  // For correct Unicode output to console.

    wstring app_name = argv[0];

    if (argc <= 1) {
        wcout << "Syntax " << app_name << " [path...] [expression]\n";
        return 0;
	}

	wstring first_arg = argv[1];

	if (first_arg == L"-help" || first_arg == L"--help") {
		wcout << ifind::gHelpText;
		return 0;
    }

	if (!IsUserAnAdmin()) {
		wcout << "Please run console with administrator privileges and try again." << endl;
		return 0;
	}

	std::vector<wstring> args;
        for (auto i = 1; i < argc; ++i)
            args.push_back(wstring(argv[i]));

    wstring format;
	wstring outputPath;
	indexer_common::uSearchQuery u_query; 

	try {
      u_query = ifind::ComposeQueryFromUserInput(args, &format, &outputPath);
    } 
	catch (std::range_error& e) {
		wcout << app_name << e.what();
		return 0;
	}
	catch (std::invalid_argument& e) {
		wcout << app_name << e.what();
		return 0;
	}

	indexer_common::ConnectionManager mgr;
	std::vector<wstring> res;

	bool ok = mgr.SendQuery(SerializeQuery(*u_query), format, outputPath.empty() ? kMaxOutputLines : -1, &res);

	if (!ok) {
		std::wcerr << res[0] << endl;
		return 0;
	}

	if (!outputPath.empty()) {

		FILE* outputFile = _wfopen(const_cast<const wchar_t*>(outputPath.c_str()), L"w");
		_setmode(_fileno(outputFile), _O_U16TEXT);

		for (size_t i = 0; i < res.size(); ++i) {
			fwprintf(outputFile, L"%s\n", res[i].c_str());
		}

		fclose(outputFile);

		return 0;
	}

	if (res.size() == 0) wcout << L"No files found";

	int num_to_display = std::min(res.size(), kMaxOutputLines);

	if (res.size() == kMaxOutputLines)
		wcout << L"Listing first " + std::to_wstring(kMaxOutputLines) + L" results:" << endl;
	else
		wcout << num_to_display << " files found" << endl;

	for (size_t i = 0; i < num_to_display; ++i)
		wcout << res[i] << endl;

	return 0;
}
