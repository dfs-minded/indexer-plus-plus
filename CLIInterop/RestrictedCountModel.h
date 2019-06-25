#pragma once

#include "Model.h"

namespace CLIInterop {

public
ref class RestrictedCountModel : public Model {
   public:
    RestrictedCountModel(System::Collections::Generic::IEnumerable<System::Char>^ selectedDrives,
                         IIconProvider^ icons_proovider, IThumbnailProvider^ thumbnails_provider)
        : Model(selectedDrives, icons_proovider, thumbnails_provider) {
    }

    void ActivateRestriction(int custom_count) {
        restriction_activated = true;
		restricted_count = custom_count;
        OnPropertyChanged("Count"); 
	}

	void DeactivateRestriction() {
        restriction_activated = false;
        OnPropertyChanged("Count"); 
    }

     property int Count {
         int get() override {
             return restriction_activated ? System::Math::Min(restricted_count, Model::Count) : Model::Count;
         }
     }

    private:
     bool restriction_activated = false;
     int restricted_count;
 };

}  // namespace CLIInterop