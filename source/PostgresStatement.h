#pragma once

#include "ArgumentsDynamicBinder.h"
#include "ArgumentsStaticBinder.h"
#include "ArgumentsVerifier.h"


namespace Postgres
{
	class IncrementalBinder
	{
	};

	//-------------------------------------------------------------------------------------------------
	/// PostgresStatement
	//-------------------------------------------------------------------------------------------------
	class PostgresStatement : public boost::noncopyable
	{
	private:
		friend class PostgresConnection;

	private:
		const char*					m_name;
		PGconn*						m_connection;
		PGresult*					m_result;

	public:
		//---------------------------------------------------------------------------------------------
		inline PostgresStatement() :
			m_name(nullptr),
			m_connection(nullptr),
			m_result(nullptr)
		{
		}

		//---------------------------------------------------------------------------------------------
		inline PostgresStatement(PostgresStatement&& PostgresStatement) :
			m_name(PostgresStatement.m_name),
			m_connection(PostgresStatement.m_connection),
			m_result(PostgresStatement.m_result)
		{
		}

		//---------------------------------------------------------------------------------------------
		inline ~PostgresStatement()
		{
			ClearResults();
		}

		//---------------------------------------------------------------------------------------------
		inline bool Execute()
		{
			assert(m_name && m_connection);

			ClearResults();

			// Execute statement and check the result
			m_result = PQexecPrepared(
				m_connection,
				m_name,
				0,
				nullptr,
				nullptr,
				nullptr,
				1);

			const ExecStatusType resultStatus = PQresultStatus(m_result);

			return resultStatus == PGRES_TUPLES_OK || resultStatus == PGRES_COMMAND_OK;
		}

		//---------------------------------------------------------------------------------------------
		/*
		template <class... Arguments>
		inline bool Execute(const Arguments&... arguments)
		{
			assert(m_name && m_connection);

			ClearResults();

			// Bind the arguments
			ArgumentsStaticBinder<Arguments...> staticBinder;

			staticBinder.BindArguments<0u>(arguments...);

			// Execute statement and check the result
			m_result = PQexecPrepared(
				m_connection,
				m_name,
				staticBinder.ArgumentsCount,
				static_cast<char**>(staticBinder.m_argumentsPointers),
				staticBinder.m_argumentsLengths,
				staticBinder.ArgumentsFormats,
				1);

			const ExecStatusType resultStatus = PQresultStatus(m_result);

			return resultStatus == PGRES_TUPLES_OK || resultStatus == PGRES_COMMAND_OK;
		}
*/
		//---------------------------------------------------------------------------------------------
		template <class Argument, class = typename std::enable_if<std::is_base_of<IncrementalBinder, Argument>::value >::type >
		inline bool Execute(const Argument& incrementalBinder)
		{
			assert(m_name && m_connection);

			ClearResults();

			// Verify arguments binding
#ifdef _DEBUG
			m_result = PQdescribePrepared(m_connection, m_name);

			if (PQresultStatus(m_result) != PGRES_COMMAND_OK)
			{
				assert(false);
				return false;
			}

			ArgumentsVerifier argumentsVerifier(m_result);

			try
			{
				incrementalBinder.BindArguments(argumentsVerifier);
			}
			catch (std::runtime_error exception)
			{
				assert(false);
				return false;
			}

			assert(PQnparams(m_result) == argumentsVerifier.m_argumentIndex);

			ClearResults();
#endif

			// Bind the arguments
			ArgumentsDynamicBinder dynamicBinder;
			incrementalBinder.BindArguments(dynamicBinder);

			const size_t argumentsCount = dynamicBinder.m_argumentsVector.size();

			void** argumentsPointers = reinterpret_cast<void**>(alloca(sizeof(void*) * argumentsCount));
			int* argumentsLengths = reinterpret_cast<int*>(alloca(sizeof(int) * argumentsCount));
			int* argumentsFormats = reinterpret_cast<int*>(alloca(sizeof(int) * argumentsCount));

			for (size_t argumentIndex = 0u; argumentIndex != argumentsCount; ++argumentIndex)
			{
				if (dynamicBinder.m_argumentsVector[argumentIndex].m_hasExternalData)
				{
					argumentsPointers[argumentIndex] = reinterpret_cast<void*>(dynamicBinder.m_argumentsVector[argumentIndex].m_argumentData);
				}
				else
				{
					argumentsPointers[argumentIndex] = &dynamicBinder.m_argumentsVector[argumentIndex].m_argumentData;
				}

				argumentsLengths[argumentIndex] = dynamicBinder.m_argumentsVector[argumentIndex].m_argumentLength;
				argumentsFormats[argumentIndex] = 1;
			}

			// Execute statement and check the result
			m_result = PQexecPrepared(
				m_connection,
				m_name,
				static_cast<int>(argumentsCount),
				reinterpret_cast<char**>(argumentsPointers),
				argumentsLengths,
				argumentsFormats,
				1);
			//  || resultStatus == PGRES_COPY_OUT
			const ExecStatusType resultStatus = PQresultStatus(m_result);

			return resultStatus == PGRES_TUPLES_OK || resultStatus == PGRES_COMMAND_OK;
		}

