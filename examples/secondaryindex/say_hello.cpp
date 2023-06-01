#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include <eosio/multi_index.hpp>

using namespace eosio;

#define N(name) name##_n.value


struct record {
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

extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        name _self(receiver);

        if (N("sayhello") == action) {
            name code = name(receiver);
            uint64_t scope = "scope"_n.value;
            
            record_table mytable( code, scope);
            
            auto itr = mytable.find(11);
            
            if (mytable.end() == itr) {
                mytable.emplace( _self, [&]( auto& row ) {
                    row.primary = 11;
                    row.secondary = 22;
                    row.data = 111;
                });
            } else {
                mytable.modify( itr, _self, [&]( auto& row ) {
                    row.data = 1111;
                    row.secondary = 44;
                });
            }
            
            itr = mytable.find(11);
            if (itr == mytable.end()) {
                mytable.emplace( _self, [&]( auto& row ) {
                    row.primary = 22;
                    row.secondary = 222;
                    row.data = 2222;
                });
            } else {
                print(itr->primary, " ", itr->secondary, " ", itr->data, "\n");
            }

            auto idx = mytable.get_index<"bysecondary"_n>();
            auto itr_sec = idx.find(222);
            print(itr_sec == idx.end(), "\n");
            print(itr_sec->primary, " ", itr_sec->secondary, " ", itr_sec->data, "\n");

            itr_sec = idx.find(888);
            print(itr_sec == idx.end(), "\n");
            print("+++++++++++done!");
        }
    }
}
