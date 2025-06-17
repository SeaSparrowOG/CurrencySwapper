#include "Hooks/hooks.h"

#include "Barter/BarterHooks.h"
#include "Training/TrainingHooks.h"

namespace Hooks {
	bool Install() {
		SECTION_SEPARATOR;
		logger::info("Installing hooks..."sv);
		constexpr size_t allocSize = Barter::allocSize + Training::allocSize;
		logger::info("  >Allocated {} bytes."sv, allocSize);
		SKSE::AllocTrampoline(allocSize);

		bool nominal = true;
		if (!Barter::Install()) {
			nominal = false;
		}
		if (!Training::Install()) {
			nominal = false;
		}
		return nominal;
	}
}