		//---------------------------------------------------------------------------------------------
		inline bool HasResults() const
		{
			assert(m_result);
			return PQntuples(m_result) != 0;
		}

		//---------------------------------------------------------------------------------------------
		inline uint32_t GetResultTuplesCount() const
		{
			assert(m_result);
			return static_cast<uint32_t>(PQntuples(m_result));
		}

		//---------------------------------------------------------------------------------------------
		template <class... Results>
		inline void GetResultTuple(uint32_t tupleIndex, Results&... results)
		{
			assert(m_result);
			assert(PQnfields(m_result) == sizeof...(Results));

			ExtractResults<0>(static_cast<int32_t>(tupleIndex), results...);
		}

		//---------------------------------------------------------------------------------------------
		inline void operator =(PostgresStatement&& PostgresStatement)
		{
			m_name = PostgresStatement.m_name;
			m_connection = PostgresStatement.m_connection;
			m_result = PostgresStatement.m_result;

			PostgresStatement.m_result = nullptr;
		}

		//---------------------------------------------------------------------------------------------
		inline operator bool() const
		{
			return m_name != nullptr;
		}

	private:
		//---------------------------------------------------------------------------------------------
		forceinline PostgresStatement(const char* name, PGconn* connection) : m_name(name), m_connection(connection), m_result(nullptr)
		{
		}

		//---------------------------------------------------------------------------------------------
		forceinline void ClearResults()
		{
			if (m_result)
			{
				PQclear(m_result);
				m_result = nullptr;
			}
		}

		//---------------------------------------------------------------------------------------------
		template <int32_t Index, class FirstResult>
		forceinline void ExtractResults(int32_t tupleIndex, FirstResult& firstResult)
		{
			ExtractResult(tupleIndex, Index, firstResult);
		}

		//---------------------------------------------------------------------------------------------
		template <int32_t Index, class FirstResult, class... RestResult>
		forceinline void ExtractResults(int32_t tupleIndex, FirstResult& firstResult, RestResult&... restResults)
		{
			ExtractResult(tupleIndex, Index, firstResult);
			ExtractResults<Index + 1>(tupleIndex, restResults...);
		}

		//---------------------------------------------------------------------------------------------
		template <class Result>
		forceinline void ExtractResult(int32_t tupleIndex, int32_t resultIndex, Result& resultValue)
		{
			ExtractResultsCategory<Result,
				std::is_enum<Result>::value ? TypeCategory::TypeEnum :
				TypeCategory::TypeUnknown
				>::Extract(*this, tupleIndex, resultIndex, resultValue);
		}

		//---------------------------------------------------------------------------------------------
		template <class Result, TypeCategory TypeCategoryValue>
		class ExtractResultsCategory
		{
		public:
			forceinline static void Extract(PostgresStatement& PostgresStatement, int32_t tupleIndex, int32_t resultIndex, Result& resultValue) = delete;
		};

