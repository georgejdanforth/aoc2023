#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define NUM_RED 12
#define NUM_GREEN 13
#define NUM_BLUE 14
#define TOTAL (NUM_RED + NUM_GREEN + NUM_BLUE)

typedef struct draw
{
	uint32_t num_red;
	uint32_t num_green;
	uint32_t num_blue;
	struct draw *next;
} draw_t;

typedef struct game
{
	uint16_t game_id;
	uint16_t num_draws;
	draw_t *draws;
} game_t;

void free_game(game_t *game)
{
	draw_t *draw;
	draw_t *next;

	draw = game->draws;
	while (draw != NULL)
	{
		next = draw->next;
		free(draw);
		draw = next;
	}
	free(game);
}

bool is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

int parse_int(char **line)
{
	int i = 0;
	char num_buf[8];

	assert(is_digit(**line));
	while (is_digit(**line))
	{
		num_buf[i] = **line;
		(*line)++;
		i++;
	}
	num_buf[i] = '\0';

	return atoi(num_buf);
}

void parse_game_id(char **line, game_t *game)
{
	while (!is_digit(**line))
		(*line)++;

	game->game_id = parse_int(line);
}

draw_t *parse_draw(char **line)
{
	draw_t *draw;

	draw = malloc(sizeof(draw_t));

	while (**line != ';' && **line != '\n')
	{
		int num_cubes;

		while (!is_digit(**line)) (*line)++;
		num_cubes = parse_int(line);

		(*line)++;

		switch (**line)
		{
			case 'r':
				draw->num_red = num_cubes;
				break;
			case 'g':
				draw->num_green = num_cubes;
				break;
			case 'b':
				draw->num_blue = num_cubes;
				break;
		}

		while (**line != ',' && **line != ';' && **line != '\n') (*line)++;
	}

	return draw;
}

game_t *parse_game(char *line)
{
	game_t *game;

	game = malloc(sizeof(game_t));

	parse_game_id(&line, game);

	while (*line != '\n')
	{
		draw_t *draw;

		draw = parse_draw(&line);
		draw->next = game->draws;
		game->draws = draw;

		if (*line == ';')
			line++;
	}

	return game;
}

int is_game_possible(char *line)
{
	int game_id;
	game_t *game;
	draw_t *draw;

	game = parse_game(line);

	for (draw = game->draws; draw != NULL; draw = draw->next)
	{
		if (draw->num_red > NUM_RED || draw->num_green > NUM_GREEN || draw->num_blue > NUM_BLUE)
		{
			free_game(game);
			return 0;
		}
	}

	game_id = game->game_id;
	free_game(game);

	return game_id;
}

int max(int a, int b)
{
	return (a > b) ? a : b;
}

int get_power(char *line)
{
	int power, max_red, max_green, max_blue;
	game_t *game;
	draw_t *draw;

	power = 0;
	max_red = 0;
	max_green = 0;
	max_blue = 0;
	game = parse_game(line);

	for (draw = game->draws; draw != NULL; draw = draw->next)
	{
		max_red = max(draw->num_red, max_red);
		max_green = max(draw->num_green, max_green);
		max_blue = max(draw->num_blue, max_blue);
	}

	power = max_red * max_green * max_blue;

	free_game(game);

	return power;
}

int main(void)
{
	FILE *f;
	char buffer[1024];
	int result;

	f = fopen("input", "r");
	if (f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	result = 0;
	while (fgets(buffer, sizeof(buffer), f) != NULL)
	{
		// Part 1:
		// result += is_game_possible(buffer);
	
		// Part 2:
		result += get_power(buffer);
	}

	printf("Result: %d\n", result);

	return 0;
}
