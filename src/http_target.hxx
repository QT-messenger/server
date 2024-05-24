#pragma once
#ifndef HTTP_TARGET_HXX
#    define HTTP_TARGET_HXX
#    include <def.hxx>

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