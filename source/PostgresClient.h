#pragma once

#include "PostgresConnection.h"


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	/// PostgresClient
	//-------------------------------------------------------------------------------------------------
	class PostgresClient : public NonCopyable
	{
	private:
		typedef tbb::enumerable_thread_specific<PostgresConnection> ConnectionCache;

	private:
		ConnectionCache				m_connectionCache;

	public:
		PostgresClient();
		~PostgresClient();

		PostgresConnection& GetConnection(const char* dbConnectionParams);

	public:
		//---------------------------------------------------------------------------------------------
		static inline PostgresClient& Instance()
		{
			static PostgresClient instance;
			return instance;
		}
	};
}
