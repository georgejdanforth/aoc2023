#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define BUFSIZE 1024

uint32_t num_seeds;
uint64_t seeds[32] = {0};

typedef struct map {
	uint64_t dest_range_start;
	uint64_t source_range_start;
	uint64_t length;
} map_t;

map_t seed_to_soil_maps[64] = {0};
uint32_t num_seed_to_soil_maps = 0;
map_t soil_to_fertilizer_maps[64] = {0};
uint32_t num_soil_to_fertilizer_maps = 0;
map_t fertilizer_to_water_maps[64] = {0};
uint32_t num_fertilizer_to_water_maps = 0;
map_t water_to_light_maps[64] = {0};
uint32_t num_water_to_light_maps = 0;
map_t light_to_temperature_maps[64] = {0};
uint32_t num_light_to_temperature_maps = 0;
map_t temperature_to_humidity_maps[64] = {0};
uint32_t num_temperature_to_humidity_maps = 0;
map_t humidity_to_location_maps[64] = {0};
uint32_t num_humidity_to_location_maps = 0;

uint64_t min_uint64(uint64_t a, uint64_t b) { return a < b ? a : b; }

bool is_digit(char c) { return c >= '0' && c <= '9'; }

void consume_until_digit(char **line) { while (**line != '\0' && !is_digit(**line)) (*line)++; }
void consume_whitespace(char **line) { while (**line != '\0' && **line == ' ') (*line)++; }
uint64_t parse_int(char **line)
{
	uint64_t result;

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

void parse_seeds(char *line)
{
	char **p;

	p = &line;

	consume_until_digit(p);
	while (**p != '\0')
	{
		seeds[num_seeds++] = parse_int(p);
		consume_until_digit(p);
	}
}

map_t parse_map(char *line)
{
	char **p;
	map_t map;

	p = &line;

	consume_until_digit(p);
	map.dest_range_start = parse_int(p);
	consume_until_digit(p);
	map.source_range_start = parse_int(p);
	consume_until_digit(p);
	map.length = parse_int(p);

	return map;
}

void *read_line(FILE *f, char *buffer) { return fgets(buffer, BUFSIZE, f); }

void parse_maps(FILE *f, map_t *maps, uint32_t *counter)
{
	char buffer[BUFSIZE];
	map_t map;

	while (1)
	{
		if (read_line(f, buffer) == NULL) break;
		if (*buffer == '\n') break;

		map = parse_map(buffer);
		memcpy(&maps[*counter], &map, sizeof(map_t));
		(*counter)++;
	}
}

void parse_file(FILE *f)
{
	char buffer[BUFSIZE];

	num_seeds = 0;
	read_line(f, buffer);
	parse_seeds(buffer);

	read_line(f, buffer);
	assert(*buffer == '\n');

	read_line(f, buffer);
	assert(strcmp(buffer, "seed-to-soil map:\n") == 0);
	parse_maps(f, seed_to_soil_maps, &num_seed_to_soil_maps);

	read_line(f, buffer);
	assert(strcmp(buffer, "soil-to-fertilizer map:\n") == 0);
	parse_maps(f, soil_to_fertilizer_maps, &num_soil_to_fertilizer_maps);

	read_line(f, buffer);
	assert(strcmp(buffer, "fertilizer-to-water map:\n") == 0);
	parse_maps(f, fertilizer_to_water_maps, &num_fertilizer_to_water_maps);

	read_line(f, buffer);
	assert(strcmp(buffer, "water-to-light map:\n") == 0);
	parse_maps(f, water_to_light_maps, &num_water_to_light_maps);

	read_line(f, buffer);
	assert(strcmp(buffer, "light-to-temperature map:\n") == 0);
	parse_maps(f, light_to_temperature_maps, &num_light_to_temperature_maps);

	read_line(f, buffer);
	assert(strcmp(buffer, "temperature-to-humidity map:\n") == 0);
	parse_maps(f, temperature_to_humidity_maps, &num_temperature_to_humidity_maps);

	read_line(f, buffer);
	assert(strcmp(buffer, "humidity-to-location map:\n") == 0);
	parse_maps(f, humidity_to_location_maps, &num_humidity_to_location_maps);
}

uint64_t apply_mapping(uint64_t val, map_t *maps, uint32_t num_maps)
{
	uint32_t i;
	map_t map;

	for (i = 0; i < num_maps; i++)
	{
		map = maps[i];
		if (val >= map.source_range_start && val < map.source_range_start + map.length)
			return map.dest_range_start + (val - map.source_range_start);
	}

	return val;
}

void apply_mapping_to_vals(uint64_t *vals, uint32_t num_vals, map_t *maps, uint32_t num_maps)
{
	uint32_t i;

	for (i = 0; i < num_vals; i++)
		vals[i] = apply_mapping(vals[i], maps, num_maps);
}

uint64_t get_min_location(void)
{
	int i;
	uint64_t min_location;
	uint64_t vals[num_seeds];

	for (i = 0; i < num_seeds; i++)
		vals[i] = seeds[i];

	apply_mapping_to_vals(vals, num_seeds, seed_to_soil_maps, num_seed_to_soil_maps);
	apply_mapping_to_vals(vals, num_seeds, soil_to_fertilizer_maps, num_soil_to_fertilizer_maps);
	apply_mapping_to_vals(vals, num_seeds, fertilizer_to_water_maps, num_fertilizer_to_water_maps);
	apply_mapping_to_vals(vals, num_seeds, water_to_light_maps, num_water_to_light_maps);
	apply_mapping_to_vals(vals, num_seeds, light_to_temperature_maps, num_light_to_temperature_maps);
	apply_mapping_to_vals(vals, num_seeds, temperature_to_humidity_maps, num_temperature_to_humidity_maps);
	apply_mapping_to_vals(vals, num_seeds, humidity_to_location_maps, num_humidity_to_location_maps);

	min_location = vals[0];
	for (i = 1; i < num_seeds; i++)
		min_location = min_uint64(min_location, vals[i]);

	return min_location;
}

int main(void)
{
	FILE *f;
	uint64_t result;

	f = fopen("input", "r");
	if (f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	parse_file(f);
	fclose(f);

	result = get_min_location();
	printf("Result: %llu\n", result);

	return 0;
}
