#include "aoc.h"

#define BUFSIZE 1024

u32 num_hands;

typedef enum hand_type {
	HIGH_CARD = 0,
	ONE_PAIR,
	TWO_PAIR,
	THREE_OF_A_KIND,
	FULL_HOUSE,
	FOUR_OF_A_KIND,
	FIVE_OF_A_KIND
} hand_type_t;

typedef struct hand {
	hand_type_t type;
	u8 cards[5];
	u32 values[5];
	u32 bid;
} hand_t;

typedef struct list {
	hand_t *hand;
	struct list *next;
} list_t;

list_t *list_add(list_t *list, hand_t *hand)
{
	list_t *new;

	new = malloc(sizeof(list_t));
	new->hand = hand;
	new->next = list;

	return new;
}

void sort(list_t *list)
{
	u32 i;
	list_t *p, *q;
	hand_t *tmp;

	for (p = list; p != NULL; p = p->next)
	{
		for (q = p->next; q != NULL; q = q->next)
		{
			for (i = 0; i < 5; i++)
			{
				if (p->hand->values[i] < q->hand->values[i])
					break;
				else if (p->hand->values[i] > q->hand->values[i])
				{
					tmp = p->hand;
					p->hand = q->hand;
					q->hand = tmp;
					break;
				}
			}
		}
	}
}

hand_type_t get_hand_type(const u32 values[5])
{
	u32 i, j;
	u32 counts[13] = {0};

	for (i = 0; i < 5; i++)
		counts[values[i]]++;

	for (i = 0; i < 13; i++)
	{
		if (counts[i] == 5)
			return FIVE_OF_A_KIND;
		if (counts[i] == 4)
			return FOUR_OF_A_KIND;
		if (counts[i] == 3)
		{
			for (j = 0; j < 13; j++)
			{
				if (j == i) continue;
				else if (counts[j] == 2) return FULL_HOUSE;
			}
			return THREE_OF_A_KIND;
		}
		if (counts[i] == 2)
		{
			for (j = 0; j < 13; j++)
			{
				if (j == i) continue;
				else if (counts[j] == 3) return FULL_HOUSE;
				else if (counts[j] == 2) return TWO_PAIR;
			}
			return ONE_PAIR;
		}
	}
	return HIGH_CARD;
}

hand_t *parse_hand(char *line)
{
	u32 i;
	hand_t *hand;
	char **p;

	hand = malloc(sizeof(hand_t));

	for (i = 0; i < 5; i++)
	{
		hand->cards[i] = line[i];
		switch (line[i])
		{
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				hand->values[i] = line[i] - '2';
				break;
			case 'T':
				hand->values[i] = 8;
				break;
			case 'J':
				hand->values[i] = 9;
				break;
			case 'Q':
				hand->values[i] = 10;
				break;
			case 'K':
				hand->values[i] = 11;
				break;
			case 'A':
				hand->values[i] = 12;
				break;
		}
	}

	line += 6;
	p = &line;

	hand->bid = parse_u32(p);
	hand->type = get_hand_type(hand->values);

	return hand;
}

u64 get_total_winnings(list_t *hands)
{
	u64 total_winnings;
	u32 i, rank;
	list_t *p;
	list_t *buckets[7] = {NULL};

	for (p = hands; p != NULL; p = p->next)
		buckets[p->hand->type] = list_add(buckets[p->hand->type], p->hand);

	for (i = 0; i < 7; i++)
		sort(buckets[i]);

	total_winnings = 0;
	rank = 1;
	for (i = 0; i < 7; i++)
	{
		for (p = buckets[i]; p != NULL; p = p->next)
		{
			u64 winnings = p->hand->bid * rank;
			printf("rank: %u, type: %u, hand: %s, bid: %u, winnings: %llu\n", rank, p->hand->type, p->hand->cards, p->hand->bid, winnings);
			total_winnings += p->hand->bid * rank;
			rank++;
		}
	}
	return total_winnings;
}

int main(void)
{
	FILE *f;
	char buf[BUFSIZE];
	list_t *hands;
	u64 total_winnings;

	hands = NULL;
	num_hands = 0;

	f = fopen("7/input", "r");
	if (f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	while (fgets(buf, sizeof(buf), f) != NULL)
	{
		hands = list_add(hands, parse_hand(buf));
		num_hands++;
	}

	fclose(f);

	total_winnings = get_total_winnings(hands);

	printf("total winnings: %llu\n", total_winnings);

	return EXIT_SUCCESS;
}
