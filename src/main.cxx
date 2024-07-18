#ifndef THREADS_COUNT
#    error please, define SERVER_THREADS_COUNT variable.
#endif
#ifndef SERVER_DB_USERNAME
#    error Please define SERVER_DB_USERNAME envirenment variable.
#endif
#ifndef SERVER_DB_PASSWORD
#    error Please define SERVER_DB_PASSWORD envirenment variable.
#endif
#include <pqxx/pqxx>
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
#define QUOTE( y )  #y
#define STRING( x ) QUOTE( x )

int main()
{
    // pqxx::connection c( "port=1605 dbname=postgres user=" STRING( SERVER_DB_USERNAME ) " password=" STRING( SERVER_DB_PASSWORD ) );

    // {
    //     pqxx::work txn( c );

    //     pqxx::result r = txn.exec(
    //         "create table if not exists employee(id serial primary key, name text, salary integer)" );

    //     txn.commit();
    // }

    // c.prepare( "insert_employee", "insert into employee(name, salary) values($1, $2)" );

    // auto start = std::chrono::high_resolution_clock::now();

    // pqxx::work txn( c );

    // txn.exec_prepared0( "insert_employee", "Warren", 12345 );

    // pqxx::result r = txn.exec(
    //     "SELECT id "
    //     "FROM Employee "
    //     "WHERE name =" +
    //     txn.quote( "Warren" ) + " order by id desc limit 1" );

    // int employee_id = r[ 0 ][ 0 ].as<int>();

    // txn.exec(
    //     "UPDATE EMPLOYEE "
    //     "SET salary = salary + 1 "
    //     "WHERE id = " +
    //     txn.quote( employee_id ) );

    // txn.commit();
    msserver::net::io_context ioc( THREADS_COUNT );
    auto ls = std::make_shared<msserver::listener>( ioc, msserver::tcp::endpoint( boost::asio::ip::make_address( HTTP_TARGET_HOST ), HTTP_TARGET_PORT ) );

    ls->get( "/hw", []( http::request<http::string_body>, http::response<http::string_body> &response )
             { response.body() = "Hello world!"; response.set(http::field::content_type, "text/plain"); } );

    ls->websocket( "/", msserver::handle );
    ls->run();

    msserver::net::signal_set signals( ioc, SIGINT, SIGTERM );
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
}
