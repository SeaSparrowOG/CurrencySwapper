#include "serde.h"

#include "hooks.h"

namespace Serialization {
	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		if (!a_intfc->OpenRecord(StoredCurrency, Version)) {
			_loggerError("Failed to open record for StoredCurrency");
			return;
		}

		auto* currency = Hooks::BarterHooks::GetSingleton()->GetCurrency();
		if (currency) {
			if (!a_intfc->WriteRecordData(currency->formID)) {
				_loggerError("Failed to write FormID ({:08X})", currency->formID);
				return;
			}
		}
		else {
			RE::FormID nullID = 0x0;
			if (!a_intfc->WriteRecordData(nullID)) {
				_loggerError("Failed to write FormID (no currecncy set) ({:08X})", nullID);
				return;
			}
		}
	}

	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != Version) {
				_loggerError("Loaded data is incompatible with plugin version!");
			}

			if (type == StoredCurrency) {
				Hooks::BarterHooks::GetSingleton()->RevertCurrency();

				RE::FormID currencyOldID;
				if (!a_intfc->ReadRecordData(currencyOldID)) {
					_loggerError("Failed to read FormID");
					return;
				}

				if (currencyOldID == 0x0) {
					return;
				}

				RE::FormID currencyID;
				if (!a_intfc->ResolveFormID(currencyOldID, currencyID)) {
					_loggerError("Failed to resolve FormID ({:08X})", currencyOldID);
					return;
				}

				auto* currencyForm = RE::TESForm::LookupByID(currencyID);
				if (!currencyForm) {
					_loggerError("Failed to find appropriate form ({:08X})", currencyID);
					return;
				}

				Hooks::BarterHooks::GetSingleton()->SetCurrency(currencyForm);
			}
		}
	}

	void RevertCallback(SKSE::SerializationInterface* a_intfc)
	{
		Hooks::BarterHooks::GetSingleton()->RevertCurrency();
	}

	std::string DecodeTypeCode(std::uint32_t a_typeCode)
	{
		constexpr std::size_t SIZE = sizeof(std::uint32_t);

		std::string sig;
		sig.resize(SIZE);
		char* iter = reinterpret_cast<char*>(&a_typeCode);
		for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
			sig[j] = iter[i];
		}
		return sig;
	}
}