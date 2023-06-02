---
comments: true
---

# Database Operations

On-chain data storage and reading is an important function of smart contracts. EOS chain has implemented a memory database that supports data storage in the form of tables. Each piece of data in each table has a unique primary index, known as the primary key, which is of uint64 type. The raw data stored in the table is binary data of any length. When the smart contract calls the function of storing data, it will serialize the class data and store it in the table. When reading, it will deserialize the original data into class objects. It also supports secondary index tables of uint64_t, uint128_t, checksum256, double, long double types, which can be regarded as special tables with fixed data length. The primary index table and the secondary index table can be used together to achieve the function of multi-indexing. There can be multiple secondary index tables. The values of the secondary index table can be repeated, but the primary index of the primary index table must be unique.

The use of EOS's on-chain memory database will be explained below with examples.

## store/find/update

Storage, search, and update are the most basic functions of a database. The following code demonstrates how to use these three functions for on-chain counting.

[Complete Code](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/counter)

First, define a table structure in the `test_contract` class. The table is named `mytable`, and the `multi_index` class is used to operate the table structure:

```cpp
struct [[eosio::table("mytable")]] record {
    name    account;
    uint64_t   count;
    uint64_t primary_key() const { return account.value; }
};

using record_table = multi_index<"mytable"_n, record>;
```

It should be noted here that the definition of the table structure must be placed in the class, otherwise, the generated ABI file will not have a description of the relevant table structure.

Below is the code related to the `inc` action:

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

The function of this action is very simple, it is used to count the number of times the account calls the action.

To explain the above code:

- `require_auth(n);` ensures that the action has the relevant account permissions
- `auto itr = mytable.find(n.value);` looks up the record where the primary index is the value of the account, the return value is an iterator
- `mytable.end() == itr` determines whether the value exists or not. If it does not exist, call `mytable.emplace` to create a record, otherwise call `mytable.modify` to add 1 to the count. The payer parameter is used to specify the account paying for the RAM resources.

Compilation:

```bash
cd examples/counter
cdt-cpp test.cpp
```

Test:

```bash
ipyeos -m pytest -s -x test.py -k test_inc
```

The test code that runs is as follows:

```python
@chain_test
def test_inc(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'inc', {'n': 'alice'}, {'alice': 'active'})
    tester.produce_block()

    r = tester.push_action('hello', 'inc', {'n': 'alice'}, {'alice': 'active'})
    tester.produce_block()
```

You can see the following output:
```
++++++++++count 1
++++++++++count 2
```

                                                                                                    
## Remove

The following code demonstrates how to remove an item of data from the database.

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

The above code first calls the `auto it = mytable.find(n.value);` method to look up the specified data. Call `mytable.end() != it` to check whether the data at the specified index exists or not. Then call the `erase` method to delete the corresponding record.

**Note:**

Here, `erase` does not need to call the permissions of the `payer` account specified by `emplace` or `modify` to delete data. Therefore, in actual applications, you need to call `require_auth` to ensure that you have the authority of the specified account to delete data, for example:

```cpp
require_auth("hello"_n)
```

Test code:

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

Here, first call the `teststore` action to store three groups of test data, then call `testremove` to delete the specified data, and use `get_table_rows` to confirm whether the data has been added, modified, or deleted. The usage of `get_table_rows` will be introduced later.

Compile:

```bash
cd examples/counter
cdt-cpp test.cpp
```

Test:

```bash
ipyeos -m pytest -s -x test.py -k test_remove
```

Output:

```
INFO     test:test.py:99 +++++++++{'rows': [{'account': '............1', 'count': 1}, {'account': '............3', 'count': 1}, {'account': '............5', 'count': 1}], 'more': False, 'next_key': ''}
INFO     test:test.py:104 +++++++++{'rows': [{'account': '............3', 'count': 1}, {'account': '............5', 'count': 1}], 'more': False, 'next_key': ''}
```

The comparison data shows that the data set `{'account': '............1', 'count': 1}` has been deleted after calling `testremove`.

## lower_bound/upper_bound

These two methods are also used to find elements in the table. Different from the `find` method, these two functions are used for fuzzy search. Among them, the `lower_bound` method returns the `Iterator` of the first element `>=` the specified `id`, and the `upper_bound` method returns the `Iterator` of the first element `>` the specified `id`. Let's take a look at the usage:

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

Test code:

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

Here, first call the `teststore` action to save three sets of test data. Then call `testbound` for testing.

Compile:

```bash
cd examples/counter
cdt-cpp test.cpp
```

Run the test:

```bash
ipyeos -m pytest -s -x test.py -k test_bound
```

Output:

```
++++++account: ............1
++++++account: ............5
```

## Use API to query the table

