#pragma once

#include <vector>
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("test")]] test_contract : public eosio::contract {
public:
    using contract::contract;

    [[eosio::action("testhash")]]
    void test_hash();

    [[eosio::action("testrecover")]]
    void test_recover(vector<char> msg, signature sig, public_key pub_key);
};
