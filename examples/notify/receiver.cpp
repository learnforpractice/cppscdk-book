#include "receiver.hpp"

[[eosio::on_notify("*::sayhello")]]
void test_contract::say_hello(name n) {
    print_f("++++in receiver get_self(): %, get_first_receiver(): %, n: %", get_self(), get_first_receiver(), n);
}
