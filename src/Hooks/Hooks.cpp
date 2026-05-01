#include "Hooks/hooks.h"

#include "Barter/BarterHooks.h"
#include "Crime/CrimeHooks.h"
#include "Training/TrainingHooks.h"

namespace Hooks {
	bool Install() {
		SECTION_SEPARATOR;
		logger::info("Installing hooks..."sv);
		constexpr size_t allocSize = Barter::allocSize + Crime::allocSize + Training::allocSize;
		logger::info("  >Allocated {} bytes."sv, allocSize);
		SKSE::AllocTrampoline(allocSize);

		bool success = true;
		success &= Barter::Install();
		success &= Crime::InstallCrimeHooks();
		success &= Training::Install();
		return success;
	}
}