#pragma once
#ifndef WEBSOCKET_TARGET_HXX
#    define WEBSOCKET_TARGET_HXX
#    include <string>
#    include <def.hxx>

namespace msserver
{

    struct websocket_target
    {
        std::string ep;
        raw_websocket_request_handler handler;
    };

} // namespace msserver

#endif