#pragma once
#ifndef TMP_USER_HXX
#    define TMP_USER_HXX
#    include <string>
#    include <cstdint>
#    include <vector>

namespace msserver
{

    class user
    {
      private:
        uint64_t id;
        std::string name;
        std::string password_hash;

      public:
        constexpr user( uint64_t id, std::string name, std::string password_hash ) :
            id( id ), name( name ), password_hash( password_hash ) {}

        constexpr inline uint64_t get_id() const noexcept
        {
            return id;
        }

        constexpr inline const std::string &get_name() const noexcept
        {
            return name;
        }

        constexpr inline const std::string &get_password_hash() const noexcept
        {
            return password_hash;
        }
    };

    static user u1 { 0, "User 1", "Password 1" };
    static user u2 { 1, "User 2", "Password 2" };
    static user u3 { 2, "User 3", "Password 3" };
    static user u4 { 3, "User 4", "Password 4" };

    static std::vector<user> users = { u1, u2, u3, u4 };
} // namespace msserver

#endif