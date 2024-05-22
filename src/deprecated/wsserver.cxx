#include <boost/asio/buffer.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/exception/exception.hpp>
#include <boost/system/error_code.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/system_error.hpp>
#include <boost/throw_exception.hpp>
#include <wsserver.hxx>
#include <def.hxx>
#include <thread>
#include <iostream>

namespace msserver
{

    void handleConnection( tcp::socket socket, webSocketCallback onRequest )
    {
        try
        {
            websocket::stream<tcp::socket> ws { std::move( socket ) };

            ws.set_option( websocket::stream_base::decorator(
                []( websocket::response_type &res )
                {
                    res.set( http::field::server,
                             std::string( BOOST_BEAST_VERSION_STRING ) +
                                 " websocket-server-sync" );
                } ) );

            ws.accept();

            while ( true )
            {
                beast::flat_buffer buffer;
                ws.read( buffer );

                std::string result;

                onRequest( std::string( buffer_cast<const char *>( buffer.data() ), buffer.size() ), result );

                ws.write( net::buffer( result ) );
            }
        }
        catch ( boost::wrapexcept<boost::system::system_error> e )
        {
            std::cerr << "Connection was closed!" << std::endl;
        }
        catch ( std::exception e )
        {
            std::cerr << e.what() << std::endl;
        }
    }

    wsserver::wsserver( const std::string &address, uint16_t port ) :
        addr( boost::asio::ip::make_address( address ) ),
        port( port ),
        ioc( net::io_context { THREADS_COUNT } ),
        acceptor( ioc, { addr, port } ),
        work( true )
    {
    }

    void wsserver::run( webSocketCallback onRequest )
    {
        while ( work )
        {
            tcp::socket socket { this->ioc };
            acceptor.accept( socket );

            std::thread { handleConnection, std::move( socket ), onRequest }.detach();
        }
    }

} // namespace msserver