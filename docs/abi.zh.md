---
comments: true
---

# ABI类型详解

# 内置的ABI类型

以下是内置的ABI类型，一共31个

- 基本类型：name bytes string
- 数值类型： bool int8 uint8 int16 uint16 int32 uint32 int64 uint64 int128 uint128 varint32 varuint32 float32 float64 float128
- 时间相关：time_point time_point_sec block_timestamp_type
- 密码函数相关：checksum160 checksum256 checksum512 public_key signature
- Token相关：symbol symbol_code asset extended_asset

比较常用的有下面这些：

```
name bytes string bool uint64 checksum256
public_key signature symbol asset extended_asset
```
                                                                                                    
# ABI中的内置数据类型和 C++ 中的数据类型的对应关系表

下面的这张表显示了ABI中的内置类型和 C++ 中的类型的对应关系.其中的所属模块，如果是非内置，则是在`cdt`这个package中相关的头文件中定义的。


关系表：

|         ABI 类型     |   C++ 类型       |      所属模块    |
|:--------------------:|:------------------:|:------------------:|
|         bool         |        bool        |   内置    |
|         int8         |         int8_t         |   内置    |
|         uint8        |         uint8_t         |   内置    |
|         int16        |         int16_t        |   内置    |
|         int32        |         int32_t        |   内置    |
|        uint32        |         uint32_t        |   内置    |
|         int64        |         int64_t        |   内置    |
|        uint64        |         uint64_t        |   内置    |
|        int128        |        int128_t        |   内置    |
|        uint128       |        uint128_t        |   内置    |
|       varint32       |   signed_int            |   core/eosio/varint.hpp       |
|       varuint32      |   unsigned_int     |   core/eosio/varint.hpp |
|        float32       |     float        |  内置     |
|        float64       |       double        |  内置     |
|       float128       |      long double      |  内置  |
|      time_point      |      time_point     |  core/eosio/time.hpp  |
|    time_point_sec    |    time_point_sec    |  core/eosio/time.hpp  |
| block_timestamp_type | block_timestamp_type |  core/eosio/time.hpp  |
|         name         |        name        |  core/eosio/name.hpp  |
|         bytes        |    vector<char>   |  <vector>  |
|        string        |        string      |  <string>  |
|      checksum160     |     checksum160    |  core/eosio/crypto.hpp  |
|      checksum256     |   checksum256 |  core/eosio/crypto.hpp  |
|      checksum512     |     checksum512    |  core/eosio/crypto.hpp  |
|      public_key      |      public_key     |  core/eosio/crypto.hpp  |
|       signature      |      signature     |  core/eosio/crypto.hpp  |
|        symbol        |       symbol       | core/eosio/asset.hpp   |
|      symbol_code     |     symbol_code     | core/eosio/asset.hpp   |
|         asset        |        asset       | core/eosio/asset.hpp   |
|    extended_asset    |    extended_asset   | core/eosio/asset.hpp   |
                                                                                                    
# 特别的ABI类型

## optional

## variant

## binaryextension
