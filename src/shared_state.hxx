#pragma once
#ifndef SHARED_STATE_HXX
#    define SHARED_STATE_HXX
#    include <unordered_set>

namespace msserver
{
    class websocket_session;

    class shared_state
    {
        std::unordered_set<websocket_session *> sessions;

      public:
        explicit shared_state() = default;

        constexpr inline void join( websocket_session &session )
        {
            sessions.insert( &session );
        }

        constexpr inline void leave( websocket_session &session )
        {
            sessions.erase( &session );
        }

        constexpr inline const std::unordered_set<websocket_session *> &get_sessions() const noexcept
        {
            return sessions;
        }
    };
} // namespace msserver

#endif