#include "NotificationHooks.h"

#include "CurrencyManager/CurrencyManager.h"

namespace Hooks::Notifications
{
	bool Install() {
		return NotificationPatch::Install();
	}

	bool NotificationPatch::Install() {
		logger::INFO("  >Installing the Notification hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(16127), 0x182 };
		if (!REL::Pattern<"E8">().match(target.address())) {
			logger::CRITICAL("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = REL::GetTrampoline();
		_itemAddedNotification = trampoline.write_call<5>(target.address(), &ItemAddedNotification);
		return true;
	}

	inline void NotificationPatch::ItemAddedNotification(RE::TESForm* a_item, 
		uint32_t a_count, 
		bool a_3, 
		bool a_4, 
		char* a_name)
	{
		// A note on this. The vanilla game does have a "Gold" default object, but
		// it is NEVER referenced. Instead, IsGold pulls directly from the formid (0xF)
		// and thus I decided this is the best way to handle this.
		static auto* goldForm = RE::TESForm::LookupByID<RE::TESForm>(0xF);
		if (!CurrencyManager::SupressingGoldNotifications() || goldForm != a_item) {
			_itemAddedNotification(a_item, a_count, a_3, a_4, a_name);
		}
	}
}
