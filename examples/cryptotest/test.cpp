#include "test.hpp"

[[eosio::action("testhash")]]
void test_contract::test_hash() {
    string msg = string("hello, world");

    assert_sha256(msg.c_str(), msg.size(), sha256(msg.c_str(), msg.size()));
    assert_sha1(msg.c_str(), msg.size(), sha1(msg.c_str(), msg.size()));
    assert_sha512(msg.c_str(), msg.size(), sha512(msg.c_str(), msg.size()));
    assert_ripemd160(msg.c_str(), msg.size(), ripemd160(msg.c_str(), msg.size()));
}

[[eosio::action("testrecover")]]
void test_contract::test_recover(vector<char> msg, signature sig, public_key pub_key) {
    auto digest = sha256(msg.data(), msg.size());
    auto pub = recover_key(digest, sig);
    check(pub == pub_key, "invalid signature");

    assert_recover_key(digest, sig, pub_key);
}

