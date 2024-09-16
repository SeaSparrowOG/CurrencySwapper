#include "hooks.h"

namespace Hooks {
	bool BarterHooks::Install()
	{
		SKSE::AllocTrampoline(112); // 7 * 14
		auto& trampoline = SKSE::GetTrampoline();

		//1.6.1170 -> 1408ec1d9 (Actor::GetGoldAmount)
		REL::Relocation<std::uintptr_t> playerGoldTarget{ REL::ID(50957), 0x69 };
		_getPlayerGold = trampoline.write_call<5>(playerGoldTarget.address(), &GetPlayerGold);

		//1.6.1170 -> 1408ec2a9 (ExtraContainerChanges__Data::GetGoldAmount)
		REL::Relocation<std::uintptr_t> vendorGoldTarget{ REL::ID(50957), 0x139 };
		_getVendorGold = trampoline.write_call<5>(vendorGoldTarget.address(), &GetVendorGold);

		//1.6.1170 -> 1408eb997
		REL::Relocation<std::uintptr_t> saleGoldTarget{ REL::ID(50951), 0x257 };
		_getGoldFromSale = trampoline.write_call<5>(saleGoldTarget.address(), &GetGoldFromSale);

		//1.6.1170 -> 1408eb861
		REL::Relocation<std::uintptr_t> purchaseGoldTarget{ REL::ID(50951), 0x121 };
		_getGoldFromPurchase = trampoline.write_call<5>(purchaseGoldTarget.address(), &GetGoldFromPurchase);

		//1.6.1170 -> 1408ebb51
		REL::Relocation<std::uintptr_t> rawDealTarget{ REL::ID(50952), 0xB1 };
		_processRawDeal = trampoline.write_call<5>(rawDealTarget.address(), &ProcessRawDeal);

		//1.6.1170 -> 1408eb8e7 (ShowHUDMessage)
		REL::Relocation<std::uintptr_t> rejectedDealTarget{ REL::ID(50951), 0x1A7 };
		_processRejectedDeal = trampoline.write_call<5>(rejectedDealTarget.address(), &ProcessRejectedDeal);

		//1.6.1170 -> 1408ec467
		REL::Relocation<std::uintptr_t> resetVendorInformationTarget{ REL::ID(50957), 0x2F7 };
		_recalcVendorGold = trampoline.write_call<5>(resetVendorInformationTarget.address(), &RecalcVendorGold);

		//1.6.1170 -> 1408ebf20
		REL::Relocation<std::uintptr_t> customBarterTarget{ REL::ID(50955), 0x21 };
		_customBarterMenu = trampoline.write_call<5>(customBarterTarget.address(), &CustomBarterMenu);

		auto* sGold = RE::GameSettingCollection::GetSingleton()->GetSetting("sGold");
		if (!sGold) return false;
		defaultReplacement = sGold->GetString();

		auto* eventHolder = RE::UI::GetSingleton();
		if (!eventHolder)
			return false;

		eventHolder->AddEventSink(this);
		return true;
	}

	void BarterHooks::RevertCurrency()
	{
		RE::TESForm* oldCurrency = nullptr;
		if (currency) oldCurrency = currency;

		currency = nullptr;
		customRevert.QueueEvent(oldCurrency);
	}

	bool BarterHooks::SetCurrency(RE::TESForm* a_newCurrency)
	{
		if (!a_newCurrency) return false;
		auto* newCurrency = static_cast<RE::TESBoundObject*>(a_newCurrency);
		if (!newCurrency) return false;

		auto* oldCurrency = currency;
		this->currency = newCurrency;

		currencySwap.QueueEvent(oldCurrency, currency);
		return true;
	}

	void BarterHooks::RegisterFormForAllEvents(RE::TESForm* a_form)
	{
		if (!a_form) return;

		customRevert.Register(a_form);
		customPurchase.Register(a_form);
		customSale.Register(a_form);
		customPurchaseFail.Register(a_form);
		currencySwap.Register(a_form);
		customBarterMenu.Register(a_form);
	}

	void BarterHooks::UnRegisterFormForAllEvents(RE::TESForm* a_form)
	{
		if (!a_form) return;

		customRevert.Unregister(a_form);
		customPurchase.Unregister(a_form);
		customSale.Unregister(a_form);
		customPurchaseFail.Unregister(a_form);
		currencySwap.Unregister(a_form);
		customBarterMenu.Unregister(a_form);
	}

	RE::TESForm* BarterHooks::GetCurrency()
	{
		if (this->currency) {
			return this->currency->As<RE::TESForm>();
		}
		return nullptr;
	}

	RE::BSEventNotifyControl BarterHooks::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
#define continueEvent RE::BSEventNotifyControl::kContinue
		if (!currency) return continueEvent;
		if (!a_event || !a_event->opening) return continueEvent;
		if (a_event->menuName != RE::BarterMenu::MENU_NAME) return continueEvent;
		
		std::string currencyName = currency->GetName(); 

		auto* menu = RE::UI::GetSingleton()->GetMovieView(RE::BarterMenu::MENU_NAME).get();
		if (!menu) return continueEvent;

		std::string pathToPlayerLabel = "_root.Menu_mc.BottomBar_mc.PlayerInfoCard_mc.PlayerGoldLabel";
		std::string pathToVendorLabel = "_root.Menu_mc.BottomBar_mc.PlayerInfoCard_mc.VendorGoldLabel";
		if (RE::TESDataHandler::GetSingleton()->LookupLoadedModByName("SkyUI_SE.esp")) {
			pathToPlayerLabel = "_root.Menu_mc.bottomBar.playerInfoCard.PlayerGoldLabel";
			pathToVendorLabel = "_root.Menu_mc.bottomBar.playerInfoCard.VendorGoldLabel";
		}

