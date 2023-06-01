---
comments: true
---

# 密码学相关函数

密码学相关的函数在cdt中的[crypto.hpp](https://github.com/AntelopeIO/cdt/blob/6f9531319ded3633d47676f4d1ab57b9001ff985/libraries/eosiolib/core/eosio/crypto.hpp)中定义。


## sha256

sha256算法hash函数

```cpp
eosio::checksum256 sha256( const char* data, uint32_t length );
```

用于检测hash256值是否正常，不正确会直接抛出异常

```cpp
void assert_sha256( const char* data, uint32_t length, const eosio::checksum256& hash );
```

## sha1

sha1算法hash函数

```cpp
eosio::checksum160 sha1( const char* data, uint32_t length );
```

用于检测sha1 hash值是否正常，不正确会直接抛出异常

```cpp
void assert_sha1( const char* data, uint32_t length, const eosio::checksum160& hash );
```

## sha512

sha512算法hash函数

```cpp
eosio::checksum512 sha512( const char* data, uint32_t length );
```

用于检测hash512值是否正常，不正确会直接抛出异常

```cpp
void assert_sha512( const char* data, uint32_t length, eosio::checksum512& hash);
```

## ripemd160

ripemd160算法hash函数

```cpp
eosio::checksum160 ripemd160( const char* data, uint32_t length );
```

用于检测ripemd160算法的hash值是否正常，不正确会直接抛出异常

```cpp
void assert_ripemd160( const char* data, uint32_t length, const eosio::checksum160& hash );
```

## recover_key

用于从digest和signture中恢复出公钥

```cpp
eosio::public_key recover_key( const eosio::checksum256& digest, const eosio::signature& sig );
```

检查签名是否正常，不正常会抛出异常

```cpp
void assert_recover_key( const eosio::checksum256& digest, const eosio::signature& sig, const eosio::public_key& pubkey );
```

## 示例：

[完整示例代码](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/cryptotest)

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

测试代码：

```python
@chain_test
def test_hash(tester):
    deploy_contract(tester, 'test')
    r = tester.push_action('hello', 'testhash', {}, {'hello': 'active'})
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

编译：

```bash
cd examples/cryptotest
cdt-cpp test.cpp
```

测试：

```
ipyeos -m pytest -s -x test.py -k test_hash
ipyeos -m pytest -s -x test.py -k test_recover_key
```

在这个示例代码中，分别演示了常用的 hash 函数的用法以及`recover_key`和`assert_recover_key`的用法。hash函数的用法比较简单，这里解释一下`recover_key`的测试代码：
`recover_key`接受二个参数，分别是`digest`和`signature`，digest是对一个二进制数据进行 sha256 运行的结果。在上面的代码中是通过对`hello, world`进行sha256算法的hash计算，如下代码所示：

```python
msg = b'hello, world'
h = hashlib.sha256()
h.update(msg)
sig = eos.sign_digest(h.hexdigest(), priv)
```

其中的`eos.sign_digest`用来对数据进行签名。


下面是对`testrecover`的解释：

```cpp
[[eosio::action("testrecover")]]
void test_contract::test_recover(vector<char> msg, signature sig, public_key pub_key) {
    auto digest = sha256(msg.data(), msg.size());
    auto pub = recover_key(digest, sig);
    check(pub == pub_key, "invalid signature");

    assert_recover_key(digest, sig, pub_key);
}
```

`recover_key`的原理和节点检验 Transaction 中的签名是否有效是一样的，就是用户通过对 digest 进行签名，然后第三方再用公钥进行验证。
在实际的智能合约的应用中，如果要在智能合约里判断某段二进制数据是否是用特定的私钥进行的签名也可以用上面的办法。过程如下：

- 合约中保存用户一个私钥对应的公钥
- 用户用自己的私钥对数据进行签名
- 用户将数据，以及对应的签名传给智能合约
- 智能合约可以调用`recover_key`从用户数据，以及对数据的签名中还原出公钥
- 智能合约读取保存在链上的用户公钥，与通过调用`recover_key`还原出的公钥进行比较，相同即可以确定数据是对应的用户签的名
