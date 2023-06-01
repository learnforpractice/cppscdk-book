#include "test.hpp"

[[eosio::action("test")]]
void test_contract::test() {
    print_f("% %\n", is_account(get_self()), is_account("noexists"_n));

    print_f("% %\n", has_auth(get_self()), has_auth("alice"_n));

    require_auth(get_self());
    require_auth(permission_level{"hello"_n, "active"_n});
}
