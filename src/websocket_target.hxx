#ifndef WEBSOCKET_TARGET_HXX
#define WEBSOCKET_TARGET_HXX

#include <def.hxx>
#include <string>

namespace msserver
{

    struct websocket_target
    {
        std::string ep;
        websocket_request_handler handler;
    };

} // namespace msserver

#endif