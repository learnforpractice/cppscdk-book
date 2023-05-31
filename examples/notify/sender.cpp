#include "sender.hpp"

[[eosio::action("sayhello")]]
void test_contract::say_hello(name n) {
    print_f("++++in sender %: %", get_self(), n);
    require_recipient("hello"_n);
}
