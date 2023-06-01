---
comments: true
---

# require_recipient Function

Declaration is as follows:

```rust
void require_recipient(name n)
```

The `require_recipient` function is used to notify other contracts that this contract has called a certain action, which is the action in which `require_recipient` is called. If the notified contract has the same action, then this action will be called.

The following `sender`, `receiver` code demonstrates how to send a notification from one contract to another.

[Complete Code](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/notify)

```cpp
// sender
[[eosio::action("sayhello")]]
void test_contract::say_hello(name n) {
    print_f("++++in sender %: %", get_self(), n);
    require_recipient("hello"_n);
}
```

```cpp
// receiver
[[eosio::on_notify("*::sayhello")]]
void test_contract::say_hello(name n) {
    print_f("++++in receiver get_self(): %, get_first_receiver(): %, n: %", get_self(), get_first_receiver(), n);
}
```

Explanation of the code:

- The `sender` contract code is deployed in the `alice` contract account. The `test` action within it calls the `require_recipient` function to notify the `hello` account that it has called the `test` action.
- The `receiver` contract code is deployed in the `hello` account. As shown in the code below, the line of code `[[eosio::on_notify("*::sayhello")]]` is different from the normal action code. The `on_notify` parameter indicates that this action is used to receive the `sayhello` action notification from other contracts. Its `get_self()` and `get_first_receiver()` are not the same. In this example, `get_first_receiver()` is the `alice` account and `get_self()` is `hello`. You can run the test to know from the output.


Here is the test code:

```python
@chain_test
def test_hello(tester):
    deploy_contracts(tester)
    args = {'n': 'alice'}
    r = tester.push_action('alice', 'sayhello', args, {'alice': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

Compilation:

```bash
cd examples/notify
cdt-cpp sender.cpp
cdt-cpp receiver.cpp
```

Testing:

```bash
ipyeos -m pytest -s -x test.py -k test_hello
```

Output:

```
[(alice,sayhello)->alice]: CONSOLE OUTPUT BEGIN =====================
++++in sender alice: alice
[(alice,sayhello)->alice]: CONSOLE OUTPUT END   =====================

[(alice,sayhello)->hello]: CONSOLE OUTPUT BEGIN =====================
++++in receiver get_self(): hello, get_first_receiver(): alice, n: alice
[(alice,sayhello)->hello]: CONSOLE OUTPUT END   =====================
```

[Sample Code](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/notify)