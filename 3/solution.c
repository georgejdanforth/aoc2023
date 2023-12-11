#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define INIT_SCHEMATIC_LEN 4096

uint32_t row_len;
uint32_t num_rows;

typedef struct gear {
	uint32_t index;
	uint32_t num_parts;
	uint32_t parts[6];
	struct gear *next;
} gear_t;

gear_t **gears;

void free_gear(gear_t *gear)
{
	if (gear == NULL)
		return;

	if (gear->next != NULL)
		free_gear(gear->next);

	free(gear);
}

gear_t *get_gear(uint32_t index)
{
	gear_t *gear;

	assert(gears != NULL);

	gear = gears[index % row_len];
	while (gear != NULL && gear->index != index)
		gear = gear->next;

	if (gear == NULL)
	{
		gear = malloc(sizeof(gear_t));
		gear->index = index;
		gear->num_parts = 0;
		gear->next = gears[index % row_len];
		gears[index % row_len] = gear;
	}

	return gear;
}

bool is_symbol(char c)
{
	return (c < '0' || c > '9') && c != '.' && c != ' ' && c != '\0' && c != '\n';
}

bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

uint32_t get_number_length(char *schematic, uint32_t index)
{
	uint32_t length, max_length;

	max_length = row_len - (index % row_len);
	length = 0;
	while (is_digit(schematic[index + length]) && length < max_length)
		length++;

	return length;
}

int parse_int(char *buf, uint32_t length)
{
	int i;
	char num_buf[length + 1];

	assert(is_digit(*buf));
	for (i = 0; i < length; i++)
		num_buf[i] = buf[i];
	num_buf[i] = '\0';

	int num = atoi(num_buf);

	return num;
}

bool is_touching_symbol(char *schematic, uint32_t index, uint32_t length)
{
	int i, start, end;
	bool check_left, check_right;

	check_left = (index % row_len) != 0;
	check_right = ((index + length) % row_len) != 0;

	if (check_left && is_symbol(schematic[index - 1])) return true;
	if (check_right && is_symbol(schematic[index + length])) return true;

	// Index is not in first row
	if (index > row_len)
	{
		start = index - row_len - check_left;
		end = index - row_len + length + check_right;
		for (i = start; i < end; i++)
		{
			if (is_symbol(schematic[i]))
				return true;
		}
	}

	// Index is not in last row
	if (index < row_len * (num_rows - 1))
	{
		start = index + row_len - check_left;
		end = index + row_len + length + check_right;
		for (i = start; i < end; i++)
		{
			if (is_symbol(schematic[i]))
				return true;
		}
	}

	return false;
}

uint32_t get_part_number_sum(char *schematic)
{
	uint32_t i, sum;

	i = 0;
	sum = 0;

	while (i < row_len * num_rows)
	{
		if (is_digit(schematic[i]))
		{
			uint32_t num_length;

			num_length = get_number_length(schematic, i);
			if (is_touching_symbol(schematic, i, num_length))
			{
				sum += parse_int(&schematic[i], num_length);
			}

			i += num_length;
		}
		else
		{
			i++;
		}
	}

	return sum;
}

void update_gear(uint32_t index, uint32_t num)
{
	gear_t *gear;

	gear = get_gear(index);
	gear->parts[gear->num_parts++] = num;
}

uint32_t update_gears(char *schematic, uint32_t index)
{
	uint32_t num_length, num, start, end, i;
	bool check_left, check_right;

	assert(is_digit(schematic[index]));

	num_length = get_number_length(schematic, index);
	num = parse_int(&schematic[index], num_length);

	check_left = (index % row_len) != 0;
	check_right = ((index + num_length) % row_len) != 0;

	// Above
	if (index > row_len)
	{
		start = index - row_len - check_left;
		end = index - row_len + num_length + check_right;

		for (i = start; i < end; i++)
		{
			if (schematic[i] == '*')
				update_gear(i, num);
		}
	}

	// Sides
	if (check_left && schematic[index - 1] == '*') update_gear(index - 1, num);
	if (check_right && schematic[index + num_length] == '*') update_gear(index + num_length, num);

	// Below
	if (index < row_len * (num_rows - 1))
	{
		start = index + row_len - check_left;
		end = index + row_len + num_length + check_right;

		for (i = start; i < end; i++)
		{
			if (schematic[i] == '*')
				update_gear(i, num);
		}
	}

	return num_length;
}

uint32_t get_gear_ratio_sum(char *schematic)
{
	uint32_t i, sum;
	gear_t *gear;

	gears = calloc(row_len, sizeof(gear_t *));

	i = 0;
	while (i < row_len * num_rows)
	{
		if (is_digit(schematic[i]))
			i += update_gears(schematic, i);
		else
			i++;
	}

	sum = 0;
	for (i = 0; i < row_len; i++)
	{
		if (gears[i] == NULL) continue;
		for (gear = gears[i]; gear != NULL; gear = gear->next)
		{
			if (gear->num_parts != 2) continue;
			sum += gear->parts[0] * gear->parts[1];
		}
	}

	return sum;
}

int main(void)
{
	FILE *f;
	uint32_t schematic_len;
	uint32_t result;
	char *schematic, *schematic_offset;
	char buffer[1024];

	schematic_len = INIT_SCHEMATIC_LEN;
	schematic = malloc(schematic_len);
	schematic_offset = schematic;

	f = fopen("input", "r");
	if (f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	num_rows = 0;
	while (fgets(buffer, sizeof(buffer), f) != NULL)
	{
		// Only compute row length on first iteration
		if (num_rows == 0)
			// Subtract 1 to remove the newline character
			row_len = strlen(buffer) - 1;

		// Resize schematic if necessary
		if ((num_rows + 1) * row_len >= schematic_len)
		{
			schematic_len *= 2;
			schematic = realloc(schematic, schematic_len);
			schematic_offset = schematic + num_rows * row_len;
		}

		// Copy row into schematic
		memcpy(schematic_offset, buffer, row_len);
		schematic_offset += row_len;

		num_rows++;
	}

	schematic[num_rows * row_len] = '\0';

	fclose(f);

	// Part 1
	// result = get_part_number_sum(schematic);

	// Part 2
	result = get_gear_ratio_sum(schematic);
	printf("Result: %u\n", result);

	return 0;
}
