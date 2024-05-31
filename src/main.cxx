#ifndef THREADS_COUNT
#    error please, define SERVER_THREADS_COUNT variable
#endif
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/signal_set.hpp>
#include <listener.hxx>
#include <websocket_session.hxx>
#include <websocket_handlers.hxx>
#include <websocket_error.hxx>
#include <def.hxx>
#include <thread>

using namespace msserver;

int main()
{
    net::io_context ioc( THREADS_COUNT );
    auto ls = std::make_shared<listener>( ioc, tcp::endpoint( boost::asio::ip::make_address( HTTP_TARGET_HOST ), HTTP_TARGET_PORT ) );

    ls->get( "/hw", []( http::request<http::string_body>, http::response<http::string_body> &response )
             { response.body() = "Hello world!"; response.set(http::field::content_type, "text/plain"); } );

    ls->websocket( "/", handle );
    ls->run();

    net::signal_set signals( ioc, SIGINT, SIGTERM );
    signals.async_wait( [ & ]( beast::error_code const &, int )
                        { ioc.stop(); } );

    std::vector<std::thread> v;
    v.reserve( THREADS_COUNT - 1 );
    for ( size_t i = THREADS_COUNT - 1; i > 0; --i )
        v.emplace_back( [ &ioc ]
                        { ioc.run(); } );
    ioc.run();

    for ( auto &t : v )
        t.join();

    ioc.run();
}
