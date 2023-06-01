---
comments: true
---

# 设置开发环境

## 安装 cdt

```
python3 -m pip install -U eoscdt
```

如果你的平台是 Windows 或 MacOSX M1/M2，你还需要下载一个包含eoscdt工具的镜像

```bash
docker pull ghcr.io/uuosio/scdt:latest
```

在 macOS 上安装和运行 Docker 的推荐软件是 [OrbStack](https://orbstack.dev/download)。对于其他平台，可以使用 [Docker Desktop](https://www.docker.com/products/docker-desktop)。

## 安装EOS测试框架

安装 ipyeos：

```bash
python3 -m pip install -U ipyeos
```

如果你的平台是 Windows 或 MacOSX M1/M2，你还需要下载一个包含ipyeos工具的镜像

```bash
docker pull ghcr.io/uuosio/ipyeos:latest
```

在 macOS 上安装和运行 Docker 的推荐软件是 [OrbStack](https://orbstack.dev/download)。对于其他平台，可以使用 [Docker Desktop](https://www.docker.com/products/docker-desktop)。

## 安装 EOS 的 Python 工具包

```bash
python3 -m pip install -U pyeoskit
```

pyeoskit 用于部署合约到主网或者测试网。

## 检查环境

下载测试代码：[helloworld](https://github.com/learnforpractice/cppscdk-book/tree/master/examples/helloworld)

其中的 hello.cpp 的代码如下：

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

编译：

```bash
cd helloworld
cdt-cpp hello.cpp
```

test.py中的测试代码如下：

```python
@chain_test
def test_hello(tester: ChainTester):
    deploy_contract(tester, 'hello')

    r = tester.push_action('hello', 'sayhello', b'', {'hello': 'active'})
    logger.info('++++++elapsed: %s', r['elapsed'])
    tester.produce_block()
```

测试:

```bash
ipyeos -m pytest -s -x test.py -k test_hello
```

在Windows平台或者或 MacOSX M1/M2，运行上面的命令将自动启动docker来运行测试。

输出：

```
[(hello,sayhello)->hello]: CONSOLE OUTPUT BEGIN =====================
+++Hello, World
[(hello,sayhello)->hello]: CONSOLE OUTPUT END   =====================
```
