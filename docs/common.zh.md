---
comments: true
---

# 常用智能合约函数

## is_account

声明：

```cpp
inline bool is_account( name n );
```

说明：

用来判断账号存不存在

## has_auth

声明：

```cpp
inline bool has_auth( name n )
```

说明：

用来判断是否有指定账号的`active`权限，也就是Transaction是否有用指定账号的`active`权限所对应的私钥进行签名。用于签名的私钥最少有一个，也可能二个以上。

## require_auth

声明：

```cpp
inline void require_auth( name n )
inline void require_auth( const permission_level& level );
```

说明：

这两个函数在账号不存在或者没有检测到有指定账号的权限时都会抛出异常，不同的是`require_auth( name n )`为检测是否存在`active`权限，而`require_auth( const permission_level& level )`可以检测指定的权限。

## current_time

声明：

```cpp
uint64_t  current_time( void );
```

说明:

用于获取 Transaction 所在的区块的时间

## check

声明：

```cpp
inline void check(bool pred, const std::string& msg);
```

说明：

如果test为false，则会抛出异常，所有在 Transaction 中的执行过的 action 以及本 action 已经执行的对数据库的操作都将被回滚，Transaction将不会上链。这和以太坊中的`revert`机制有比较大的区别。其结果是导致EOS网络相对比较脆弱，因为出异常的 Transaction 被回滚后不消耗资源，也就是不造成成本，导致网络比较容易被攻击。但是在正常的合约中，该函数在智能合约中使用也比较频繁，可参考[eosio.token](https://github.com/AntelopeIO/reference-contracts/tree/main/contracts/eosio.token)中相关的代码


## 示例代码：

```cpp
[[eosio::action("test")]]
void test_contract::test() {
    print_f("% %\n", is_account(get_self()), is_account("noexists"_n));

    print_f("% %\n", has_auth(get_self()), has_auth("alice"_n));

    require_auth(get_self());
    require_auth(permission_level{"hello"_n, "active"_n});
}

```

编译：

```bash
cd examples/commonfunctions
cdt-cpp test.cpp
```

测试代码：

```python
@chain_test
def test_hello(tester: ChainTester):
    deploy_contract(tester, 'test')
    args = {}
    
    r = tester.push_action('hello', 'test', args, {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

测试：

```
ipyeos -m pytest -s -x test.py -k test_hello
```

输出：

```
[(hello,test)->hello]: CONSOLE OUTPUT BEGIN =====================
true false
true false

[(hello,test)->hello]: CONSOLE OUTPUT END   =====================
```

[完整示例代码](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/commonfunctions)
