#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("receiver")]] test_contract : public eosio::contract {
public:
    using contract::contract;

    [[eosio::on_notify("*::sayhello")]]
    void say_hello(name n);
};
