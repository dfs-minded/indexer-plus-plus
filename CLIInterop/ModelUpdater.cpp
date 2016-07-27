#include "ModelUpdater.h"

using namespace System::Threading;

namespace CLIInterop {

ModelUpdater::ModelUpdater(Model ^ modelToCall) {
    model   = modelToCall;
    lockObj = gcnew System::Object();
        }

        ModelUpdater::~ModelUpdater() {
            delete model;
        }

	// Called from SE thread.
        void ModelUpdater::OnNewSearchResult(pSearchResult searchResult, bool isNewQuery) {
            Monitor::Enter(lockObj);
            searchResults.push_back(searchResult);
            Monitor::Exit(lockObj);

            model->OnNewSearchResult(isNewQuery);
        }

        void ModelUpdater::StatusChanged(const std::string& newStatus) {
            System::String ^ status = gcnew System::String(newStatus.c_str());
            model->OnNewStatus(status);
        }

	// Called from UI thread.
        pSearchResult ModelUpdater::GetSearchResult() {
            pSearchResult res = nullptr;
            Monitor::Enter(lockObj);

            if (!searchResults.empty()) {
                res = searchResults.back();
                searchResults.clear();
            }

            Monitor::Exit(lockObj);

            return res;
        }
}