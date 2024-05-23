#ifndef THREADS_COUNT
#    error please, define SERVER_THREADS_COUNT variable
#endif
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/system/error_code.hpp>
#include <listener.hxx>
#include <def.hxx>

using namespace msserver;

int main()
{
    net::io_context ioc( THREADS_COUNT );
    auto ls = std::make_shared<listener>( ioc, tcp::endpoint( boost::asio::ip::make_address( HTTP_TARGET_HOST ), HTTP_TARGET_PORT ) );
    ls->get( "/hw", []( http::request<http::string_body>, http::response<http::string_body> &response )
             { response.body() = "Hello world!"; response.set(http::field::content_type, "text/plain"); } );

    ls->websocket( "/", []( const std::string &str, std::string &req )
                   { req = str; } );

    ls->run();

    ioc.run();
}
