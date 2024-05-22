#ifndef HTTP_TARGET_HXX
#define HTTP_TARGET_HXX

#include <boost/beast/http/verb.hpp>
#include <def.hxx>
#include <string>

namespace msserver
{

    struct http_target
    {
        http::verb method;
        std::string endp;
        http_request_handler handler;
    };

} // namespace msserver

#endif