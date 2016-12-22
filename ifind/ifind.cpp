// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include <fcntl.h>
#include <io.h>
#include <fstream>
#include <iostream>

#include <shlobj.h>
#include <string>
#include <vector>

#include "ConnectionManager.h"
#include "QueryFactory.h"
#include "SearchQuery.h"

using std::wcout;
using std::endl;
using std::wstring;

const int kMaxOutputLines = 50;

int wmain(int argc, wchar_t* argv[]) {
	_setmode(_fileno(stdout), _O_U16TEXT);  // For correct Unicode output to console.
	wstring appName = argv[0];

	if (argc <= 1) {
		wcout << "Syntax " << appName << " [path...] [expression]\n";
		return 0;
	}

	wstring firstArg = argv[1];

	if (firstArg == L"-help" || firstArg == L"--help") {
		std::wifstream helpfile("helpText.txt");
		wstring line;
		while (getline(helpfile, line))
			wcout << line;
		return 0;
	}

	if (!IsUserAnAdmin()) {
		wcout << "Please run console with administrator privileges and try again." << endl;
		return 0;
	}

	std::vector<wstring> args;
	for (int i = 1; i < argc; ++i)
		args.push_back(wstring(argv[i]));

	ifind::QueryFactory factory;
	wstring format;
	wstring outputPath;
	indexer_common::uSearchQuery query;

	try {
		query = factory.ParseInput(args, &format, &outputPath);
	} catch (std::range_error& e) {
		wcout << appName << e.what();
		return 0;
	}
	catch (std::invalid_argument& e) {
		wcout << appName << e.what();
		return 0;
	}

	indexer_common::ConnectionManager mgr;
	std::vector<wstring> res;

	bool ok = mgr.SendQuery(SerializeQuery(*query.get()), format, outputPath.empty() ? kMaxOutputLines : -1, &res);

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

	int num_to_display = min(res.size(), kMaxOutputLines);

	if (res.size() == kMaxOutputLines)
		wcout << L"Listing first " + std::to_wstring(kMaxOutputLines) + L" results:" << endl;
	else
		wcout << num_to_display << " files found" << endl;

	for (size_t i = 0; i < num_to_display; ++i)
		wcout << res[i] << endl;

	return 0;
}
