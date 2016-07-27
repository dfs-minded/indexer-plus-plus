// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <vcclr.h>
#include <string>

#include "Model.h"
#include "SearchResultObserver.h"
#include "StatusObserver.h"

namespace CLIInterop {

class ModelUpdater : public SearchResultObserver, public StatusObserver {

   public:
    ModelUpdater(Model ^ model);

    virtual ~ModelUpdater();

    virtual void OnNewSearchResult(pSearchResult searchResult, bool isNewQuery) override;

    virtual void StatusChanged(const std::string& newStatus) override;

    pSearchResult GetSearchResult();

   private:
    gcroot<Model ^> model;
    std::vector<pSearchResult> searchResults;
    gcroot<System::Object ^> lockObj;
};
}