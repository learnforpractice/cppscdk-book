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
};
