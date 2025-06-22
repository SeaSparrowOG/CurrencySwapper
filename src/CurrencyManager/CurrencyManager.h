#pragma once

namespace CurrencyManager
{
	bool Initialize();
	bool Save(SKSE::SerializationInterface* a_intfc);
	bool Load(SKSE::SerializationInterface* a_intfc);
	void Revert(SKSE::SerializationInterface* a_intfc);

	bool GetPlayerGold(RE::Actor* a_player, int32_t& out);
	bool GetVendorGold(RE::InventoryChanges* a_player, int32_t& out);
	bool GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
		RE::Actor* a_buyer,
		int32_t a_value,
		RE::ItemList* a_itemList);

	bool ProcessRawDeal(uint64_t a_itemValue,
		uint64_t a_merchantGold,
		std::string& a_out);
	bool ProcessTrainingDeal(RE::Actor* a_player, int32_t a_value);
	bool ProcessRejectedDeal();

	void ResetVendorInfo(RE::GFxValue* a_updateObj);

	bool SendCustomSaleEvent(RE::TESForm*& a_out);
	void SendRejectedDealEvent();
	void SendBarterMenuEvent(RE::TESObjectREFR* a_ref);
	void SendCustomTrainingMenuEvent(RE::TrainingMenu* a_menu);

	RE::TESForm* GetCurrency();
	void RevertCurrency();
	void RegisterFormForEvents(RE::TESForm* a_form);
	void SetTrainingOverrides(bool a_overrideMult, float a_multiplier, bool a_overrideBase, float a_base);
	bool SetCurrency(RE::TESForm* a_currency);

	bool RequestTrainingCost(float a_SkillLevel, float& a_out);

	void ReloadIniSettings();

	class CurrencyManager : 
		public REX::Singleton<CurrencyManager>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		bool Initialize();

		bool Save(SKSE::SerializationInterface* a_intfc);
		bool Load(SKSE::SerializationInterface* a_intfc);
		void Revert(SKSE::SerializationInterface* a_intfc);

		void         RevertCurrency();
		bool         SetCurrency(RE::TESForm* a_newCurrency);
		void         SetTrainingOverrides(bool a_overrideMult, float a_multiplier, bool a_overrideBase, float a_base);
		void         RegisterFormForAllEvents(RE::TESForm* a_form);
		void         UnRegisterFormForAllEvents(RE::TESForm* a_form);
		RE::TESForm* GetCurrency();

		// Function replacers
		void SendCustomTrainingMenuEvent(RE::TrainingMenu* a_this);
		bool GetPlayerGold(RE::Actor* a_player, int32_t& a_out);
		bool GetVendorGold(RE::InventoryChanges* a_vendorInventoryChanges, int32_t& a_out);
		bool GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
			RE::Actor* a_buyer,
			int a_value,
			RE::ItemList* param_4);
		void SendSaleEvent();
		void SendCustomMenuEvent(RE::TESObjectREFR* a_ref);
		void SendRejectedDealEvent();
		
		bool SendRawDealWarning(uint64_t a_value, uint64_t a_merchantGold, std::string& a_out);
		bool SendNotEnoughGoldWarning();
		void ResetVendorInfo(RE::GFxValue* a_updateObj);

		bool ProcessTrainingDeal(RE::Actor* a_player, int32_t a_value);

		bool RequestTrainingCost(float a_SkillLevel, float& a_out);

		void ReloadINISettings();

	private:
		RE::BSEventNotifyControl ProcessEvent(
			const RE::MenuOpenCloseEvent* a_event,
			RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;

		void UpdateSkyUIText(RE::GFxValue* a_updateObj);
		void UpdateVanillaText(RE::GFxValue* a_updateObj);

		static inline SKSE::RegistrationSet<RE::TESForm*> customRevert{ "OnCurrencyRevert"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*> customPurchase{ "OnCustomPurchase"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*> customSale{ "OnCustomSale"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*> customPurchaseFail{ "OnCustomCurrencyFail"sv };
		static inline SKSE::RegistrationSet<RE::Actor*>   customBarterMenu{ "OnCustomBarterMenu"sv };
		static inline SKSE::RegistrationSet<RE::TESForm*, RE::TESForm*> currencySwap{ "OnCurrencySwap"sv };

		RE::TESBoundObject* customCurrency{ nullptr };

		RE::FormID barterActorID{ 0 };
		RE::FormID trainerActorID{ 0 };

		float trainingCostMultiplierOverride{ 0.0f };
		bool overrideTrainingCostMultiplier{ false };
		float trainingCostBaseOverride{ 0.0f };
		bool overrideTrainingCostBase{ false };

		float trainingLabelOffsety{ 0.0f };

		inline static constexpr std::uint32_t Version = 2;
		inline static constexpr std::uint32_t ID = 'AMSF';
		inline static constexpr std::uint32_t StoredCurrency = 'STCU';
	};

	inline static constexpr const char* pathToPlayerLabel = "_root.Menu_mc.BottomBar_mc.PlayerInfoCard_mc.PlayerGoldLabel";
	inline static constexpr const char* pathToVendorLabel = "_root.Menu_mc.BottomBar_mc.PlayerInfoCard_mc.VendorGoldLabel";
	inline static constexpr const char* pathToPlayerLabel_SkyUI = "_root.Menu_mc.bottomBar.playerInfoCard.PlayerGoldLabel";
	inline static constexpr const char* pathToVendorLabel_SkyUI = "_root.Menu_mc.bottomBar.playerInfoCard.VendorGoldLabel";
}