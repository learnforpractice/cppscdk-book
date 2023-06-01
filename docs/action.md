Output:

---
comments: true
---

# Use of Inline Action in Smart Contracts

An action can also be initiated in a smart contract, such an action is called an inline action. Note that the action is asynchronous, meaning that only after all the code is executed will the contract code corresponding to the inline action be called. If the called contract does not define a related action or the account does not have a deployed contract, then the call will have no effect, but no exceptions will be thrown. Such empty inline actions are not useless, for example, they can serve as on-chain logs for application queries.

The usage of inline action is illustrated below through an example of an EOS transfer using inline action.

[Full example](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/inlineaction)

```cpp
struct transfer {
    name from;
    name to;
    asset quantity;
    string memo;
};

[[eosio::action("test")]]
void test_contract::test() {
    action(
        {"hello"_n, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(
            "hello"_n,
            "alice"_n,
            asset(5000, symbol("EOS", 4)),
            string("hello, world")
        )
    ).send();

    transfer t = transfer {
        "hello"_n,
        "alice"_n,
        asset(5000, symbol("EOS", 4)),
        string("hello, world")
    };

    action(
        {"hello"_n, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        t
    ).send();
}
```

Interpretation of the above code:

- The code above initiates two inline actions, demonstrating two common uses of actions
- The first method is to construct action data through `make_tuple`, but when using it, you must pay special attention to the type. For example, the `memo` type of transfer is string, and the string pointer type "hello, world" cannot be directly converted, otherwise the result of serialization is not what is desired.
- The second method is to pass the structure method. The compiler will automatically generate serialization and deserialization code for the structure. In the old code, a serialization and deserialization macro must be defined, as shown below. In the new code, this is no longer necessary:
```cpp
struct transfer {
    name from;
    name to;
    asset quantity;
    string memo;
    EOSLIB_SERIALIZE(transfer, (from)(to)(quantity)(memo))
};
```

Link to find macro code: [EOSLIB_SERIALIZE](https://github.com/AntelopeIO/cdt/blob/6f9531319ded3633d47676f4d1ab57b9001ff985/libraries/eosiolib/core/eosio/serialize.hpp#L23)

- Each action implements the function of transferring `0.5000 EOS` from the `hello` account to the `alice` account.

The `hello` and `alice` accounts were created when the test was launched and can be used directly.

Test code:

```python
@chain_test
def test_inline_action(tester: ChainTester):
    deploy_contract(tester, 'test')
    args = {}
    
    logger.info("balance of hello before transfer: %s",  tester.get_balance('hello'))
    logger.info("balance of alice before transfer: %s",  tester.get_balance('alice'))

    r = tester.push_action('hello', 'test', args, {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()

    logger.info("balance of hello after transfer: %s",  tester.get_balance('hello'))
    logger.info("balance of alice after transfer: %s",  tester.get_balance('alice'))
```

Compile:

```bash
cd examples/inlineaction
cdt-cpp test.cpp
```

Run the test:

```bash
ipyeos -m pytest -s -x test.py -k test_inline_action
```

Output:

```
INFO     test:test.py:74 balance of hello before transfer: 50000000000
INFO     test:test.py:75 balance of alice before transfer: 50000000000
INFO     test:test.py:81 balance of hello after transfer: 49999990000
INFO     test:test.py:82 balance of alice after transfer: 50000010000
```


Please note that in order to call inline action in the contract, you need to add the virtual permission `eosio.code` to the `active` permission of the account. In the test code, the following function is used to add the virtual permission `eosio.code` to the `active` permission.


```python
def update_auth(chain, account):
    a = {
        "account": account,
        "permission": "active",
        "parent": "owner",
        "auth": {
            "threshold": 1,
            "keys": [
                {
                    "key": 'EOS6AjF6hvF7GSuSd4sCgfPKq5uWaXvGM2aQtEUCwmEHygQaqxBSV',
                    "weight": 1
                }
            ],
            "accounts": [{"permission":{"actor":account,"permission": 'eosio.code'}, "weight":1}],
            "waits": []
        }
    }
    chain.push_action('eosio', 'updateauth', a, {account:'active'})
```

Summary:

In EOS, in addition to calling contract code by including actions in Transactions, an Action can also be initiated in contract code to call contract code, which is called an Inline Action. To allow contract code to use Inline Action, you must also add the virtual permission `eosio.code` to the `active` permission of the contract account.