#include <shared_state.hxx>
#include <websocket_session.hxx>

namespace msserver
{
    void shared_state::join( websocket_session &session )
    {
        uint64_t id;
        if ( _freeIds.empty() )
        {
            sessions.push_back( &session );
            id = _mId++;
        }
        else
        {
            id = _freeIds.front();
            _freeIds.pop_front();
            sessions[ id ] = &session;
        }
        session.id = id;
    }

    void shared_state::leave( websocket_session &session )
    {
        sessions[ session.get_id() ] = nullptr;
        if ( _mId == session.get_id() )
            _mId--;
        else
            _freeIds.push_back( session.get_id() );
    }

    const std::vector<websocket_session *> &shared_state::get_sessions() const noexcept
    {
        return sessions;
    }
} // namespace msserver