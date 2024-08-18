#include <database.hxx>

namespace msserver
{
    namespace database
    {
        pqxx::connection connection( "port=1605 dbname=postgres user=" STRING( SERVER_DB_USERNAME ) " password=" STRING( SERVER_DB_PASSWORD ) );

        void init()
        {
            pqxx::work txn( connection );

            pqxx::result r = txn.exec(
                "create table if not exists users(login text primary key, password text)" );
            txn.commit();
        }
    } // namespace database
} // namespace msserver