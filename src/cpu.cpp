#include <iostream>
#include <cstdint>

#include "cpu.h"

void nop()
{

}

void undefined()
{
	
}

extern const instruction instructions[256] = {
	{ "NOP", 0, nop }, // 0x00
	{ "", 0, undefined }, // 0x1
	{ "", 0, undefined }, // 0x2
	{ "", 0, undefined }, // 0x3
	{ "", 0, undefined }, // 0x4
	{ "", 0, undefined }, // 0x5
	{ "", 0, undefined }, // 0x6
	{ "", 0, undefined }, // 0x7
	{ "LD (a16),SP", 0, undefined }, // 0x8
	{ "", 0, undefined }, // 0x9
	{ "", 0, undefined }, // 0xa
	{ "", 0, undefined }, // 0xb
	{ "", 0, undefined }, // 0xc
	{ "", 0, undefined }, // 0xd
	{ "", 0, undefined }, // 0xe
	{ "", 0, undefined }, // 0xf
	{ "", 0, undefined }, // 0x10
	{ "", 0, undefined }, // 0x11
	{ "", 0, undefined }, // 0x12
	{ "", 0, undefined }, // 0x13
	{ "", 0, undefined }, // 0x14
	{ "", 0, undefined }, // 0x15
	{ "", 0, undefined }, // 0x16
	{ "", 0, undefined }, // 0x17
	{ "", 0, undefined }, // 0x18
	{ "", 0, undefined }, // 0x19
	{ "", 0, undefined }, // 0x1a
	{ "", 0, undefined }, // 0x1b
	{ "", 0, undefined }, // 0x1c
	{ "", 0, undefined }, // 0x1d
	{ "", 0, undefined }, // 0x1e
	{ "", 0, undefined }, // 0x1f
	{ "", 0, undefined }, // 0x20
	{ "", 0, undefined }, // 0x21
	{ "", 0, undefined }, // 0x22
	{ "", 0, undefined }, // 0x23
	{ "", 0, undefined }, // 0x24
	{ "", 0, undefined }, // 0x25
	{ "", 0, undefined }, // 0x26
	{ "", 0, undefined }, // 0x27
	{ "", 0, undefined }, // 0x28
	{ "", 0, undefined }, // 0x29
	{ "", 0, undefined }, // 0x2a
	{ "", 0, undefined }, // 0x2b
	{ "", 0, undefined }, // 0x2c
	{ "", 0, undefined }, // 0x2d
	{ "", 0, undefined }, // 0x2e
	{ "", 0, undefined }, // 0x2f
	{ "", 0, undefined }, // 0x30
	{ "", 0, undefined }, // 0x31
	{ "", 0, undefined }, // 0x32
	{ "", 0, undefined }, // 0x33
	{ "", 0, undefined }, // 0x34
	{ "", 0, undefined }, // 0x35
	{ "", 0, undefined }, // 0x36
	{ "", 0, undefined }, // 0x37
	{ "", 0, undefined }, // 0x38
	{ "", 0, undefined }, // 0x39
	{ "", 0, undefined }, // 0x3a
	{ "", 0, undefined }, // 0x3b
	{ "", 0, undefined }, // 0x3c
	{ "", 0, undefined }, // 0x3d
	{ "", 0, undefined }, // 0x3e
	{ "", 0, undefined }, // 0x3f
	{ "", 0, undefined }, // 0x40
	{ "", 0, undefined }, // 0x41
	{ "", 0, undefined }, // 0x42
	{ "", 0, undefined }, // 0x43
	{ "", 0, undefined }, // 0x44
	{ "", 0, undefined }, // 0x45
	{ "", 0, undefined }, // 0x46
	{ "", 0, undefined }, // 0x47
	{ "", 0, undefined }, // 0x48
	{ "", 0, undefined }, // 0x49
	{ "", 0, undefined }, // 0x4a
	{ "", 0, undefined }, // 0x4b
	{ "", 0, undefined }, // 0x4c
	{ "", 0, undefined }, // 0x4d
	{ "", 0, undefined }, // 0x4e
	{ "", 0, undefined }, // 0x4f
	{ "", 0, undefined }, // 0x50
	{ "", 0, undefined }, // 0x51
	{ "", 0, undefined }, // 0x52
	{ "", 0, undefined }, // 0x53
	{ "", 0, undefined }, // 0x54
	{ "", 0, undefined }, // 0x55
	{ "", 0, undefined }, // 0x56
	{ "", 0, undefined }, // 0x57
	{ "", 0, undefined }, // 0x58
	{ "", 0, undefined }, // 0x59
	{ "", 0, undefined }, // 0x5a
	{ "", 0, undefined }, // 0x5b
	{ "", 0, undefined }, // 0x5c
	{ "", 0, undefined }, // 0x5d
	{ "", 0, undefined }, // 0x5e
	{ "", 0, undefined }, // 0x5f
	{ "", 0, undefined }, // 0x60
	{ "", 0, undefined }, // 0x61
	{ "", 0, undefined }, // 0x62
	{ "", 0, undefined }, // 0x63
	{ "", 0, undefined }, // 0x64
	{ "", 0, undefined }, // 0x65
	{ "", 0, undefined }, // 0x66
	{ "", 0, undefined }, // 0x67
	{ "", 0, undefined }, // 0x68
	{ "", 0, undefined }, // 0x69
	{ "", 0, undefined }, // 0x6a
	{ "", 0, undefined }, // 0x6b
	{ "", 0, undefined }, // 0x6c
	{ "", 0, undefined }, // 0x6d
	{ "", 0, undefined }, // 0x6e
	{ "", 0, undefined }, // 0x6f
	{ "", 0, undefined }, // 0x70
	{ "", 0, undefined }, // 0x71
	{ "", 0, undefined }, // 0x72
	{ "", 0, undefined }, // 0x73
	{ "", 0, undefined }, // 0x74
	{ "", 0, undefined }, // 0x75
	{ "", 0, undefined }, // 0x76
	{ "", 0, undefined }, // 0x77
	{ "", 0, undefined }, // 0x78
	{ "", 0, undefined }, // 0x79
	{ "", 0, undefined }, // 0x7a
	{ "", 0, undefined }, // 0x7b
	{ "", 0, undefined }, // 0x7c
	{ "", 0, undefined }, // 0x7d
	{ "", 0, undefined }, // 0x7e
	{ "", 0, undefined }, // 0x7f
	{ "", 0, undefined }, // 0x80
	{ "", 0, undefined }, // 0x81
	{ "", 0, undefined }, // 0x82
	{ "", 0, undefined }, // 0x83
	{ "", 0, undefined }, // 0x84
	{ "", 0, undefined }, // 0x85
	{ "", 0, undefined }, // 0x86
	{ "", 0, undefined }, // 0x87
	{ "", 0, undefined }, // 0x88
	{ "", 0, undefined }, // 0x89
	{ "", 0, undefined }, // 0x8a
	{ "", 0, undefined }, // 0x8b
	{ "", 0, undefined }, // 0x8c
	{ "", 0, undefined }, // 0x8d
	{ "", 0, undefined }, // 0x8e
	{ "", 0, undefined }, // 0x8f
	{ "", 0, undefined }, // 0x90
	{ "", 0, undefined }, // 0x91
	{ "", 0, undefined }, // 0x92
	{ "", 0, undefined }, // 0x93
	{ "", 0, undefined }, // 0x94
	{ "", 0, undefined }, // 0x95
	{ "", 0, undefined }, // 0x96
	{ "", 0, undefined }, // 0x97
	{ "", 0, undefined }, // 0x98
	{ "", 0, undefined }, // 0x99
	{ "", 0, undefined }, // 0x9a
	{ "", 0, undefined }, // 0x9b
	{ "", 0, undefined }, // 0x9c
	{ "", 0, undefined }, // 0x9d
	{ "", 0, undefined }, // 0x9e
	{ "", 0, undefined }, // 0x9f
	{ "", 0, undefined }, // 0xa0
	{ "", 0, undefined }, // 0xa1
	{ "", 0, undefined }, // 0xa2
	{ "", 0, undefined }, // 0xa3
	{ "", 0, undefined }, // 0xa4
	{ "", 0, undefined }, // 0xa5
	{ "", 0, undefined }, // 0xa6
	{ "", 0, undefined }, // 0xa7
	{ "", 0, undefined }, // 0xa8
	{ "", 0, undefined }, // 0xa9
	{ "", 0, undefined }, // 0xaa
	{ "", 0, undefined }, // 0xab
	{ "", 0, undefined }, // 0xac
	{ "", 0, undefined }, // 0xad
	{ "", 0, undefined }, // 0xae
	{ "", 0, undefined }, // 0xaf
	{ "", 0, undefined }, // 0xb0
	{ "", 0, undefined }, // 0xb1
	{ "", 0, undefined }, // 0xb2
	{ "", 0, undefined }, // 0xb3
	{ "", 0, undefined }, // 0xb4
	{ "", 0, undefined }, // 0xb5
	{ "", 0, undefined }, // 0xb6
	{ "", 0, undefined }, // 0xb7
	{ "", 0, undefined }, // 0xb8
	{ "", 0, undefined }, // 0xb9
	{ "", 0, undefined }, // 0xba
	{ "", 0, undefined }, // 0xbb
	{ "", 0, undefined }, // 0xbc
	{ "", 0, undefined }, // 0xbd
	{ "", 0, undefined }, // 0xbe
	{ "", 0, undefined }, // 0xbf
	{ "", 0, undefined }, // 0xc0
	{ "", 0, undefined }, // 0xc1
	{ "", 0, undefined }, // 0xc2
	{ "", 0, undefined }, // 0xc3
	{ "", 0, undefined }, // 0xc4
	{ "", 0, undefined }, // 0xc5
	{ "", 0, undefined }, // 0xc6
	{ "", 0, undefined }, // 0xc7
	{ "", 0, undefined }, // 0xc8
	{ "", 0, undefined }, // 0xc9
	{ "", 0, undefined }, // 0xca
	{ "", 0, undefined }, // 0xcb
	{ "", 0, undefined }, // 0xcc
	{ "", 0, undefined }, // 0xcd
	{ "", 0, undefined }, // 0xce
	{ "", 0, undefined }, // 0xcf
	{ "", 0, undefined }, // 0xd0
	{ "", 0, undefined }, // 0xd1
	{ "", 0, undefined }, // 0xd2
	{ "", 0, undefined }, // 0xd3
	{ "", 0, undefined }, // 0xd4
	{ "", 0, undefined }, // 0xd5
	{ "", 0, undefined }, // 0xd6
	{ "", 0, undefined }, // 0xd7
	{ "", 0, undefined }, // 0xd8
	{ "", 0, undefined }, // 0xd9
	{ "", 0, undefined }, // 0xda
	{ "", 0, undefined }, // 0xdb
	{ "", 0, undefined }, // 0xdc
	{ "", 0, undefined }, // 0xdd
	{ "", 0, undefined }, // 0xde
	{ "", 0, undefined }, // 0xdf
	{ "", 0, undefined }, // 0xe0
	{ "", 0, undefined }, // 0xe1
	{ "", 0, undefined }, // 0xe2
	{ "", 0, undefined }, // 0xe3
	{ "", 0, undefined }, // 0xe4
	{ "", 0, undefined }, // 0xe5
	{ "", 0, undefined }, // 0xe6
	{ "", 0, undefined }, // 0xe7
	{ "", 0, undefined }, // 0xe8
	{ "", 0, undefined }, // 0xe9
	{ "", 0, undefined }, // 0xea
	{ "", 0, undefined }, // 0xeb
	{ "", 0, undefined }, // 0xec
	{ "", 0, undefined }, // 0xed
	{ "", 0, undefined }, // 0xee
	{ "", 0, undefined }, // 0xef
	{ "", 0, undefined }, // 0xf0
	{ "", 0, undefined }, // 0xf1
	{ "", 0, undefined }, // 0xf2
	{ "", 0, undefined }, // 0xf3
	{ "", 0, undefined }, // 0xf4
	{ "", 0, undefined }, // 0xf5
	{ "", 0, undefined }, // 0xf6
	{ "", 0, undefined }, // 0xf7
	{ "", 0, undefined }, // 0xf8
	{ "", 0, undefined }, // 0xf9
	{ "", 0, undefined }, // 0xfa
	{ "", 0, undefined }, // 0xfb
	{ "", 0, undefined }, // 0xfc
	{ "", 0, undefined }, // 0xfd
	{ "", 0, undefined }, // 0xfe
	{ "", 0, undefined }, // 0xff
};