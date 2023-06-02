---
comments: true
---

# 数据库的操作

链上数据存储和读取是智能合约的一个重要功能。EOS链实现了一个内存数据库，支持以表的方式来存储数据，其中，每一个表的每一项数据都有唯一的主索引，称之为primary key，类型为uint64，表中存储的原始数据为任意长度的二进制数据，在智能合约调用存储数据的功能时，会将类的数据序列化后存进表中，在读取的时候又会通过反序列化的方式将原始数据转成类对象。并且还支持 uint64_t, uint128_t, checksum256, double, long double类型的二重索引表，可以把二重索引表看作数据长度固定的特殊的表。主索引表和二重索引表可以配合起来使用，以实现多重索引的功能。二重索引表可以有多个。二重索引表的值是可以重复的，但是主索引表的主索引必须是唯一的。

下面结合示例来讲解下EOS的链上的内存数据库的使用。

## store/find/update

存储，查找，更新三个功能是数据库最基本的功能了，下面的代码演示了如何通过这三个功能进行链上的计数。

[完整代码](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/counter)


首先，在`test_contract`类中定义一个表结构，表的名称为`mytable`，`multi_index`类用于操作表结构：

```cpp
struct [[eosio::table("mytable")]] record {
    name    account;
    uint64_t   count;
    uint64_t primary_key() const { return account.value; }
};

using record_table = multi_index<"mytable"_n, record>;
```

这里要注意的是，表结构的定义必须放在类中，不然会导致生成的 ABI 文件中没有相关表结构的描述。

下面是`inc`action相关的代码：

```cpp

[[eosio::action("inc")]]
void test_contract::inc(name n) {
    require_auth(n);

    record_table mytable(get_self(), 0);
    
    auto itr = mytable.find(n.value);
    name payer = get_self();

    if (mytable.end() == itr) {
        mytable.emplace( payer, [&]( auto& row ) {
            row.account = n;
            row.count = 1;
            print_f("++++++++++count %", row.count);
        });
    } else {
        mytable.modify( itr, payer, [&]( auto& row ) {
            row.count +=1;
            print_f("++++++++++count %", row.count);
        });
    }
}
```

这个action的功能很简单，就是用于对账号调用action的次数计数。

解释一下上面的代码：

- `require_auth(n);` 确保action有相关账号的权限
- `auto itr = mytable.find(n.value);` 查找以账号的值为主索引所在的记录，返回的值是一个iterator
- `mytable.end() == itr`判断值存不存在，如果不存在，则调用`mytable.emplace`来创建记录，否则调用`mytable.modify`来对计数加1，其中的payer参数用于指定给RAM资源付费的账号。

编译：

```bash
cd examples/counter
cdt-cpp test.cpp
```

测试：

```bash
ipyeos -m pytest -s -x test.py -k test_inc
```

运行的测试代码如下：

```python
@chain_test
def test_inc(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'inc', {'n': 'alice'}, {'alice': 'active'})
    tester.produce_block()

    r = tester.push_action('hello', 'inc', {'n': 'alice'}, {'alice': 'active'})
    tester.produce_block()
```

可以看到如下的输出：
```
++++++++++count 1
++++++++++count 2
```

                                                                                                    
## Remove

下面的代码演示了如何去删除数据库中的一项数据。

```cpp
[[eosio::action("testremove")]]
void test_contract::test_remove(name n) {
    record_table mytable(get_self(), 0);

    auto it = mytable.find(n.value);
    if (mytable.end() != it) {
        mytable.erase(it);
    }
}
```

上面的代码先调用`auto it = mytable.find(n.value);`方法来查找指定的数据。调用`mytable.end() != it`以检查指定的索引所在的数据存不存在。然后再调用`erase`方法来删除对应的记录，

**注意：**

这里的`erase`并不需要调用`emplace`或者`modify`所指定的`payer`账号的权限即可删除数据，所以，在实际的应用中，需要通过调用`require_auth`来确保指定账号的权限才可以删除数据，例如：

```cpp
require_auth("hello"_n)
```

测试代码：

```python
@chain_test
def test_remove(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'teststore', {}, {'alice': 'active'})
    tester.produce_block()
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++++%s", r)

    r = tester.push_action('hello', 'testremove', {"n": eos.n2s(1)}, {'alice': 'active'})
    tester.produce_block()
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++++%s", r)
```

这里，先调用`teststore`这个action来存储了三组测试数据，然后调用`testremove`来删除指定的数据，并且通过`get_table_rows`来确定数据是否已经添加或者被修改或者被删除，相关的`get_table_rows`的用法将在下面介绍。

编译：

```bash
cd examples/counter
cdt-cpp test.cpp
```

测试：

```bash
ipyeos -m pytest -s -x test.py -k test_remove
```
输出：