		RE::GFxValue var;
		menu->GetVariable(&var, pathToPlayerLabel.c_str());
		if (var.IsUndefined()) {
			return continueEvent;
		}

		var.SetText(currencyName.c_str());
		menu->GetVariable(&var, pathToVendorLabel.c_str());
		if (var.IsUndefined()) {
			return continueEvent;
		}

		currencyName = "Vendor " + currencyName;
		var.SetText(currencyName.c_str());
		return continueEvent;
#undef continueEvent
	}

	//Hook code
	int BarterHooks::GetPlayerGold(RE::Actor* a_player)
	{
		if (currency) {
			if (a_player->GetInventoryCounts().contains(currency)) {
				return a_player->GetInventoryCounts().at(currency);
			}
			else {
				return 0;
			}
		}
		return _getPlayerGold(a_player);
	}

	int BarterHooks::GetVendorGold(RE::InventoryChanges* a_changes)
	{
		if (!currency) return _getVendorGold(a_changes);

		for (auto* entry : *a_changes->entryList) {
			if (entry->object == currency) {
				return entry->countDelta;
			}
		}
		return 0;
	}

	RE::TESForm* BarterHooks::GetGoldFromSale(int a_formID)
	{
		if (currency) {
			customSale.QueueEvent(currency);
			return currency;
		}
		return _getGoldFromSale(a_formID);
	}

	void BarterHooks::GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges, RE::Actor* a_buyer, int a_value, RE::ItemList* param_4)
	{
		if (currency) {
			GoldRemovedMessage(currency->As<RE::TESForm>(), a_value, false, true, "");
			MoveGoldBetweenContainers(a_inventoryChanges, new uint32_t, a_buyer, currency->As<RE::TESForm>(), a_value, 4, 0, param_4, 0, 0);
			customPurchase.QueueEvent(currency);
		}
		else {
			return _getGoldFromPurchase(a_inventoryChanges, a_buyer, a_value, param_4);
		}
	}

	void BarterHooks::GoldRemovedMessage(RE::TESForm* a_formToRemove, int a_ammount, bool arg3, bool arg4, const char* arg5)
	{
		using func_t = decltype(&BarterHooks::GoldRemovedMessage);
		static REL::Relocation<func_t> func{ REL::ID(51636) };
		return func(a_formToRemove, a_ammount, arg3, arg4, arg5);
	}

	uint32_t* BarterHooks::MoveGoldBetweenContainers(RE::InventoryChanges* a_inventoryChanges, uint32_t* param_2, RE::Actor* a_actor, RE::TESForm* a_form,
								   uint64_t a_concatResult, int arg6, long long** arg7, RE::ItemList* arg8, long long** arg9, long long **arg10)
	{
		using func_t = decltype(&BarterHooks::MoveGoldBetweenContainers);
		static REL::Relocation<func_t> func{ REL::ID(16059) };
		return func(a_inventoryChanges, param_2, a_actor, a_form, a_concatResult, arg6, arg7, arg8, arg9, arg10);
	}

	void BarterHooks::ProcessRawDeal(uint64_t* param_1, const char* a_message, uint64_t a_itemValue, uint64_t a_merchantGold) {
		if (currency) {
			std::string currencyName = currency->GetName();
			defaultMessage = a_message;
			clib_util::string::replace_all(defaultMessage, defaultReplacement, currencyName);
			a_message = defaultMessage.c_str();
		}
		return _processRawDeal(param_1, a_message, a_itemValue, a_merchantGold);
	}

	void BarterHooks::ProcessRejectedDeal(const char* a_message, const char* a_functionName, uint64_t a_value)
	{
		if (currency) {
			std::string currencyName = currency->GetName();
			defaultNotEnoughGoldMessage = a_message;
			clib_util::string::replace_all(defaultNotEnoughGoldMessage, defaultReplacement, currencyName);
			a_message = defaultNotEnoughGoldMessage.c_str();
		}
		_processRejectedDeal(a_message, a_functionName, a_value);

		if (currency) {
			customPurchaseFail.QueueEvent(currency);
		}
	}

	void BarterHooks::RecalcVendorGold(void* param_1, const char* a_function, void* param_2)
	{
		_recalcVendorGold(param_1, a_function, param_2);

		if (currency) {
			auto* menu = RE::UI::GetSingleton()->GetMovieView(RE::BarterMenu::MENU_NAME).get();
			if (!menu) return;

			RE::GFxValue var;
			if (RE::TESDataHandler::GetSingleton()->LookupLoadedModByName("SkyUI_SE.esp")) {
				menu->GetVariable(&var, "_root.Menu_mc.bottomBar.playerInfoCard.VendorGoldLabel");
			}
			else {
				menu->GetVariable(&var, "_root.Menu_mc.BottomBar_mc.PlayerInfoCard_mc.VendorGoldLabel");
			}

			if (var.IsUndefined()) return;
			defaultVendorInformation = "Vendor " + std::string(currency->GetName());
			var.SetText(defaultVendorInformation.c_str());
		}
	}

	void* BarterHooks::CustomBarterMenu(RE::TESObjectREFR* a_actor, void* arg2)
	{
		if (a_actor && currency && a_actor->As<RE::Actor>()) {
			customBarterMenu.QueueEvent(a_actor->As<RE::Actor>());
		}
		return _customBarterMenu(a_actor, arg2);
	}
}