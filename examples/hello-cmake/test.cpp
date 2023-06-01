#include "test.hpp"
#include "utils.hpp"

[[eosio::action("test")]]
void test_contract::test() {
    ::say_hello("alice"_n);
}

