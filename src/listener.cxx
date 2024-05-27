#include <memory>
#include <boost/beast.hpp>
#include <websocket_session.hxx>
#include <boost/asio/buffer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <listener.hxx>
#include <http_session.hxx>

namespace msserver
{
    namespace beast = boost::beast;

    listener::listener( net::io_context &ioc, tcp::endpoint endpoint ) :
        ioc( ioc ), acceptor( net::make_strand( ioc ) ), state( std::make_shared<shared_state>() )
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
        else
        {
            std::make_shared<http_session>( socket, http_targets, websocket_targets, state )->run();
        }

        do_accept();
    }
} // namespace msserver
