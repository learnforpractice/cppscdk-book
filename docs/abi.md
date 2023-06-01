# Detailed Explanation of ABI Types

# Built-in ABI Types

The following are the built-in ABI types, a total of 31

- Basic types: name bytes string
- Numeric types: bool int8 uint8 int16 uint16 int32 uint32 int64 uint64 int128 uint128 varint32 varuint32 float32 float64 float128
- Time-related: time_point time_point_sec block_timestamp_type
- Cryptography-related: checksum160 checksum256 checksum512 public_key signature
- Token-related: symbol symbol_code asset extended_asset

The following are commonly used:

```
name bytes string bool uint64 checksum256
public_key signature symbol asset extended_asset
```

# Correspondence Table between Built-in Data Types in ABI and C++

The following table shows the correspondence between built-in types in ABI and types in C++. The corresponding module, if not built-in, is defined in the relevant header file in the `cdt` package.

Relationship Table:

| ABI Type | C++ Type | Module |
|:--------:|:--------:|:------:|
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
| varint32 | signed_int | core/eosio/varint.hpp |
| varuint32| unsigned_int | core/eosio/varint.hpp |
|  float32 |  float  |  Built-in  |
|  float64 | double  |  Built-in  |
| float128 | long double |  Built-in  |
| time_point | time_point | core/eosio/time.hpp |
| time_point_sec | time_point_sec | core/eosio/time.hpp |
| block_timestamp_type | block_timestamp_type | core/eosio/time.hpp |
|   name   |   name  | core/eosio/name.hpp |
|  bytes   | vector<char> | <vector> |
|  string  | string | <string> |
| checksum160 | checksum160 | core/eosio/crypto.hpp |
| checksum256 | checksum256 | core/eosio/crypto.hpp |
| checksum512 | checksum512 | core/eosio/crypto.hpp |
| public_key | public_key | core/eosio/crypto.hpp |
| signature | signature | core/eosio/crypto.hpp |
|  symbol   |  symbol  | core/eosio/asset.hpp |
| symbol_code| symbol_code | core/eosio/asset.hpp |
|   asset   |  asset  | core/eosio/asset.hpp |
| extended_asset | extended_asset | core/eosio/asset.hpp |

# Special ABI Types

## optional

## variant

## binaryextension
