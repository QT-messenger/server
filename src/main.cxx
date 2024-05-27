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

// Only for test
uint64_t authentificate_user()
{
    return 1;
}

websocket_session *find_session( std::shared_ptr<shared_state> state, uint64_t id ) noexcept
{
    auto sessions = state->get_sessions();
    auto session  = std::find_if( sessions.begin(), sessions.end(), [ id ]( websocket_session *session )
                                  { return session->get_id() == id; } );

    if ( session == sessions.end() )
    {
        return nullptr;
    }

    return *session;
}

// websocket_handler_result test_handle( const std::string &str, std::string &req, std::shared_ptr<shared_state> state, std::shared_ptr<msserver::websocket_session> self )
// {
//     if ( str == "connect" )
//     {
//         auto id = handle_connect( str, req );
//         self->send( "ok" );
//         return { id, msserver::websocket_error::ok };
//     }
//     else
//     {
//         int64_t thisid = authentificate_user();

//         auto id = find_session( state, 1 );
//         if ( id != nullptr )
//         {
//             id->send( "Your id is: " + std::to_string( id->get_id() ) );
//             return { thisid, websocket_error::ok };
//         }

//         self->send( "not authentified" );
//         return { -1, websocket_error::not_authorized };
//     }
// }

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
    for ( auto i = THREADS_COUNT - 1; i > 0; --i )
        v.emplace_back( [ &ioc ]
                        { ioc.run(); } );
    ioc.run();

    for ( auto &t : v )
        t.join();

    ioc.run();
}
