#include <websocket_session.hxx>
#include <boost/asio/buffer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/read.hpp>
#include <listener.hxx>
#include <memory>
#include <http_session.hxx>

namespace msserver
{
    namespace beast     = boost::beast;
    namespace websocket = boost::beast::websocket;

    listener::listener( net::io_context &ioc, tcp::endpoint endpoint ) :
        ioc( ioc ), acceptor( net::make_strand( ioc ) )
    {
        beast::error_code ec;

        acceptor.open( endpoint.protocol(), ec );
        if ( ec )
        {
            fail( ec, "Open" );
            return;
        }

        acceptor.set_option( net::socket_base::reuse_address( true ), ec );
        if ( ec )
        {
            fail( ec, "Set option" );
            return;
        }

        acceptor.bind( endpoint, ec );
        if ( ec )
        {
            fail( ec, "Bind" );
            return;
        }

        acceptor.listen( net::socket_base::max_listen_connections, ec );
        if ( ec )
        {
            fail( ec, "Listen" );
            return;
        }
    }

    void listener::do_accept() noexcept
    {
        acceptor.async_accept( net::make_strand( ioc ), beast::bind_front_handler( &listener::on_accept, shared_from_this() ) );
    }

    void listener::on_accept( beast::error_code ec, tcp::socket socket ) noexcept
    {
        if ( ec )
        {
            fail( ec, "Acception" );
            return;
        }

        auto self = shared_from_this();
        beast::flat_buffer buffer;
        http::request<http::string_body> req;

        http::read( socket, buffer, req, ec );

        on_read( socket, req, ec );
    }

    void listener::on_read( tcp::socket &socket, const http::request<http::string_body> &req, beast::error_code ec )
    {
        if ( ec )
        {
            fail( ec, "Read" );
            return;
        }

        if ( websocket::is_upgrade( req ) )
        {
            std::make_shared<websocket_session>( socket, req.target().to_string(), websocket_targets )->run( req );
        }
        else
        {
            std::make_shared<http_session>( socket, http_targets )->run( req );
        }
    }
} // namespace msserver
