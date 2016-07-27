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