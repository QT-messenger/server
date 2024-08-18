#pragma once
#ifndef SHARED_STATE_HXX
#    define SHARED_STATE_HXX
#    include <deque>
#    include <vector>

namespace msserver
{
    class websocket_session;

    class shared_state
    {
        std::vector<websocket_session *> sessions;
        uint64_t _mId { 0 };
        std::deque<uint64_t> _freeIds;

      public:
        uint64_t getFreeId();
        explicit shared_state() = default;
        void join( websocket_session &session );
        void leave( websocket_session &session );
        const std::vector<websocket_session *> &get_sessions() const noexcept;
    };
} // namespace msserver

#endif