```
INFO     test:test.py:99 +++++++++{'rows': [{'account': '............1', 'count': 1}, {'account': '............3', 'count': 1}, {'account': '............5', 'count': 1}], 'more': False, 'next_key': ''}
INFO     test:test.py:104 +++++++++{'rows': [{'account': '............3', 'count': 1}, {'account': '............5', 'count': 1}], 'more': False, 'next_key': ''}
```

对比数据可以发现`{'account': '............1', 'count': 1}`这组数据在调用`testremove`后已经被删除了

## lower_bound/upper_bound

这两个方法也是用来查找表中的元素的，不同于`find`方法，这两个函数用于模糊查找。其中，`lower_bound`方法返回`>=`指定`id`第一个元素的`Iterator`，`upper_bound`方法返回`>`指定`id`的第一个元素的`Iterator`，下面来看下用法：

```cpp
[[eosio::action("testbound")]]
void test_contract::test_bound() {
    record_table mytable(get_self(), 0);

    auto it = mytable.lower_bound(1);
    print_f("++++++account: %\n", it->account);

    it = mytable.upper_bound(1);
    print_f("++++++account: %\n", it->account);
}
```

测试代码：

```python
@chain_test
def test_bound(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'teststore', {}, {'alice': 'active'})
    tester.produce_block()

    r = tester.push_action('hello', 'testbound', {}, {'alice': 'active'})
    tester.produce_block()
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++++%s", r)
```

这里，先调用`teststore`这个action来保存三组测试数据。再调用`testbound`进行测试

编译：

```bash
cd examples/counter
cdt-cpp test.cpp
```

运行测试：

```bash
ipyeos -m pytest -s -x test.py -k test_bound
```

输出：

```
++++++account: ............1
++++++account: ............5
```
                                                                                                    
## 利用API来对表进行查询

上面的例子都是讲的如何通过智能合约来操作链上的数据库的表，实际上，通过EOS提供的链下的`get_table_rows`的API的接口，也同样可以对链上的表进行查询工作。在`ipyeos`的`ChainTester`这个类中和`pyeoskit`的`ChainApiAsync`和`ChainApi`这两个类，都提供了`get_table_rows`接口，以方便对表进行查询操作

在Python代码中，`get_table_rows`的定义如下

```python
def get_table_rows(self, _json, code, scope, table,
                                lower_bound, upper_bound,
                                limit,
                                key_type='',
                                index_position='', 
                                encode_type='',
                                reverse = False,
                                show_payer = False):
    """ Fetch smart contract data from an account. 
    key_type: "i64"|"i128"|"i256"|"float64"|"float128"|"sha256"|"ripemd160"
    index_position: "2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"|"10"
    encode_type: "dec" or "hex", default to "dec"
    """
```

解释下这个接口的参数：

- `_json`: True 返回json格式的数据库的记录，False返回16进制表示的原始数据
- `code`： 表所在的账号，
- `scope`： 一般设置为空字符串，当有相同的`code`，`table`时，不同的`scope`可以用来区别不同的表
- `table`： 要查询的数据表名
- `lower_bound`：起始主索引值或者二重索引值，类型由`key_type`指定，可以是数值类型，也可以是数值字符串，也可以是十六进制字符串。
- `upper_bound`：结束主索引值或者二重索引值，类型由`key_type`指定，可以是数值类型，也可以是数值字符串，也可以是十六进制字符串。为空时表示没有设置上限，如果设置了一个非空的值，结果将返回`>=lower_bound`并且`<=upper_bound`的所有值
- `limit`：用于限制返回的值的个数，如果查询的记录多于limit，返回的值中`more`将被设置成`true`, `next_key`: 指向下一个有效的索引。
- `key_type`：值可以为：`"name"`，`"i64"`，`"i128"`，`"i256"`，`"float64"`，`"float128"`，`"sha256"`，`"ripemd160"`。对于主索引，也就是`index_position`为`1`时，值只能是`"name"`或者`"i64"`，对于`index_position >= 2`的二重索引，值可能是其中的任意一个。会在下面单独讲解各个取值时，`lower_bound`和`upper_bound`的编码方式。
- `index_position`：用于指定索引的相对位置，为空或者为`1`表示主索引，从`2`以上表示二重索引的位置
- `encode_type`：什为`"dec"`或者`"hex"`，默认为`dec`。指定`lower_bound`，`upper_bound`的编码格式，以及返回值`next_key`的格式
- `reverse`：指定是否要返回倒序表示的数据
- `show_payer`：指定是否显示RAM资源的付费账号


`key_type`详解：

