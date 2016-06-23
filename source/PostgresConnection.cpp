#include "stdafx.h"
#include "PostgresConnection.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	PostgresConnection::PostgresConnection() : m_internalConnection(nullptr)
	{
	}

	//-------------------------------------------------------------------------------------------------
	PostgresConnection::~PostgresConnection()
	{
		CloseConnection();
	}

	//-------------------------------------------------------------------------------------------------
	bool PostgresConnection::OpenConnection(const char* dbConnectionParams)
	{
		// Close currently opened connection
		CloseConnection();

		// Try to open connection
		m_internalConnection = PQconnectdb(dbConnectionParams);

		return PQstatus(m_internalConnection) != CONNECTION_OK;
	}

	//-------------------------------------------------------------------------------------------------
	void PostgresConnection::CloseConnection()
	{
		if (m_internalConnection)
		{
			PQfinish(m_internalConnection);
			m_internalConnection = nullptr;
		}

		m_statementCache.clear();
	}
}
