#pragma once

#include "PostgresTemplate.h"
#include "PostgresStatement.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	/// PostgresConnection
	//-------------------------------------------------------------------------------------------------
	class PostgresConnection : public NonCopyable
	{
	private:
#pragma pack(push, 1)
		class StatementName
		{
		public:
			char						m_name[18]; // '_' + strhex(sizeof(uint64_t)) + '\0'

		public:
			inline StatementName() = default;

			inline StatementName(uint64_t statementHash)
			{
				std::ostringstream stream;
				stream << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(16u) << statementHash;

				assert(stream.str().size() == sizeof(StatementName) - 1u);

				memcpy(m_name, stream.str().c_str(), sizeof(StatementName));
			}

			inline operator const char* () const
			{
				return m_name;
			}
		};
#pragma pack(pop)

		typedef std::map<uint64_t, StatementName> StatementCache;

	private:
		PGconn*						m_internalConnection;
		StatementCache				m_statementCache;

	public:
		PostgresConnection();
		~PostgresConnection();

		bool OpenConnection(const char* dbConnectionParams);
		void CloseConnection();

	public:
		//---------------------------------------------------------------------------------------------
		inline bool IsOpen() const
		{
			return m_internalConnection && PQstatus(m_internalConnection) == CONNECTION_OK;
		}

		//---------------------------------------------------------------------------------------------
		inline const char* LastError() const
		{
			return m_internalConnection ? PQerrorMessage(m_internalConnection) : "";
		}

		//---------------------------------------------------------------------------------------------
		template <class TemplateType>
		inline bool ExecuteTemplate(const TemplateType& PostgresTemplate)
		{
			// Find or compile statement
			auto PostgresStatement = StatementFromTemplate(PostgresTemplate);

			if (!PostgresStatement)
			{
				assert(false);
				return false;
			}

			// Execute statement
			return PostgresStatement.Execute();
		}

		//---------------------------------------------------------------------------------------------
		template <class TemplateType, class... Arguments>
		inline bool ExecuteTemplate(const TemplateType& PostgresTemplate, const Arguments&... arguments)
		{
			// Find or compile statement
			auto PostgresStatement = StatementFromTemplate(PostgresTemplate);

			if (!PostgresStatement)
			{
				assert(false);
				return false;
			}

			// Execute statement
			return PostgresStatement.Execute(arguments...);
		}

		//---------------------------------------------------------------------------------------------
		template <class TemplateType>
		inline PostgresStatement StatementFromTemplate(const TemplateType& PostgresTemplate)
		{
			assert(IsOpen());

			// Find prepared statement
			auto itStatement = m_statementCache.find(PostgresTemplate.GetHash());

			// Compile statement and save into cache
			if (itStatement == m_statementCache.end())
			{
				const StatementName statementName = PostgresTemplate.GetHash();

				auto* compilationResult = PQprepare(m_internalConnection, statementName, PostgresTemplate.GetCode(), 0, nullptr);

				if (PQresultStatus(compilationResult) != PGRES_COMMAND_OK)
				{
					assert(false);
					PQclear(compilationResult);
					return PostgresStatement(nullptr, nullptr);
				}

				PQclear(compilationResult);

				itStatement = m_statementCache.insert(std::make_pair(PostgresTemplate.GetHash(), statementName)).first;
			}

			return PostgresStatement(itStatement->second, m_internalConnection);
		}
	};
}
