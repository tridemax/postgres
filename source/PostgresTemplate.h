#pragma once


namespace Postgres
{
	//-------------------------------------------------------------------------------------------------
	/// PostgresTemplate
	//-------------------------------------------------------------------------------------------------
	class PostgresTemplate : public boost::noncopyable
	{
	private:
		const char*					m_templateCode;
		const uint64_t				m_templateHash;

	public:
		//---------------------------------------------------------------------------------------------
		forceinline PostgresTemplate(const char* templateCode) : m_templateCode(templateCode), m_templateHash(Aux::Hash64(templateCode, strlen(templateCode)))
		{
		}

		//---------------------------------------------------------------------------------------------
		forceinline const char* GetCode() const
		{
			return m_templateCode;
		}

		//---------------------------------------------------------------------------------------------
		forceinline uint64_t GetHash() const
		{
			return m_templateHash;
		}
	};
}
