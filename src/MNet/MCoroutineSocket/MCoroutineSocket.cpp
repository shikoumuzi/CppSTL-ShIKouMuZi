#include"MCoroutineSocket.h"

namespace MUZI::net::coroutine
{
	class MCoroutineSocketData
	{
	public:
		MCoroutineSocketData()
		{

		}
		MCoroutineSocketData(const IOContext& io_context)
		{

		}
	public:
		IOContext* m_io_context;
		std::map<std::string, NetAsyncIOAdapt> sessions;  // �Ựmap
		MSyncAnnularQueue<NetAsyncIOAdapt> session_notified_queue;  // ֪ͨ����
		NotifiedFunction notified_fun;  // ֪ͨ����
	};

	MCoroutineSocket::MCoroutineSocket():
		m_data(new MCoroutineSocketData())
	{
	}
	MCoroutineSocket::MCoroutineSocket(const IOContext& io_context):
		m_data(new MCoroutineSocketData(io_context))
	{
	}
	MCoroutineSocket::Await<void> MCoroutineSocket::listener()
	{
		auto executor = co_await boost::asio::this_coro::executor;
		TCPAcceptor acceptor(executor, {});
		while (1)
		{
			TCPSocket socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
			
		}
		
	}
}


