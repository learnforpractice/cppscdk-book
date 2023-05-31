#include "test.hpp"

[[eosio::action("sayhello")]]
void test_contract::say_hello() {
    print("++++hello, world");
}