		//---------------------------------------------------------------------------------------------
		template <class Result>
		class ExtractResultsCategory<Result, TypeCategory::TypeEnum>
		{
		public:
			forceinline static void Extract(PostgresStatement& PostgresStatement, int32_t tupleIndex, int32_t resultIndex, Result& resultValue)
			{
				PostgresStatement.ExtractResult(tupleIndex, resultIndex, reinterpret_cast<typename std::underlying_type<Result>::type&>(resultValue));
			}
		};
	};

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<int16_t>(int32_t tupleIndex, int32_t resultIndex, int16_t& resultValue)
	{
		assert(PQgetlength(m_result, tupleIndex, resultIndex) == sizeof(int16_t));
		assert(PQftype(m_result, resultIndex) == INT2OID);

		resultValue = *reinterpret_cast<int16_t*>(PQgetvalue(m_result, tupleIndex, resultIndex));

		EndianSwap(resultValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<int32_t>(int32_t tupleIndex, int32_t resultIndex, int32_t& resultValue)
	{
		assert(PQgetlength(m_result, tupleIndex, resultIndex) == sizeof(int32_t));
		assert(PQftype(m_result, resultIndex) == INT4OID);

		resultValue = *reinterpret_cast<int32_t*>(PQgetvalue(m_result, tupleIndex, resultIndex));

		EndianSwap(resultValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<int64_t>(int32_t tupleIndex, int32_t resultIndex, int64_t& resultValue)
	{
		assert(PQgetlength(m_result, tupleIndex, resultIndex) == sizeof(int64_t));
		assert(PQftype(m_result, resultIndex) == INT8OID);

		resultValue = *reinterpret_cast<int64_t*>(PQgetvalue(m_result, tupleIndex, resultIndex));

		EndianSwap(resultValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<uint16_t>(int32_t tupleIndex, int32_t resultIndex, uint16_t& resultValue)
	{
		assert(PQgetlength(m_result, tupleIndex, resultIndex) == sizeof(int16_t));
		assert(PQftype(m_result, resultIndex) == INT2OID);

		int16_t packedValue = *reinterpret_cast<int16_t*>(PQgetvalue(m_result, tupleIndex, resultIndex));
		EndianSwap(packedValue);

		resultValue = static_cast<uint16_t>(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<uint32_t>(int32_t tupleIndex, int32_t resultIndex, uint32_t& resultValue)
	{
		assert(PQgetlength(m_result, tupleIndex, resultIndex) == sizeof(int32_t));
		assert(PQftype(m_result, resultIndex) == INT4OID);

		int32_t packedValue = *reinterpret_cast<int32_t*>(PQgetvalue(m_result, tupleIndex, resultIndex));
		EndianSwap(packedValue);

		resultValue = static_cast<uint32_t>(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<uint64_t>(int32_t tupleIndex, int32_t resultIndex, uint64_t& resultValue)
	{
		assert(PQgetlength(m_result, tupleIndex, resultIndex) == sizeof(int64_t));
		assert(PQftype(m_result, resultIndex) == INT8OID);

		int64_t packedValue = *reinterpret_cast<int64_t*>(PQgetvalue(m_result, tupleIndex, resultIndex));
		EndianSwap(packedValue);

		resultValue = static_cast<uint64_t>(packedValue);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<bool>(int32_t tupleIndex, int32_t resultIndex, bool& resultValue)
	{
		assert(false);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<float>(int32_t tupleIndex, int32_t resultIndex, float& resultValue)
	{
		assert(false);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<double>(int32_t tupleIndex, int32_t resultIndex, double& resultValue)
	{
		assert(false);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<std::string>(int32_t tupleIndex, int32_t resultIndex, std::string& resultValue)
	{
		assert(PQftype(m_result, resultIndex) == TEXTOID || PQftype(m_result, resultIndex) > 10000);

		const size_t packedLength = PQgetlength(m_result, tupleIndex, resultIndex);
		const auto* packedValue = PQgetvalue(m_result, tupleIndex, resultIndex);

		resultValue.assign(packedValue, packedValue + packedLength);
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	forceinline void PostgresStatement::ExtractResult<std::vector<byte>>(int32_t tupleIndex, int32_t resultIndex, std::vector<byte>& resultValue)
	{
		assert(PQftype(m_result, resultIndex) == BYTEAOID);

		const size_t packedLength = PQgetlength(m_result, tupleIndex, resultIndex);
		const byte* packedValue = reinterpret_cast<byte*>(PQgetvalue(m_result, tupleIndex, resultIndex));

		resultValue.assign(packedValue, packedValue + packedLength);
	}
}