- "name" 是一个`name`类型的字符串
- "i64" 可以是数值类型，也可以是数值字符串，例如123, "123"
- "i128" 可以是数值类型，或者数值字符串，或者十六进制字符串，例如:123, "123", "0xaabb", "aabb"
- "i256" 当`encode_type`值为`"dec"`或者空字符串`""`时，编码格式为：**小端模式**表示的十六进制的字符串，长度为64个字符。例如：
`fb54b91bfed2fe7fe39a92d999d002c550f0fa8360ec998f4bb65b00c86282f5`将被转换成二个小端模式的`uint128_t`类型的值:`50f0fa8360ec998f4bb65b00c86282f5`, `fb54b91bfed2fe7fe39a92d999d002c5`。当`encode_type`的值为`"hex"`时，采用和`"sha256"`类型一样的大端模式的编码方式。
- "float64" 值为浮点字符串，如`"123.456"`
- "float128" 当`encode_type`值为`"dec"`或者空字符串`""`时，值为浮点字符串，如`"123.456"`，表示的范围只能是`float64`所允许的范围。当`encode_type`的值为`"hex"`时，encode_type值为小端模式表示的十六进制的数据。
"sha256" **大端模式**表示的十六进制的字符串，长度为64个字符，会被转换成两个小端模式的uint128_t类型的值：如`f58262c8005bb64b8f99ec6083faf050c502d099d9929ae37ffed2fe1bb954fb`会被转换成`50f0fa8360ec998f4bb65b00c86282f5`和`fb54b91bfed2fe7fe39a92d999d002c5`，可参考[keytype_converter](https://github.com/AntelopeIO/leap/blob/db132c5fd44e0b1c492e46e3f51e185cd5c59ed0/plugins/chain_plugin/include/eosio/chain_plugin/chain_plugin.hpp#L900)结构的代码。
"ripemd160" 十六进制的字符串，长度为64个字符，大端模式，会被转换成两个小端模式的uint128_t类型的值：如`83a83a3876c64c33f66f33c54f1869edef5b5d4a000000000000000000000000`会被转换成`ed69184fc5336ff6334cc676383aa883`和`0000000000000000000000004a5d5bef`，可参考[keytype_converter](https://github.com/AntelopeIO/leap/blob/db132c5fd44e0b1c492e46e3f51e185cd5c59ed0/plugins/chain_plugin/include/eosio/chain_plugin/chain_plugin.hpp#L918)结构的代码。


`get_table_rows`接口的参数还是非常复杂的，作下总结：

- 如果`lower_bound`和`upper_bound`为空，表示查询不限范围
- 当`key_type`的值为`"i256"`和`"float128"`时，`lower_bound`和`upper_bound`的编码方式还受`encode_type`的影响

要通过`get_table_rows`来查询表，表的结构必须在ABI的描述中可见，在`db_example1`这个例子中，生成的`test.abi`中，包含如下信息即是对表的描述：


```json
"tables": [
    {
        "name": "counter",
        "type": "Counter",
        "index_type": "i64",
        "key_names": [],
        "key_types": []
    }
]
```

测试代码：

```python
@chain_test
def test_offchain_find(tester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'teststore', {}, {'alice': 'active'})
    tester.produce_block()

    r = tester.get_table_rows(False, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++rows: %s", r)

    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++rows: %s", r)

    r = tester.get_table_rows(True, 'hello', '', 'mytable', '1', '3', 10)
    logger.info("+++++++rows: %s", r)
```

运行测试代码:

```bash
ipyeos -m pytest -s -x test.py -k test_offchain_find
```

输出：

```
INFO     test:test.py:114 +++++++rows: {'rows': ['01000000000000000100000000000000', '03000000000000000100000000000000', '05000000000000000100000000000000'], 'more': False, 'next_key': ''}
INFO     test:test.py:117 +++++++rows: {'rows': [{'account': '............1', 'count': 1}, {'account': '............3', 'count': 1}, {'account': '............5', 'count': 1}], 'more': False, 'next_key': ''}
INFO     test:test.py:120 +++++++rows: {'rows': [{'account': '............1', 'count': 1}, {'account': '............3', 'count': 1}], 'more': False, 'next_key': ''}
```

注意，这里的`account`由于是`name`结构，会将数值转换成字符串，所以输出看起来比较奇怪。
                                                                                                    
## 二重索引的存储，查询和更新

请先看下面的例子：

[示例代码](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/secondaryindex)

```cpp
struct [[eosio::table("mytable")]] record {
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



[[eosio::action("test")]]
void test_contract::test() {
    record_table mytable( get_self(), 0);
    
    mytable.emplace( _self, [&]( auto& row ) {
        row.primary = 1;
        row.secondary = 2;
        row.data = 3;
    });

    mytable.emplace( _self, [&]( auto& row ) {
        row.primary = 11;
        row.secondary = 22;
        row.data = 33;
    });

    mytable.emplace( _self, [&]( auto& row ) {
        row.primary = 111;
        row.secondary = 222;
        row.data = 333;
    });
}

[[eosio::action("testsec")]]
void test_contract::test_secondary() {
    record_table mytable( get_self(), 0);
    auto idx = mytable.get_index<"bysecondary"_n>();

    auto it = idx.lower_bound(2);
    check(it->secondary == 2, "bad value");

    it = idx.upper_bound(2);
    check(it->secondary == 22, "bad value");

    auto it_sec = idx.find(22);
    check(it_sec != idx.end(), "secondary value not found");
    idx.modify( it_sec, get_self(), [&]( auto& row ) {
        row.data = 1111;
        row.secondary = 222;
    });
}
```

在这个例子中，定义了一个`uint128_t`类型的二重索引：

`teststore`action存储了3组数据。
`testsec`action演示了调用二重索引的`lower_bound`来查找二重索引，以及调用`modify`这个生成的方法来更新二重索引的数据

测试代码：

```python
@chain_test
def test_secondary(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'teststore', b'', {'hello': 'active'})
    tester.produce_block()
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++++%s", r)

    r = tester.push_action('hello', 'testsec', b'', {'hello': 'active'})
    tester.produce_block()
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++++%s", r)
```

编译：

```bash
cd examples/secondaryindex
cdt-cpp test.cpp
```

运行测试：

```bash
ipyeos -m pytest -s -x test.py -k test_secondary
```

输出：
```
INFO     test:test.py:76 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
INFO     test:test.py:81 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '222', 'data': 1111}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
```

从输出中可知`{'primary': 11, 'secondary': '22', 'data': 33}`已经被修改成`{'primary': 11, 'secondary': '222', 'data': 1111}`.
                   
## 二重索引的删除

```cpp
[[eosio::action("testremove")]]
void test_contract::test_remove() {
    record_table mytable( get_self(), 0);

    auto idx = mytable.get_index<"bysecondary"_n>();
    auto it = idx.find(2);
    check(idx.end() != it, "bad value");
    idx.erase(it);
}
```

解释一下上面的代码：

- `auto it = idx.find(2);` 查找二重索引
- `check(it->secondary == 2, "bad value");` 检查是否查找成功
- `idx.erase(it);` 删除表中的元素，包含主索引和所有二重索引

**注意** 二重索引的索引值是可以重复的，在上面的例子中，find方法实际上返回的第一个有相同索引值的iterator。要确保二重索引的值唯一，必须在调用`mytable.emplace`或者`idx.emplace`之前，先调用`mytable.find`方法或者`idx.lower_bound`方法来确保对应的二重索引不存在。

测试代码：

```python
@chain_test
def test_remove(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'teststore', b'', {'hello': 'active'})
    tester.produce_block()
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++++%s", r)


    r = tester.push_action('hello', 'testremove', b'', {'hello': 'active'})
    tester.produce_block()
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '', '', 10)
    logger.info("+++++++++%s", r)
```

编译：

```bash
cd examples/secondaryindex
cdt-cpp test.cpp
```

运行测试：

```bash
ipyeos -m pytest -s -x test.py -k test_remove
```

输出：

```
INFO     test:test.py:90 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
INFO     test:test.py:96 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}], 'more': False, 'next_key': ''}
```

对比两次get_table_rows的返回值，会发现`{'primary': 111, 'secondary': '222', 'data': 333}`这组数据被删除了

## 利用API来对表进行二重索引查询

在上面的例子中定义了一个类型为`uint128_t`的二重索引，`get_table_rows`API还支持通过二重索引来查找对应的值

```python
@chain_test
def test_offchain_find(tester: ChainTester):
    deploy_contract(tester, 'test')

    args = {}
    r = tester.push_action('hello', 'teststore', args, {'hello': 'active'})
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '1', '', 10, key_type="i64", index_position="1")
    logger.info("+++++++rows: %s", r)

    # 0xde == 222
    r = tester.get_table_rows(True, 'hello', '', 'mytable', '0xde', '', 10, key_type="i128", index_position="2")
    logger.info("+++++++rows: %s", r)
```

**注意**, 在查询`secondary`二索索引值的时候，由于类型是`uint128_t`，对于超出`u64`类型的范围时，可以用十六进制来表示数据，例如上面的`0xde`的十进制数据为`222`

运行测试用例：

```bash
ipyeos -m pytest -s -x test.py -k test_offchain_find
```

上面的测试代码的运行结果如下：

```
INFO     test:test.py:105 +++++++rows: {'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
INFO     test:test.py:109 +++++++rows: {'rows': [{'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
```

## 总结

EOS中的数据存储功能是比较完善的，并且有二重索引表的功能，使数据的查找变得非常的灵活。本章详细讲解了数据库表的增，删，改，查的代码。本章的内容较多，需要花点时间好好消化。可以在示例的基础上作些改动，并且尝试运行以增加对这章知识点的理解。

[示例代码1](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/counter)
[示例代码2](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/secondaryindex)