The above examples are all about how to operate the database tables on the chain through smart contracts. In fact, through the off-chain `get_table_rows` API interface provided by EOS, the tables on the chain can also be queried. In the `ChainTester` class of `ipyeos` and the `ChainApiAsync` and `ChainApi` classes of `pyeoskit`, the `get_table_rows` interface is provided to facilitate the query operation of the table.

In the Python code, the definition of `get_table_rows` is as follows:


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

Let's break down the parameters for this function:

- `_json`: If True, it returns JSON-formatted database records, if False, it returns the raw data in hexadecimal format.
- `code`: Represents the account where the table is located.
- `scope`: Generally set as an empty string. When `code` and `table` are the same, different `scope` can be used to differentiate different tables.
- `table`: The name of the table to be queried.
- `lower_bound`: The starting value of the primary or secondary index, specified by `key_type`. It can be numeric, a string of numbers, or a hexadecimal string.
- `upper_bound`: The ending value of the primary or secondary index, specified by `key_type`. It can be numeric, a string of numbers, or a hexadecimal string. If empty, it means no upper limit has been set. If a non-empty value is set, the results will return all values that are `>=lower_bound` and `<=upper_bound`.
- `limit`: Limits the number of returned values. If the queried records exceed the limit, `more` will be set to `true` in the returned values, and `next_key` will point to the next valid index.
- `key_type`: The values can be: `"name"`, `"i64"`, `"i128"`, `"i256"`, `"float64"`, `"float128"`, `"sha256"`, `"ripemd160"`. For the primary index (i.e., `index_position` is `1`), the value can only be `"name"` or `"i64"`. For secondary index (i.e., `index_position >= 2`), the value could be any of the listed types. The encoding method of `lower_bound` and `upper_bound` under each value will be explained separately below.
- `index_position`: Specifies the relative position of the index. If it's empty or `1`, it denotes the primary index. Any number above `2` denotes the position of the secondary index.
- `encode_type`: It's either `"dec"` or `"hex"`, defaulting to `"dec"`. It specifies the encoding format of `lower_bound`, `upper_bound`, and the return value `next_key`.
- `reverse`: Specifies whether the data should be returned in reverse order.
- `show_payer`: Specifies whether to display the RAM resource paying account.

Detailed explanation for `key_type`:

