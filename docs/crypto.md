---
comments: true
---

# Cryptography-related Functions

Cryptography-related functions are defined in the CDT's [crypto.hpp](https://github.com/AntelopeIO/cdt/blob/6f9531319ded3633d47676f4d1ab57b9001ff985/libraries/eosiolib/core/eosio/crypto.hpp).


## sha256

sha256 algorithm hash function

```cpp
eosio::checksum256 sha256( const char* data, uint32_t length );
```

Used to check whether the hash256 value is normal, incorrect values will directly throw an exception

```cpp
void assert_sha256( const char* data, uint32_t length, const eosio::checksum256& hash );
```

## sha1

sha1 algorithm hash function

```cpp
eosio::checksum160 sha1( const char* data, uint32_t length );
```

Used to check whether the sha1 hash value is normal, incorrect values will directly throw an exception

```cpp
void assert_sha1( const char* data, uint32_t length, const eosio::checksum160& hash );
```

## sha512

sha512 algorithm hash function

```cpp
eosio::checksum512 sha512( const char* data, uint32_t length );
```

Used to check whether the hash512 value is normal, incorrect values will directly throw an exception

```cpp
void assert_sha512( const char* data, uint32_t length, eosio::checksum512& hash);
```

## ripemd160

ripemd160 algorithm hash function

```cpp
eosio::checksum160 ripemd160( const char* data, uint32_t length );
```

Used to check whether the hash value of the ripemd160 algorithm is normal, incorrect values will directly throw an exception

```cpp
void assert_ripemd160( const char* data, uint32_t length, const eosio::checksum160& hash );
```

## recover_key

Used to recover the public key from the digest and signature

```cpp
eosio::public_key recover_key( const eosio::checksum256& digest, const eosio::signature& sig );
```

Checks whether the signature is normal, abnormal will throw an exception

```cpp
void assert_recover_key( const eosio::checksum256& digest, const eosio::signature& sig, const eosio::public_key& pubkey );
```

## Example:

[Full Example Code](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/cryptotest)

```cpp
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
```

Test Code:

```python
@chain_test
def test_hash(tester):
    deploy_contract(tester, 'test')
    r = tester.push_action('hello', 'testhash', {},{'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()

@chain_test
def test_recover_key(tester):
    deploy_contract(tester, 'test')
    key = eos.create_key()
    pub = key['public']
    priv = key['private']

    msg = b'hello, world'
    h = hashlib.sha256()
    h.update(msg)
    sig = eos.sign_digest(h.hexdigest(), priv)

    args = {
        'msg': msg.hex(),
        'sig': sig,
        'pub_key': pub,
    }
    r = tester.push_action('hello', 'testrecover', args, {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

Compilation:

```bash
cd examples/cryptotest
cdt-cpp test.cpp
```

Testing:

```
ipyeos -m pytest -s -x test.py -k test_hash
ipyeos -m pytest -s -x test.py -k test_recover_key
```

In this example code, the common usage of hash functions and the usage of `recover_key` and `assert_recover_key` are demonstrated. The usage of hash functions is quite simple, here is an explanation of the test code for `recover_key`:
`recover_key` accepts two parameters, `digest` and `signature`. Digest is the result of running sha256 on a binary data. In the above code, it is calculated by hashing `hello, world` using the sha256 algorithm, as shown in the following code:

```python
msg = b'hello, world'
h = hashlib.sha256()
h.update(msg)
sig = eos.sign_digest(h.hexdigest(), priv)
```

The `eos.sign_digest` is used to sign data.

Below is the explanation for `testrecover`:

```cpp
[[eosio::action("testrecover")]]
void test_contract::test_recover(vector<char> msg, signature sig, public_key pub_key) {
    auto digest = sha256(msg.data(), msg.size());
    auto pub = recover_key(digest, sig);
    check(pub == pub_key, "invalid signature");

    assert_recover_key(digest, sig, pub_key);
}
```

The principle of `recover_key` is the same as the verification of the signature in the Transaction by the node. The user signs the digest, and then the third party verifies it with the public key.
In actual smart contract applications, if you need to determine in the smart contract whether a piece of binary data is signed with a specific private key, you can use the above method. The process is as follows:

- The contract saves a public key corresponding to the user's private key
- The user signs the data with their own private key
- The user sends the data and its corresponding signature to the smart contract
- The smart contract can call `recover_key` to recover the public key from the user data and the signature of the data
- The smart contract reads the user's public key saved on the chain, and compares it with the public key recovered by calling `recover_key`. If they are the same, it can be determined that the data is signed by the corresponding user.
