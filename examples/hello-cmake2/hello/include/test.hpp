#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("test")]] test_contract : public eosio::contract {
public:
    using contract::contract;

    [[eosio::action("test")]]
    void test();

    // static actions
    using say_hello_action = eosio::action_wrapper<"test"_n, &test_contract::test>;
};
