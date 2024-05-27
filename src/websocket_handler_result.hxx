#pragma once
#ifndef WEBSOCKET_HANDLER_RESULT
#    define WEBSOCKET_HANDLER_RESULT
#    include <cstdint>
#    include <websocket_error.hxx>

namespace msserver
{
    struct websocket_handler_result
    {
        int64_t user_id;
        websocket_error ws_error;
    };
} // namespace msserver

#endif