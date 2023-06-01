---
comments: true
---

# Common Smart Contract Functions

## is_account

Declaration:

```cpp
inline bool is_account( name n );
```

Description:

Used to determine whether an account exists

## has_auth

Declaration:

```cpp
inline bool has_auth( name n )
```

Description:

Used to determine whether there is `active` permission for the specified account, that is, whether the Transaction is signed with the private key corresponding to the `active` permission of the specified account. The private key used for signing is at least one, and may be more than two.

## require_auth

Declaration:

```cpp
inline void require_auth( name n )
inline void require_auth( const permission_level& level );
```

Description:

These two functions will throw an exception when the account does not exist or the specified account's authority is not detected. The difference is that `require_auth( name n )` checks for the existence of `active` permission, while `require_auth( const permission_level& level )` can check the specified authority.

## current_time

Declaration:

```cpp
uint64_t  current_time( void );
```

Description:

Used to get the time of the block where the Transaction is located

## check

Declaration:

```cpp
inline void check(bool pred, const std::string& msg);
```

Description:

If test is false, an exception will be thrown, all actions executed in the Transaction and operations on the database already executed by this action will be rolled back, and the Transaction will not be chained. This is quite different from the `revert` mechanism in Ethereum. The result is that the EOS network is relatively fragile, because the rolled back Transaction does not consume resources, which means it does not cause costs, making the network more susceptible to attack. However, in normal contracts, this function is also frequently used in smart contracts, you can refer to the related code in [eosio.token](https://github.com/AntelopeIO/reference-contracts/tree/main/contracts/eosio.token)


## Sample code:

```cpp
[[eosio::action("test")]]
void test_contract::test() {
    print_f("% %\n", is_account(get_self()), is_account("noexists"_n));

    print_f("% %\n", has_auth(get_self()), has_auth("alice"_n));

    require_auth(get_self());
    require_auth(permission_level{"hello"_n, "active"_n});
}

```

Compilation:

```bash
cd examples/commonfunctions
cdt-cpp test.cpp
```

Test code:

```python
@chain_test
def test_hello(tester: ChainTester):
    deploy_contract(tester, 'test')
    args = {}
    
    r = tester.push_action('hello', 'test', args, {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

Testing:

```
ipyeos -m pytest -s -x test.py -k test_hello
```

Output:

```
[(hello,test)->hello]: CONSOLE OUTPUT BEGIN =====================
true false
true false

[(hello,test)->hello]: CONSOLE OUTPUT END   =====================
```

[Complete Sample Code](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/commonfunctions)
