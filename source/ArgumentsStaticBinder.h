#pragma once

#include "PostgresTraits.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	/// ArgumentsStaticBinder
	//-------------------------------------------------------------------------------------------------
	template <class... Arguments>
	class ArgumentsStaticBinder
	{
	public:
		typedef std::tuple<typename Traits<Arguments>::Type...> ArgumentsTuple;

	public:
		static constexpr size_t		ArgumentsCount = sizeof...(Arguments);
		static constexpr int		ArgumentsFormats[ArgumentsCount] = { Traits<Arguments>::Format... };

		ArgumentsTuple				m_argumentsTuple;
		void*						m_argumentsPointers[ArgumentsCount];
		int							m_argumentsLengths[ArgumentsCount];

	public:
		//---------------------------------------------------------------------------------------------
		template <size_t Index, class FirstArgument>
		forceinline void BindArguments(const FirstArgument& firstArgument)
		{
			m_argumentsPointers[Index] = &std::get<Index>(m_argumentsTuple);

			BindStaticArgument<Index, FirstArgument>::Bind(*this, firstArgument);
		}

		//---------------------------------------------------------------------------------------------
		template <size_t Index, class FirstArgument, class... RestArgument>
		forceinline void BindArguments(const FirstArgument& firstArgument, const RestArgument&... restArguments)
		{
			m_argumentsPointers[Index] = &std::get<Index>(m_argumentsTuple);

			BindStaticArgument<Index, FirstArgument>::Bind(*this, firstArgument);
			BindArguments<Index + 1u>(restArguments...);
		}

		//---------------------------------------------------------------------------------------------
		/*
		template <class Argument>
		forceinline void BindArgument(size_t argumentIndex, const Argument& argumentValue)
		{
			BindArgumentsCategory<Index, Argument,
				std::is_enum<Argument>::value ? TypeCategory::TypeEnum :
				TypeCategory::TypeUnknown
				>::Bind(*this, argumentIndex, argumentValue);
		}
*/
	private:
		//---------------------------------------------------------------------------------------------
		template <size_t Index, class Argument, TypeCategory TypeCategoryValue>
		class BindArgumentsCategory
		{
		public:
			forceinline static void Bind(ArgumentsStaticBinder& binder, const Argument& argumentValue) = delete;
		};

		//---------------------------------------------------------------------------------------------
		template <size_t Index, class Argument>
		class BindArgumentsCategory<Index, Argument, TypeCategory::TypeEnum>
		{
		public:
			forceinline static void Bind(ArgumentsStaticBinder& binder, const Argument& argumentValue)
			{
				BindStaticArgument<Index, Argument>::Bind(static_cast<typename std::underlying_type<Argument>::type>(binder, argumentValue));
			}
		};

		//---------------------------------------------------------------------------------------------
		template <size_t Index, class Argument>
		class BindStaticArgument
		{
		public:
			forceinline static void Bind(ArgumentsStaticBinder& binder, const Argument& argumentValue)
			{
				BindArgumentsCategory<Index, Argument,
					std::is_enum<Argument>::value ? TypeCategory::TypeEnum :
					TypeCategory::TypeUnknown
					>::Bind(binder, argumentValue);
			}
		};

		//---------------------------------------------------------------------------------------------
		template <size_t Index>
		class BindStaticArgument<Index, int16_t>
		{
		public:
			forceinline static void Bind(ArgumentsStaticBinder& binder, const int16_t& argumentValue)
			{
			}
		};
	};
/*
	//-------------------------------------------------------------------------------------------------
	template <class... Arguments>
	template <>
	forceinline void ArgumentsStaticBinder::BindArgument<int16_t>(size_t argumentIndex, const int16_t& argumentValue)
	{
		int16_t* packedValue = static_cast<int16_t*>(m_argumentsPointers[argumentIndex]);
		*packedValue = argumentValue;
		EndianSwap(*packedValue);

		m_argumentsLengths[argumentIndex] = sizeof(int16_t);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void ArgumentsStaticBinder::BindArgument<int32_t>(size_t argumentIndex, const int32_t& argumentValue)
	{
		return true;
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void ArgumentsStaticBinder::BindArgument<int64_t>(size_t argumentIndex, const int64_t& argumentValue)
	{
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void ArgumentsStaticBinder::BindArgument<uint16_t>(size_t argumentIndex, const uint16_t& argumentValue)
	{
		int16_t* packedValue = static_cast<int16_t*>(m_argumentsPointers[argumentIndex]);
		*packedValue = static_cast<int16_t>(argumentValue);
		EndianSwap(*packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void ArgumentsStaticBinder::BindArgument<uint32_t>(size_t argumentIndex, const uint32_t& argumentValue)
	{
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void ArgumentsStaticBinder::BindArgument<uint64_t>(size_t argumentIndex, const uint64_t& argumentValue)
	{
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void ArgumentsStaticBinder::BindArgument<const char*>(const char* const& argumentValue)
	{
	}
*/
}
