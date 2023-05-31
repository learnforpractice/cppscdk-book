#include "test.hpp"

struct transfer {
    name from;
    name to;
    asset quantity;
    string memo;
};

[[eosio::action("test")]]
void test_contract::test() {
    action(
        {"hello"_n, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(
            "hello"_n,
            "alice"_n,
            asset(5000, symbol("EOS", 4)),
            string("hello, world")
        )
    ).send();

    transfer t = transfer {
        "hello"_n,
        "alice"_n,
        asset(5000, symbol("EOS", 4)),
        string("hello, world")
    };

    action(
        {"hello"_n, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        t
    ).send();
}
