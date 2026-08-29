#include "Hooks/hooks.h"

#include "Barter/BarterHooks.h"
#include "Crime/CrimeHooks.h"
#include "Notification/NotificationHooks.h"
#include "Training/TrainingHooks.h"

namespace Hooks {
	bool Install() {
		SECTION_SEPARATOR;
		logger::INFO("Installing hooks..."sv);
		
		bool success = true;
		success &= Barter::Install();
		success &= Crime::InstallCrimeHooks();
		success &= Notifications::Install();
		success &= Training::Install();
		return success;
	}
}