#include "aoc.h"

#define BUF_SIZE 1024

u32 num_races = 0;
u64 times[128] = {0};
u64 distances[128] = {0};

void parse_line_part_1(char *line, u64 *values)
{
	u64 num_values, value;
	char **p;

	p = &line;
	num_values = 0;

	while (**p != '\0' && **p != '\n')
	{
		consume_until_digit(p);
		value = parse_u64(p);
		values[num_values] = value;
		num_values++;
	}

	if (num_races != 0)
		assert(num_races == num_values);
	else
		num_races = num_values;
}

void parse_line_part_2(char *line, u64 *values)
{
	u64 num_digits;
	char *buf;
	char **p;

	buf = malloc(128 * sizeof(char));
	p = &line;
	num_digits = 0;
	while (**p != '\0' && **p != '\n')
	{
		consume_until_digit(p);
		while (is_digit(**p))
		{
			buf[num_digits++] = **p;
			(*p)++;
		}
	}
	buf[num_digits] = '\0';

	num_races = 1;
	values[0] = parse_u64(&buf);
}

u64 get_num_ways_to_win_part_1(u64 time, u64 best_distance)
{
	u32 i, distance;
	u64 num_ways_to_win;

	num_ways_to_win = 0;

	for (i = 1; i < time; i++)
	{
		distance = i * (time - i);
		if (distance > best_distance)
			num_ways_to_win++;
	}

	return num_ways_to_win;
}

u64 get_num_ways_to_win_part_2(u64 time, u64 best_distance)
{
	u64 i, j, distance;
	u64 num_ways_to_win;

	num_ways_to_win = 0;

	i = 0;
	while (true)
	{
		distance = i * (time - i);
		if (distance > best_distance)
			break;
		i++;
	}

	j = time;
	while (true)
	{
		distance = j * (time - j);
		if (distance > best_distance)
			break;
		j--;
	}

	num_ways_to_win = j - i + 1;

	return num_ways_to_win;
}

int main(void)
{
	FILE *f;
	char buf[BUF_SIZE];
	u32 i;
	u64 result;

	f = fopen("6/input", "r");
	if (f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	// parse lines
	fgets(buf, sizeof(buf), f);
	//parse_line_part_1(buf, times);
	parse_line_part_2(buf, times);
	fgets(buf, sizeof(buf), f);
	//parse_line_part_1(buf, distances);
	parse_line_part_2(buf, distances);

	fclose(f);

	result = 1;
	for (i = 0; i < num_races; i++)
		//result *= get_num_ways_to_win_part_1(times[i], distances[i]);
		result *= get_num_ways_to_win_part_2(times[i], distances[i]);

	printf("result: %llu\n", result);

	return EXIT_SUCCESS;
}
