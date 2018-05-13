# Introduction
MPK is a file with simple structure. Its been used by Steins;Gate and Steins;Gate 0 as far as I know

# Structure

## Header
##### Total 8 bytes
The header of the MPK is

Name | Size | Values(if any) |
--- | --- | --- 
**IDENT** | 4 bytes | "MPK"\0
Null Bytes | 2 bytes | \0\0
Two | 2 bytes | 0x2

## File count
##### Total 64 bytes
Name | Size |
--- | --- 
File count | 8 bytes
Null Bytes | 7 * 8 bytes

## File information
##### Total 256 bytes(each)
Repeat this `File count` times

Name | Size |
--- | --- 
Index | 4 bytes
Position | 8 bytes
Size | 8 bytes
Size(again) | 8 bytes
Null terminated file name | 228 bytes

## Raw data
##### Total n bytes(each)
Repeat this `File count` times

Name | Size |
--- | --- 
Raw data | `Size` bytes from respective info structure
