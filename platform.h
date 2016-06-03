#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdlib.h>
#include <string.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define f32 float

u16 swap_bytes_16(u16 val) {
	return __builtin_bswap16(val);
}

u32 swap_bytes_32(u32 val) {
	return __builtin_bswap32(val);
}

u64 swap_bytes_64(u64 val) {
	return __builtin_bswap64(val);
}

char *swap_string_bytes(char s[]) {
	char *ret = (char *)malloc(strlen(s));

	for (u32 i = 0; i < strlen(s); i += 2) {
    	ret[i] = s[i+1];
    	ret[i+1] = s[i];
	}
	return ret;
}

#endif
