#ifndef __AOC_H__
#define __AOC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* Basic type aliases */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* Parsing utilities */
bool is_digit(char c) { return c >= '0' && c <= '9'; }
void consume_until_digit(char **line) { while (**line != '\0' && !is_digit(**line)) (*line)++; }
void consume_whitespace(char **line) { while (**line != '\0' && **line == ' ') (*line)++; }

u32 parse_u32(char **line)
{
	u32 result;

	assert(is_digit(**line));

	result = 0;
	while (**line != '\0' && is_digit(**line))
	{
		result *= 10;
		result += **line - '0';
		(*line)++;
	}

	return result;
}

u64 parse_u64(char **line)
{
	u64 result;

	assert(is_digit(**line));

	result = 0;
	while (**line != '\0' && is_digit(**line))
	{
		result *= 10;
		result += **line - '0';
		(*line)++;
	}

	return result;
}

#endif // __AOC_H__
