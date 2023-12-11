#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define MAX_CARDS 256

uint32_t n_winning_nums;
uint32_t n_card_nums;

uint32_t num_cards;

typedef struct list {
	uint32_t value;
	struct list *next;
} list_t;

typedef struct card {
	uint32_t card_num;
	uint32_t *winning_nums;
	uint32_t *card_nums;
} card_t;

void free_card(card_t *card)
{
	if (card == NULL) return;
	free(card->winning_nums);
	free(card->card_nums);
	free(card);
}

bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

void consume_whitespace(char **line) { while (**line == ' ') (*line)++; }
void consume_until_digit(char **line) { while (!is_digit(**line)) (*line)++; }

uint32_t parse_int(char **line)
{
	uint32_t result;

	assert(is_digit(**line));

	result = 0;
	while (is_digit(**line))
	{
		result *= 10;
		result += **line - '0';
		(*line)++;
	}

	return result;
}

card_t *parse_card(char *line)
{
	char **p;
	card_t *card;

	card = malloc(sizeof(card_t));
	card->winning_nums = malloc(sizeof(uint32_t) * 128);
	card->card_nums = malloc(sizeof(uint32_t) * 128);

	p = &line;

	consume_until_digit(p);
	card->card_num = parse_int(p);
	consume_until_digit(p);

	while (**p != '|')
	{
		card->winning_nums[n_winning_nums++] = parse_int(p);
		consume_whitespace(p);
	}

	consume_until_digit(p);

	while (**p != '\n' && **p != '\0')
	{
		card->card_nums[n_card_nums++] = parse_int(p);
		consume_whitespace(p);
	}

	return card;
}

uint32_t get_num_matches(card_t *card)
{
	uint32_t num_matches, i, j;

	num_matches = 0;
	for (i = 0; i < n_winning_nums; i++)
	{
		for (j = 0; j < n_card_nums; j++)
		{
			if (card->winning_nums[i] == card->card_nums[j])
			{
				num_matches++;
				break;
			}
		}
	}

	return num_matches;
}

uint32_t score_card(card_t *card)
{
	uint32_t num_matches;

	num_matches = get_num_matches(card);

	if (num_matches == 0) return 0;
	return 1 << (num_matches - 1);
}

uint64_t count_cards(card_t *cards[MAX_CARDS])
{
	card_t *card;
	uint64_t i, j, num_matches, sum;
	uint64_t card_counts[num_cards];

	for (i = 0; i < num_cards; i++)
		card_counts[i] = 1;

	for (i = 0; i < num_cards; i++)
	{
		card = cards[i];

		num_matches = get_num_matches(card);
		for (j = i + 1; j <= i + num_matches; j++)
		{
			if (j >= num_cards) break;
			card_counts[j] += card_counts[i];
		}
	}

	sum = 0;
	for (i = 0; i < num_cards; i++)
		sum += card_counts[i];

	return sum;
}

int main(void)
{
	FILE *f;
	char buffer[1024];
	uint64_t result, i;
	card_t *card;
	card_t *cards[MAX_CARDS];

	f = fopen("input", "r");
	if (f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	result = 0;
	i = 0;
	while (fgets(buffer, sizeof(buffer), f) != NULL)
	{
		n_winning_nums = 0;
		n_card_nums = 0;
		card = parse_card(buffer);
		// Part 1
		// result += score_card(buffer);
		// free_card(card);

		// Part 2
		cards[i] = card;
		i++;
	}

	num_cards = i;

	fclose(f);

	result = count_cards(cards);

	for (i = 0; i < num_cards; i++)
	{
		free_card(cards[i]);
	}

	printf("Result: %llu\n", result);
	return 0;
}
