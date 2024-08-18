#pragma once
#ifndef WEBSOCKET_ERROR_HXX
#    define WEBSOCKET_ERROR_HXX

namespace msserver
{
    // struct websocket_err
    // {
    //   private:
    //     int error_id;
    //     std::string error_text;
    //     constexpr websocket_err( int id, std::string text ) :
    //         error_id( id ), error_text( text ) {}

    //   public:
    //     constexpr inline operator std::string() const noexcept
    //     {
    //         return this->error_text;
    //     }

    //     constexpr inline operator int() const noexcept
    //     {
    //         return this->error_id;
    //     }

    //     constexpr inline bool operator==( const websocket_err &&other ) const noexcept
    //     {
    //         return error_id == other.error_id && error_text == other.error_text;
    //     }

    //     constexpr inline operator bool() const noexcept
    //     {
    //         return error_id == 0;
    //     }

    //     const static websocket_err ok;
    //     const static websocket_err not_authorized;
    // }

    // const inline websocket_err::ok             = { 0, "ok" };
    // const inline websocket_err::not_authorized = { 1, "not authorized" };

    enum struct websocket_error
    {
        Ok = 0,
        ServerError,
        WrongData,
        TooManyTries,
        AuthTimeOut,
        WrongAuthData,
        WrongAuthLogin,
    };
} // namespace msserver
#endif