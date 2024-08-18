#pragma once
#ifndef DATABASE_HXX
#    define DATABASE_HXX
#    include <pqxx/pqxx>
#    define QUOTE( y )  #y
#    define STRING( x ) QUOTE( x )
namespace msserver
{
    namespace database
    {
        extern pqxx::connection connection;
        void init();
    } // namespace database
} // namespace msserver
#endif