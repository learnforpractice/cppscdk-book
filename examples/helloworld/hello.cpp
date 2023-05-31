#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include <eosio/multi_index.hpp>

using namespace eosio;

extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (action == "sayhello"_n.value) {
            print("+++Hello, World");
        }
    }
}
