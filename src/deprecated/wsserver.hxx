#ifndef WSERVER_HXX
#define WSERVER_HXX

#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket/stream.hpp>

namespace msserver
{

    namespace beast         = boost::beast;
    namespace net           = boost::asio;
    namespace http          = beast::http;
    namespace websocket     = boost::beast::websocket;
    using tcp               = boost::asio::ip::tcp;
    using webSocketCallback = std::function<void( const std::string &request, std::string &response )>;

    class wsserver
    {
      private:
        net::ip::address addr;
        uint16_t port;
        net::io_context ioc;
        net::ip::tcp::acceptor acceptor;

        bool work;

      public:
        wsserver( const std::string &address, uint16_t port );
        void run( webSocketCallback onRequest );

        constexpr inline void stop() noexcept
        {
            work = false;
        }

        constexpr inline bool isWorking() const noexcept
        {
            return work;
        }
    };
} // namespace msserver

#endif