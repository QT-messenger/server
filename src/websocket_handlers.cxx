#include <websocket_session.hxx>
#include <def.hxx>
#include <websocket_handlers.hxx>

namespace msserver
{
    static std::hash<std::string> hasher;
    static int64_t users_count = 0;

    int64_t authentificate_user( json::object &req )
    {
        if ( !req.contains( "user" ) && !req[ "user" ].is_object() )
        {
            return -1;
        }

        return req[ "user" ].as_object()[ "id" ].as_int64();
    }

    int64_t get_send_user( json::object &req )
    {
        if ( !req.contains( "data" ) && !req[ "data" ].as_object().contains( "message" ) && !req[ "data" ].as_object()[ "message" ].as_object().contains( "id" ) )
        {
            return -1;
        }

        return req[ "data" ].as_object()[ "message" ].as_object()[ "id" ].as_int64();
    }

    static inline std::unordered_map<size_t, parsed_websocket_request_handler> get_handlers()
    {
        return {
            { hasher( "connect" ),
              handle_connect },
            { hasher( "sendmessage" ), handle_sendmessage } };
    }

    json::object validate_request( const json::value &value )
    {
        if ( !value.is_object() )
        {
            throw std::exception();
        }

        json::object obj = value.as_object();

        if ( !obj.contains( "request" ) )
        {
            throw std::exception();
        }

        return obj;
    }

    websocket_handler_result handle( const std::string &str, std::string &req, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self )
    {
        json::object request = parse_request_string( str );

        try
        {
            return get_handlers().at( hasher( std::string( request[ "request" ].as_string() ) ) )( request, req, state, self );
        }
        catch ( std::out_of_range )
        {
            return { -1, websocket_error::wrong_request };
        }
    }

    websocket_handler_result handle_connect( json::object &req, std::string &resp, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self ) noexcept
    {
        auto id = authentificate_user( req );

        if ( id < 0 )
        {
            self->send( "fail: couldn't autherntificate user" );
            return {
                id, websocket_error::not_authorized };
        }

        self->send( "ok" );
        return { id, msserver::websocket_error::ok };
    }

    websocket_handler_result handle_sendmessage( json::object &req, std::string &resp, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self ) noexcept
    {
        auto id = authentificate_user( req );

        if ( id < 0 )
        {
            self->send( "fail: couldn't autherntificate user" );
            return {
                id, websocket_error::not_authorized };
        }

        auto user = get_send_user( req );

        if ( id < 0 )
        {
            self->send( "wrong send user data" );
            return {
                id, websocket_error::not_authorized };
        }

        std::string data = std::string( req[ "data" ].as_object()[ "message" ].as_object()[ "data" ].as_string() );

        auto sessions = state->get_sessions();

        auto usession = std::find_if( sessions.begin(), sessions.end(), [ user ]( const websocket_session *session )
                                      { return session->get_id() == user; } );

        if ( usession != sessions.end() )
        {
            ( *usession )->send( data );
            self->send( "ok" );
        }

        return { id, websocket_error::ok };
    }
} // namespace msserver
