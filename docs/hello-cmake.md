# Compiling Code with CMake

Previous examples were compiled directly with `cdt-cpp` for C++ code. In actual projects, in order to better manage the code, especially C++ code, the `cmake` tool is often used for compilation.

The following demonstrates how to generate wasm files and library files with cmake.

[Complete Example](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/hello-cmake)

First, let's take a look at the code in `test.cpp`:

```cpp
#include "test.hpp"
#include "utils.hpp"

[[eosio::action("test")]]
void test_contract::test() {
    ::say_hello("alice"_n);
}
```

Here, the function `say_hello` from the `utils` library is called.

Next, let's see what's in `CMakeLists.txt`:

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

Here, `utils.cpp` is first compiled into the library file `libutils.a`, then the `add_contract` macro in cmake is used to compile the contract. `target_link_libraries` links the `utils` library.

To compile:

```bash
mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=`cdt-get-dir`/CDTWasmToolchain.cmake ..
make
cd ..
```

During compilation, the toolchain must be specified as `CDTWasmToolchain.cmake`.

Testing the code:

```python
@chain_test
def test_hello(tester: ChainTester):
    deploy_contract(tester, 'test')

    r = tester.push_action('hello', 'test', b'', {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

To run the test code:

```
ipyeos -m pytest -s -x test.py -k test_hello
```

Output:

```
[(hello,sayhello)->hello]: CONSOLE OUTPUT BEGIN =====================
hello alice
[(hello,sayhello)->hello]: CONSOLE OUTPUT END   =====================
```