- "name" is a `name` type string.
- "i64" can be a numeric type or a string of numbers, such as 123 or "123".
- "i128" can be a numeric type, a string of numbers, or a 32-character hexadecimal string, such as: 123, "123", "0xaabb", "aabb".
- "i256" when the value of `encode_type` is `"dec"` or an empty string `""`, the encoding format is: a hexadecimal string, represented in **little-endian mode**, 64 characters in length. For example: `fb54b91bfed2fe7fe39a92d999d002c550f0fa8360ec998f4bb65b00c86282f5` will be converted into two `uint128_t` type values in little-endian mode: `50f0fa8360ec998f4bb65b00c86282f5` and `fb54b91bfed2fe7fe39a92d999d002c5`. When the value of `encode_type` is `"hex"`, it uses the same encoding method as the `"sha256"` type, which is big-endian mode.
- "float64": The value is a floating-point string, like `"123.456"`.
- "float128": When the value of `encode_type` is `"dec"` or an empty string `""`, the value is a floating-point string, like `"123.456"`, and the range it represents can only be within the range allowed by `float64`. When the value of `encode_type` is `"hex"`, `encode_type` represents the data as a hexadecimal string in little-endian mode.
- "sha256": A hexadecimal string represented in **big-endian mode**, 64 characters long, will be converted into two `uint128_t` type values in little-endian mode: such as `f58262c8005bb64b8f99ec6083faf050c502d099d9929ae37ffed2fe1bb954fb` will be converted into `50f0fa8360ec998f4bb65b00c86282f5` and `fb54b91bfed2fe7fe39a92d999d002c5`. Refer to the [keytype_converter](https://github.com/AntelopeIO/leap/blob/db132c5fd44e0b1c492e46e3f51e185cd5c59ed0/plugins/chain_plugin/include/eosio/chain_plugin/chain_plugin.hpp#L900) structure's code for more details.
- "ripemd160": A hexadecimal string, 64 characters long, big-endian mode, will be converted into two `uint128_t` type values in little-endian mode: such as `83a83a3876c64c33f66f33c54f1869edef5b5d4a000000000000000000000000` will be converted into `ed69184fc5336ff6334cc676383aa883` and `0000000000000000000000004a5d5bef`. Refer to the [keytype_converter](https://github.com/AntelopeIO/leap/blob/db132c5fd44e0b1c492e46e3f51e185cd5c59ed0/plugins/chain_plugin/include/eosio/chain_plugin/chain_plugin.hpp#L918) structure's code for more details.

The `get_table_rows` function's parameters are quite complex, here's a summary:

- If `lower_bound` and `upper_bound` are empty, it means the query has no range limit.
- When the value of `key_type` is `"i256"` and `"float128"`, the encoding method of `lower_bound` and `upper_bound` is also affected by `encode_type`.


To query the table through `get_table_rows`, the structure of the table must be visible in the ABI description. In the `db_example1` example, the generated `test.abi` contains the following information, which is a description of the table:

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

Test code:

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

Run test code:

```bash
ipyeos -m pytest -s -x test.py -k test_offchain_find
```


Output:

```
INFO     test:test.py:114 +++++++rows: {'rows': ['01000000000000000100000000000000', '03000000000000000100000000000000', '05000000000000000100000000000000'], 'more': False, 'next_key': ''}
INFO     test:test.py:117 +++++++rows: {'rows': [{'account': '............1', 'count': 1}, {'account': '............3', 'count': 1}, {'account': '............5', 'count': 1}], 'more': False, 'next_key': ''}
INFO     test:test.py:120 +++++++rows: {'rows': [{'account': '............1', 'count': 1}, {'account': '............3', 'count': 1}], 'more': False, 'next_key': ''}
```

Please note that the `account` here is structured as a `name`, which will convert the value into a string, so the output may look a bit strange.

## Storage, Querying, and Updating of Secondary Indexes

Please see the example below:

[Example Code](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/secondaryindex)

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

In this example, a `uint128_t` type secondary index is defined:

The `teststore` action stores 3 sets of data.
The `testsec` action demonstrates the calling of `lower_bound` on the secondary index to find the secondary index, and the calling of the generated `modify` method to update the data of the secondary index.

Test code:

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

Compilation:

```bash
cd examples/secondaryindex
cdt-cpp test.cpp
```

Run the test:

```bash
ipyeos -m pytest -s -x test.py -k test_secondary
```

Output:

```
INFO     test:test.py:76 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
INFO     test:test.py:81 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '222', 'data': 1111}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
```

From the output, we can see that `{'primary': 11, 'secondary': '22', 'data': 33}` has been modified to `{'primary': 11, 'secondary': '222', 'data': 1111}`.

## Deletion of Secondary Indexes

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

Explanation of the above code:

- `auto it = idx.find(2);` searches for the secondary index
- `check(it->secondary == 2, "bad value");` checks if the search is successful
- `idx.erase(it);` removes the element from the table, including the main index and all secondary indexes

**Note** The index value of the secondary index can be repeated. In the example above, the find method actually returns the first iterator with the same index value. To ensure the uniqueness of the value of the secondary index, you must call the `mytable.find` method or the `idx.lower_bound` method before calling `mytable.emplace` or `idx.emplace` to ensure that the corresponding secondary index does not exist.

Test code:

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

Compilation:

```bash
cd examples/secondaryindex
cdt-cpp test.cpp
```

Run the test:

```bash
ipyeos -m pytest -s -x test.py -k test_remove
```

Output:

```
INFO     test:test.py:90 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
INFO     test:test.py:96 +++++++++{'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}], 'more': False, 'next_key': ''}
```

Comparing the return values of the two get_table_rows, you will find that the data group `{'primary': 111, 'secondary': '222', 'data': 333}` has been deleted.

## Use API for Secondary Index Query on Tables

In the above example, a secondary index of type `uint128_t` was defined. The `get_table_rows` API also supports finding the corresponding values through the secondary index.

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

**Note**, when querying the value of the `secondary` secondary index, because the type is `uint128_t`, when it exceeds the range of the `u64` type, hexadecimal can be used to represent data. For example, the decimal data of the above `0xde` is `222`.

Run the test case:

```bash
ipyeos -m pytest -s -x test.py -k test_offchain_find
```

The execution result of the above test code is as follows:

```
INFO     test:test.py:105 +++++++rows: {'rows': [{'primary': 1, 'secondary': '2', 'data': 3}, {'primary': 11, 'secondary': '22', 'data': 33}, {'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
INFO     test:test.py:109 +++++++rows: {'rows': [{'primary': 111, 'secondary': '222', 'data': 333}], 'more': False, 'next_key': ''}
```

## Conclusion

The data storage function in EOS is relatively complete, and it has the function of the secondary index table, which makes the data search very flexible. This chapter gives a detailed explanation of the addition, deletion, modification, and query of database tables. There is a lot of content in this chapter, so take some time to digest it properly. You can make some modifications based on the example and try to run it to increase the understanding of the knowledge points in this chapter.

[Example Code 1](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/counter)
[Example Code 2](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/secondaryindex)