#pragma once

#include "PostgresTraits.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	/// ArgumentsVerifier
	//-------------------------------------------------------------------------------------------------
	class ArgumentsVerifier
	{
	public:
		PGresult*					m_result;
		int32_t						m_argumentIndex;

	public:
		//---------------------------------------------------------------------------------------------
		inline ArgumentsVerifier(PGresult* result) : m_result(result), m_argumentIndex(0)
		{
		}

		//---------------------------------------------------------------------------------------------
		template <class Argument>
		inline void BindArgument(const Argument& argumentValue)
		{
			return BindArgumentsCategory<Argument,
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
			inline static void Bind(ArgumentsVerifier& binder, const Argument& argumentValue) = delete;
		};

		//---------------------------------------------------------------------------------------------
		template <class Argument>
		class BindArgumentsCategory<Argument, TypeCategory::TypeEnum>
		{
		public:
			inline static void Bind(ArgumentsVerifier& binder, const Argument& argumentValue)
			{
				binder.BindArgument(static_cast<typename std::underlying_type<Argument>::type>(argumentValue));
			}
		};
	};

	//-------------------------------------------------------------------------------------------------
	template <>
	inline void ArgumentsVerifier::BindArgument<int16_t>(const int16_t& argumentValue)
	{
		const auto argumentOid = PQparamtype(m_result, m_argumentIndex++);

		if (argumentOid != INT2OID)
		{
			throw std::runtime_error("argumentOid != INT2OID");
		}
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	inline void ArgumentsVerifier::BindArgument<int32_t>(const int32_t& argumentValue)
	{
		const auto argumentOid = PQparamtype(m_result, m_argumentIndex++);

		if (argumentOid != INT4OID)
		{
			throw std::runtime_error("argumentOid != INT4OID");
		}
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	inline void ArgumentsVerifier::BindArgument<int64_t>(const int64_t& argumentValue)
	{
		const auto argumentOid = PQparamtype(m_result, m_argumentIndex++);

		if (argumentOid != INT8OID)
		{
			throw std::runtime_error("argumentOid != INT8OID");
		}
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	inline void ArgumentsVerifier::BindArgument<uint16_t>(const uint16_t& argumentValue)
	{
		const auto argumentOid = PQparamtype(m_result, m_argumentIndex++);

		if (argumentOid != INT2OID)
		{
			throw std::runtime_error("argumentOid != INT2OID");
		}
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	inline void ArgumentsVerifier::BindArgument<uint32_t>(const uint32_t& argumentValue)
	{
		const auto argumentOid = PQparamtype(m_result, m_argumentIndex++);

		if (argumentOid != INT4OID)
		{
			throw std::runtime_error("argumentOid != INT4OID");
		}
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	inline void ArgumentsVerifier::BindArgument<uint64_t>(const uint64_t& argumentValue)
	{
		const auto argumentOid = PQparamtype(m_result, m_argumentIndex++);

		if (argumentOid != INT8OID)
		{
			throw std::runtime_error("argumentOid != INT8OID");
		}
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	inline void ArgumentsVerifier::BindArgument<const char*>(const char* const& argumentValue)
	{
		const auto argumentOid = PQparamtype(m_result, m_argumentIndex++);

		if (argumentOid != TEXTOID && argumentOid <= 10000)
		{
			throw std::runtime_error("argumentOid != TEXTOID");
		}
	}
}
