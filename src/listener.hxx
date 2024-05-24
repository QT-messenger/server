#pragma once
#ifndef LISTENER_HXX
#    define LISTENER_HXX
#    include <memory>
#    include <vector>
#    include <boost/asio/io_context.hpp>
#    include <boost/asio/strand.hpp>
#    include <boost/beast.hpp>
#    include <boost/beast/core/bind_handler.hpp>
#    include <http_target.hxx>
#    include <websocket_target.hxx>
#    include <def.hxx>

namespace msserver
{
    namespace net   = boost::asio;
    namespace beast = boost::beast;
    namespace http  = boost::beast::http;
    using tcp       = net::ip::tcp;

    class listener : public std::enable_shared_from_this<listener>
    {
      private:
        net::io_context &ioc;
        net::ip::tcp::acceptor acceptor;
        std::vector<http_target> http_targets;
        std::vector<websocket_target> websocket_targets;

      public:
        listener( net::io_context &ioc, tcp::endpoint endpoint );

        constexpr inline void get( const std::string &ep, const http_request_handler &handler ) noexcept
        {
            http_targets.emplace_back( http::verb::get, ep, handler );
        }

        constexpr inline void post( const std::string ep, const http_request_handler &handler ) noexcept
        {
            http_targets.emplace_back( http::verb::post, ep, handler );
        }

        constexpr inline void websocket( const std::string ep, const websocket_request_handler &handler ) noexcept
        {
            websocket_targets.emplace_back( ep, handler );
        }

        inline void run() noexcept
        {
            net::dispatch( acceptor.get_executor(), beast::bind_front_handler( &listener::do_accept, shared_from_this() ) );
        }

      private:
        void do_accept() noexcept;
        void on_accept( beast::error_code ec, tcp::socket socket ) noexcept;
        void on_read( tcp::socket &socket, const http::request<http::string_body> &req, beast::error_code ec, size_t bytes_transferred );
    };

} // namespace msserver

#endif