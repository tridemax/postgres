#pragma once

#include "PostgresTraits.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	/// ArgumentsDynamicBinder
	//-------------------------------------------------------------------------------------------------
	class ArgumentsDynamicBinder
	{
	public:
#pragma pack(push, 4)
		class PostgresArgument
		{
		public:
			uintptr_t					m_argumentData;
			uint32_t					m_argumentLength;
			bool						m_hasExternalData;

		public:
			template <class Argument, class = typename std::enable_if<sizeof(Argument) <= sizeof(uintptr_t)>::type>
			__forceinline PostgresArgument(const Argument& argumentValue)
			{
				memcpy(&m_argumentData, &argumentValue, sizeof(Argument));
				m_argumentLength = sizeof(Argument);
				m_hasExternalData = false;
			}

			__forceinline PostgresArgument(const char* const& argumentValue)
			{
				m_argumentData = reinterpret_cast<uintptr_t>(argumentValue);
				m_argumentLength = static_cast<uint32_t>(strlen(argumentValue));
				m_hasExternalData = true;
			}
		};
#pragma pack(pop)

		typedef std::vector<PostgresArgument> ArgumentVector;

	public:
		ArgumentVector				m_argumentsVector;

	public:
		//---------------------------------------------------------------------------------------------
		__forceinline void ReserveArguments(uint32_t argumentsCount)
		{
			m_argumentsVector.reserve(argumentsCount);
		}

		//---------------------------------------------------------------------------------------------
		template <class Argument>
		__forceinline void BindArgument(const Argument& argumentValue)
		{
			BindArgumentsCategory<Argument,
				std::is_enum<Argument>::value ? TypeCategory::TypeEnum :
				TypeCategory::TypeUnknown
				>::Bind(*this, argumentValue);
		}

	private:
		//---------------------------------------------------------------------------------------------
		template <class Argument, TypeCategory TypeCategoryValue>
		class BindArgumentsCategory
		{
		public:
			__forceinline static void Bind(ArgumentsDynamicBinder& binder, const Argument& argumentValue) = delete;
		};

		//---------------------------------------------------------------------------------------------
		template <class Argument>
		class BindArgumentsCategory<Argument, TypeCategory::TypeEnum>
		{
		public:
			__forceinline static void Bind(ArgumentsDynamicBinder& binder, const Argument& argumentValue)
			{
				binder.BindArgument(static_cast<typename std::underlying_type<Argument>::type>(argumentValue));
			}
		};
	};

	//-------------------------------------------------------------------------------------------------
	template <>
	__forceinline void ArgumentsDynamicBinder::BindArgument<int16_t>(const int16_t& argumentValue)
	{
		int16_t packedValue = argumentValue;
		EndianSwap(packedValue);

		m_argumentsVector.emplace_back(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	__forceinline void ArgumentsDynamicBinder::BindArgument<int32_t>(const int32_t& argumentValue)
	{
		int32_t packedValue = argumentValue;
		EndianSwap(packedValue);

		m_argumentsVector.emplace_back(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	__forceinline void ArgumentsDynamicBinder::BindArgument<int64_t>(const int64_t& argumentValue)
	{
		int64_t packedValue = argumentValue;
		EndianSwap(packedValue);

		m_argumentsVector.emplace_back(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	__forceinline void ArgumentsDynamicBinder::BindArgument<uint16_t>(const uint16_t& argumentValue)
	{
		int16_t packedValue = static_cast<int16_t>(argumentValue);
		EndianSwap(packedValue);

		m_argumentsVector.emplace_back(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	__forceinline void ArgumentsDynamicBinder::BindArgument<uint32_t>(const uint32_t& argumentValue)
	{
		int32_t packedValue = static_cast<int32_t>(argumentValue);
		EndianSwap(packedValue);

		m_argumentsVector.emplace_back(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	__forceinline void ArgumentsDynamicBinder::BindArgument<uint64_t>(const uint64_t& argumentValue)
	{
		int64_t packedValue = static_cast<int64_t>(argumentValue);
		EndianSwap(packedValue);

		m_argumentsVector.emplace_back(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	__forceinline void ArgumentsDynamicBinder::BindArgument<const char*>(const char* const& argumentValue)
	{
		m_argumentsVector.emplace_back(argumentValue);
	}
}
