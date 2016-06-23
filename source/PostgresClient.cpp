#include "platform.h"
#include "PostgresClient.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	PostgresClient::PostgresClient()
	{
	}

	//-------------------------------------------------------------------------------------------------
	PostgresClient::~PostgresClient()
	{
		for (auto& connection : m_connectionCache)
		{
			connection.CloseConnection();
		}
	}

	//-------------------------------------------------------------------------------------------------
	PostgresConnection& PostgresClient::GetConnection(const char* dbConnectionParams)
	{
		auto& connection = m_connectionCache.local();

		if (!connection.IsOpen())
		{
			connection.OpenConnection(dbConnectionParams);
			assert(connection.IsOpen());
		}

		return connection;
	}
}
