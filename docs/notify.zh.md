---
comments: true
---

# require_recipient函数

声明如下：

```rust
void require_recipient(name n)
```

`require_recipient`函数用来通知其它合约本合约调用了某个action，这个action即是调用`require_recipient`所在的action. 如果被通知的合约有相同的action，那么这个action将被调用。

以下的`sender`, `receiver`的代码演示了如何从一个合约发送通知到另一个合约。

[完整代码](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/notify)

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

解释下代码：

- `sender`合约代码部署在`alice`这个合约账号中，其中的`test`action调用了`require_recipient`这个函数来通知`hello`这个账号自己调用了`test`这个action
- `receiver`合约代码部署在`hello`这个账号中，从其中如下所示的代码，`[[eosio::on_notify("*::sayhello")]]`这行代码与正常的action代码不同，其中的`on_notify`参数表示这个action是用来接收其它合约的`sayhello` action 通知的，其`get_self()`和`get_first_receiver()`是不相同的，在本例中,`get_first_receiver()`为账号`alice`，`get_self()`为`hello`，可以运行测试从输出中即可知道。


以下是测试代码：

```python
@chain_test
def test_hello(tester):
    deploy_contracts(tester)
    args = {'n': 'alice'}
    r = tester.push_action('alice', 'sayhello', args, {'alice': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

编译：

```bash
cd examples/notify
cdt-cpp sender.cpp
cdt-cpp receiver.cpp
```

测试：

```bash
ipyeos -m pytest -s -x test.py -k test_hello
```

输出：

```
[(alice,sayhello)->alice]: CONSOLE OUTPUT BEGIN =====================
++++in sender alice: alice
[(alice,sayhello)->alice]: CONSOLE OUTPUT END   =====================

[(alice,sayhello)->hello]: CONSOLE OUTPUT BEGIN =====================
++++in receiver get_self(): hello, get_first_receiver(): alice, n: alice
[(alice,sayhello)->hello]: CONSOLE OUTPUT END   =====================
```

[示例代码](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/notify)
