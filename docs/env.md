# Setting Up the Development Environment

## Install cdt

```
python3 -m pip install -U eoscdt
```

If your platform is Windows or MacOSX M1/M2, you also need to download an image containing the eoscdt tools:

```bash
docker pull ghcr.io/uuosio/scdt:latest
```

The recommended software for installing and running Docker on macOS is [OrbStack](https://orbstack.dev/download). For other platforms, you can use [Docker Desktop](https://www.docker.com/products/docker-desktop).

## Install the EOS Testing Framework

Install ipyeos:

```bash
python3 -m pip install -U ipyeos
```

If your platform is Windows or MacOSX M1/M2, you also need to download an image containing the ipyeos tools:

```bash
docker pull ghcr.io/uuosio/ipyeos:latest
```

The recommended software for installing and running Docker on macOS is [OrbStack](https://orbstack.dev/download). For other platforms, you can use [Docker Desktop](https://www.docker.com/products/docker-desktop).

## Install the EOS Python Tool Package

```bash
python3 -m pip install -U pyeoskit
```

pyeoskit is used to deploy contracts to the main network or test network.

## Check the Environment

Download the test code: [helloworld](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/helloworld)

The code in `hello.cpp` is as follows:

```cpp
#include <eosio/print.hpp>
#include <eosio/name.hpp>

using namespace eosio;

extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (action == "sayhello"_n.value) {
            print("+++Hello, World");
        }
    }
}
```

To compile:

```bash
cd helloworld
cdt-cpp hello.cpp
```

The test code in `test.py` is as follows:

```python
@chain_test
def test_hello(tester: ChainTester):
    deploy_contract(tester, 'hello')

    r = tester.push_action('hello', 'sayhello', b'', {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

To test:

```bash
ipyeos -m pytest -s -x test.py -k test_hello
```

On Windows platform or MacOSX M1/M2, running the above command will automatically start Docker to run the tests.

Output:

```
[(hello,sayhello)->hello]: CONSOLE OUTPUT BEGIN =====================
+++Hello, World
[(hello,sayhello)->hello]: CONSOLE OUTPUT END   =====================
```