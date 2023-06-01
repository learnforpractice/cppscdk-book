#include "test.hpp"

[[eosio::action("test")]]
void test_contract::test() {
    record_table mytable( get_self(), 0);
    
    mytable.emplace( _self, [&]( auto& row ) {
        row.primary = 1;
        row.secondary = 2;
        row.data = 3;
    });

    mytable.emplace( _self, [&]( auto& row ) {
        row.primary = 11;
        row.secondary = 22;
        row.data = 33;
    });

    mytable.emplace( _self, [&]( auto& row ) {
        row.primary = 111;
        row.secondary = 222;
        row.data = 333;
    });
}

[[eosio::action("testupdate")]]
void test_contract::test_update() {
    record_table mytable( get_self(), 0);
    auto idx = mytable.get_index<"bysecondary"_n>();

    auto it_sec = idx.find(22);
    check(it_sec != idx.end(), "secondary value not found");
    idx.modify( it_sec, get_self(), [&]( auto& row ) {
        row.data = 1111;
        row.secondary = 222;
    });
}

[[eosio::action("testbound")]]
void test_contract::test_bound() {
    record_table mytable( get_self(), 0);

    auto idx = mytable.get_index<"bysecondary"_n>();
    auto it = idx.lower_bound(2);
    check(it->secondary == 2, "bad value");

    it = idx.upper_bound(2);
    check(it->secondary == 22, "bad value");
}

[[eosio::action("testremove")]]
void test_contract::test_remove() {
    record_table mytable( get_self(), 0);

    auto idx = mytable.get_index<"bysecondary"_n>();
    auto it = idx.lower_bound(2);
    check(it->secondary == 2, "bad value");
    idx.erase(it);
}
