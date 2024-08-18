#include <websocket_session.hxx>
#include <def.hxx>
#include <websocket_handlers.hxx>
#include <database.hxx>

namespace msserver
{
    static std::hash<std::string> hasher;
    websocket_error authentificate_user( json::object &req )
    {
        if ( !req[ "user" ].is_object() || !req[ "user" ].as_object().contains( "login" ) )
        {
            return websocket_error::WrongData;
        }
        pqxx::work txn( msserver::database::connection );
        try
        {
            auto exist { txn.exec( std::format( "SELECT 1 FROM users WHERE login = {};", req[ "user" ].as_object()[ "login" ].as_string().data() ) ) };
        }
        catch ( pqxx::sql_error &e )
        {
            return websocket_error::WrongAuthLogin;
        }
        if ( txn.exec( std::format( "SELECT password FROM users WHERE login = {};", req[ "user" ].as_object()[ "login" ].as_string().data() ) )[ 0 ][ 0 ].get<std::string>() == req[ "user" ].as_object()[ "password" ].as_string() )
            return websocket_error::Ok;
        else
            return websocket_error::WrongAuthData;
    }

    bool is_user_exists( json::object &req )
    {
        if ( !req.contains( "data" ) && !req[ "data" ].as_object().contains( "message" ) && !req[ "data" ].as_object()[ "message" ].as_object().contains( "login" ) )
        {
            return 0;
        }
        else
        {
            pqxx::work txn( msserver::database::connection );
            auto res { txn.exec( std::format( "SELECT login FROM users WHERE login = {};", req[ "login" ].as_string().data() ) ) };
            if ( res[ 0 ][ 0 ].get<std::string>() == req[ "data" ].as_object()[ "message" ].as_object()[ "login" ].as_string() )
                return 1;
            else
                return 0;
        }
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

    websocket_error handle( const std::string &str, std::string &req, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self )
    {
        json::object request = parse_request_string( str );

        try
        {
            return get_handlers().at( hasher( std::string( request[ "request" ].as_string() ) ) )( request, req, state, self );
        }
        catch ( std::out_of_range )
        {
            return websocket_error::ServerError;
        }
    }

    websocket_error handle_connect( json::object &req, std::string &resp, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self ) noexcept
    {
        auto res { authentificate_user( req ) };
        json::object answ;
        answ[ "answer" ] = static_cast<int64_t>( res );
        if ( res != websocket_error::Ok )
        {
            self->send( json::serialize( answ ) );
            return res;
        }
        self->send( json::serialize( answ ) );
        return res;
    }

    websocket_error handle_sendmessage( json::object &req, std::string &resp, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self ) noexcept
    {
        auto res = authentificate_user( req );
        json::object answ;
        if ( res != websocket_error::Ok )
        {
            answ[ "answer" ] = static_cast<int64_t>( res );
            self->send( json::serialize( answ ) );
            return res;
        }

        auto user = is_user_exists( req );

        if ( !user )
        {
            answ[ "answer" ] = static_cast<int64_t>( websocket_error::WrongData );
            self->send( json::serialize( answ ) );
            return websocket_error::WrongData;
        }

        std::string data = std::string( req[ "data" ].as_object()[ "message" ].as_object()[ "data" ].as_string() );
        auto sessions    = state->get_sessions();
        auto usession    = std::find_if( sessions.begin(), sessions.end(), [ user ]( const websocket_session *session )
                                         { return session->get_id() == user; } );

        if ( usession != sessions.end() )
        {
            ( *usession )->send( data );
            answ[ "answer" ] = static_cast<int64_t>( websocket_error::Ok );
            self->send( json::serialize( answ ) );
        }

        return websocket_error::Ok;
    }
} // namespace msserver
