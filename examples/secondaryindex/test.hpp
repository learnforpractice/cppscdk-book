#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("test")]] test_contract : public eosio::contract {
public:
    using contract::contract;

    [[eosio::action("sayhello")]]
    void say_hello();

    // static actions
    using say_hello_action = eosio::action_wrapper<"sayhello"_n, &test_contract::say_hello>;
};
