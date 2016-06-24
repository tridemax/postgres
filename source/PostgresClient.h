#pragma once

#include "PostgresConnection.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	/// PostgresClient
	//-------------------------------------------------------------------------------------------------
	class PostgresClient : public boost::noncopyable
	{
	private:
		typedef boost::thread_specific_ptr<PostgresConnection> ConnectionHolder;

	private:
		ConnectionHolder			m_threadSpecificConnection;

	public:
		//---------------------------------------------------------------------------------------------
		inline PostgresConnection& GetConnection(const char* dbConnectionParams)
		{
			if (!m_threadSpecificConnection.get())
			{
				m_threadSpecificConnection.reset(new PostgresConnection());
			}

			if (!m_threadSpecificConnection->IsOpen())
			{
				m_threadSpecificConnection->OpenConnection(dbConnectionParams);
				assert(m_threadSpecificConnection->IsOpen());
			}

			return *m_threadSpecificConnection.get();
		}

		//---------------------------------------------------------------------------------------------
		static inline PostgresClient& Instance()
		{
			static PostgresClient instance;
			return instance;
		}
	};
}
