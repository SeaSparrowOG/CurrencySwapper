#pragma once

namespace Serialization
{
	constexpr std::uint32_t Version = 1;
	constexpr std::uint32_t ID = 'STPR';

	constexpr std::uint32_t RecordType = 'RCTP';

	void SaveCallback(SKSE::SerializationInterface* a_intfc);
	void LoadCallback(SKSE::SerializationInterface* a_intfc);
	void RevertCallback(SKSE::SerializationInterface* a_intfc);
	
	/// <summary>
	/// Debug tool. When encountering unexpected RecordTypes, converts them to a readable string (HDEC, STEN, etc).
	/// </summary>
	/// <param name="a_typeCode">The unexpected record type.</param>
	/// <returns>The unexpected record type as a string.</returns>
	inline std::string DecodeTypeCode(std::uint32_t a_typeCode)
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

	/// <summary>
	/// Helper function. Encodes a string into the interface.
	/// </summary>
	/// <param name="a_intfc">The serialization interface provided by SKSE.</param>
	/// <param name="a_str">The string to serialize.</param>
	/// <returns>True if encoding is successful, false otherwise.</returns>
	inline bool WriteString(SKSE::SerializationInterface* a_intfc,
		const std::string& a_str)
	{
		std::size_t size = a_str.length() + 1;
		return a_intfc->WriteRecordData(size) && a_intfc->WriteRecordData(a_str.data(), static_cast<std::uint32_t>(size));
	}

	/// <summary>
	/// Helper function. Decodes a string from the interface, and stores it in a given variable.
	/// </summary>
	/// <param name="a_intfc">The serialization interface provided by SKSE.</param>
	/// <param name="a_str">The result is stored here.</param>
	/// <returns>True if successful, false otherwise.</returns>
	inline bool ReadString(SKSE::SerializationInterface* a_intfc,
		std::string& a_str)
	{
		std::size_t size = 0;
		if (!a_intfc->ReadRecordData(size)) {
			return false;
		}
		a_str.reserve(size);
		if (!a_intfc->ReadRecordData(a_str.data(), static_cast<std::uint32_t>(size))) {
			return false;
		}
		return true;
	}
}