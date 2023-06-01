---
comments: true
---

# 用CMake编译代码

前面的例子都是用cdt-cpp直接编译C++代码。在实际的项目中，为了更好的管理代码，特别是C++代码，经常要用到cmake工具进行编译。

以下展示如何用cmake生成wasm文件和库文件

[完整示例](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/hello-cmake)

先看下test.cpp中的代码：

```cpp
#include "test.hpp"
#include "utils.hpp"

[[eosio::action("test")]]
void test_contract::test() {
    ::say_hello("alice"_n);
}
```

这里调用了`say_hello`这个在`utils`库中的函数。

下面看下CMakeLists.txt中的内容：

```cmake
add_library(utils
    utils.cpp
)

add_contract(test test
    test.cpp
)

target_link_libraries(test PUBLIC utils)

target_include_directories(test PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

这里先将`utils.cpp`编译成库文件`libutils.a`，再用`add_contract`这个cmake中的宏编译合约。`target_link_libraries`会链接`utils`这个库。


编译：

```bash
mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=`cdt-get-dir`/CDTWasmToolchain.cmake ..
make
cd ..
```

编译的时候必须指定工具链为`CDTWasmToolchain.cmake`。

测试代码：

```python
@chain_test
def test_hello(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'test', b'', {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

运行测试代码：

```
ipyeos -m pytest -s -x test.py -k test_hello
```

输出：

```
[(hello,sayhello)->hello]: CONSOLE OUTPUT BEGIN =====================
hello alice
[(hello,sayhello)->hello]: CONSOLE OUTPUT END   =====================
```
