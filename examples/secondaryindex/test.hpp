#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("test")]] test_contract : public eosio::contract {
public:
    using contract::contract;

    struct [[eosio::table("mytable")]] record {
        uint64_t    primary;
        uint128_t   secondary;
        uint64_t    data;
        uint64_t primary_key() const { return primary; }
        uint128_t get_secondary() const { return secondary; }
    };

    using record_table = multi_index<"mytable"_n,
                record,
                indexed_by< "bysecondary"_n,
                const_mem_fun<record, uint128_t, &record::get_secondary> > >;

    [[eosio::action("teststore")]]
    void test_store();

    [[eosio::action("testsec")]]
    void test_secondary();

    [[eosio::action("testbound")]]
    void test_bound();

    [[eosio::action("testremove")]]
    void test_remove();

};
