#pragma once

namespace Hooks {
	class BarterHooks : public ISingleton<BarterHooks>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
	public:
		bool Install();

		void         RevertCurrency();
		bool         SetCurrency(RE::TESForm* a_newCurrency);
		void         RegisterFormForAllEvents(RE::TESForm* a_form);
		void         UnRegisterFormForAllEvents(RE::TESForm* a_form);
		RE::TESForm* GetCurrency();

		//Papyrus registration set.
		static inline SKSE::RegistrationSet<RE::TESForm*> customRevert{ "OnCurrencyRevert"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*> customPurchase{ "OnCustomPurchase"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*> customSale{ "OnCustomSale"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*> customPurchaseFail{ "OnCustomCurrencyFail"sv };
		static inline SKSE::RegistrationSet<RE::Actor*>   customBarterMenu{ "OnCustomBarterMenu"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*, RE::TESForm*> currencySwap{ "OnCurrencySwap"sv };

	private:
		RE::BSEventNotifyControl ProcessEvent(
			const RE::MenuOpenCloseEvent* a_event,
			RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;

		// Returns the player's total CURRENCY, based on what the actual currency currently is.
		static int          GetPlayerGold(RE::Actor* a_player);
		// Returns the vendor's total CURRENCY, again based on the actual currency.
		static int          GetVendorGold(RE::InventoryChanges* a_changes);
		// Function that moves gold from the player to the merchant when a purchase is complete.
		static void         GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
			                     RE::Actor* a_buyer, int a_value, RE::ItemList* unknown);
		// Function that moves gold from the merchant's inventory to the player's when a sale is complete.
		static RE::TESForm* GetGoldFromSale(int a_formID);
		// "Raw Deals" are deals where the vendor has less currency than the value of the thing being sold.
		static void         ProcessRawDeal(uint64_t* param_1, const char* a_message, uint64_t a_concatResult, uint64_t param_4);
		// "Rejected Deals" are deals where the player has less currency than the value of the thing being sold.
		static void         ProcessRejectedDeal(const char* a_message, const char* a_functionName, uint64_t a_value);
		static void         RecalcVendorGold(void* param_1, const char* a_function, void* param_2);
		static void*        CustomBarterMenu(RE::TESObjectREFR* a_actor, void* arg2);

		// Original function calls.
		inline static REL::Relocation<decltype(&ProcessRejectedDeal)> _processRejectedDeal;
		inline static REL::Relocation<decltype(&ProcessRawDeal)>      _processRawDeal;
		inline static REL::Relocation<decltype(&GetPlayerGold)>       _getPlayerGold;
		inline static REL::Relocation<decltype(&GetVendorGold)>       _getVendorGold;
		inline static REL::Relocation<decltype(&GetGoldFromSale)>     _getGoldFromSale;
		inline static REL::Relocation<decltype(&GetGoldFromPurchase)> _getGoldFromPurchase;
		inline static REL::Relocation<decltype(&RecalcVendorGold)>    _recalcVendorGold;
		inline static REL::Relocation<decltype(&CustomBarterMenu)>    _customBarterMenu;


		// Training Menu specific hooks
		// Initial menu setup. This function sets the currency count and name (bit hacky).
		static void  TrainingMenuSetup(RE::TrainingMenu* a_this);
		static INT64 GetTrainingUpdateGold(RE::Actor* a_this);
		static INT64 GetTrainingTrainGold(RE::Actor* a_this);
		static void  GetGoldFromTraining(RE::Actor* a_buyer, RE::Actor* a_trainer, int a_amount);

		// Original function calls.
		inline static REL::Relocation<decltype(&TrainingMenuSetup)>     _trainingMenuSetup;
		inline static REL::Relocation<decltype(&GetTrainingUpdateGold)> _getTrainingUpdateGold;
		inline static REL::Relocation<decltype(&GetTrainingTrainGold)>  _getTrainingTrainGold;
		inline static REL::Relocation<decltype(&GetGoldFromTraining)>   _getGoldFromTraining;

		// Current currency. If nullptr, gold is used. Otherwise it is whatever form is specified.
		inline static RE::TESBoundObject* currency{ nullptr };
		inline static std::string defaultMessage{ "" };
		inline static std::string defaultNotEnoughGoldMessage{ "" };
		inline static std::string defaultReplacement{ "" };
		inline static std::string defaultVendorInformation{ "" };

		//The following are game functions. I call them from the GetGoldFromPurchase function.
		static void         GoldRemovedMessage(RE::TESForm* a_formToRemove, int a_ammount,
			bool arg3, bool arg4, const char* arg5);
		static uint32_t* MoveGoldBetweenContainers(RE::InventoryChanges* a_inventoryChanges, uint32_t* param_2,
			RE::Actor* a_actor, RE::TESForm* a_form, uint64_t a_concatResult, int arg6, long long** arg7,
			RE::ItemList* arg8, long long** arg9, long long** arg10);

	};
}