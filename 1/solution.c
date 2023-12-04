#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUF_SIZE 2048

char *one = "one";
char *two = "two";
char *three = "three";
char *four = "four";
char *five = "five";
char *six = "six";
char *seven = "seven";
char *eight = "eight";
char *nine = "nine";

bool is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

int get_calibration_value_part1(char *line)
{
	int i;
	int value = 0;

	for (i = 0; line[i] != '\0'; i++)
	{
		if (is_digit(line[i]))
		{
			value += (line[i] - '0') * 10;
			break;
		}
	}

	while (line[i] != '\0')
		i++;

	i--;

	while (i >= 0)
	{
		if (is_digit(line[i]))
		{
			value += line[i] - '0';
			break;
		}
		i--;
	}

	return value;
}

int get_calibration_value_part2(char *line)
{
	int li, bi;
	char new_buffer[BUF_SIZE];

	li = 0;
	bi = 0;

	while (line[li] != '\0')
	{
		if (is_digit(line[li]))
		{
			new_buffer[bi] = line[li];
			bi++;
			li++;
			continue;
		}

		switch (line[li])
		{
			case 'o':
				if (strncmp(one, line + li, 3) == 0)
				{
					new_buffer[bi] = '1';
					bi++;
					li += 1;
				}
				else
					li++;
				break;
			case 't':
				if (strncmp(two, line + li, 3) == 0)
				{
					new_buffer[bi] = '2';
					bi++;
					li += 1;
				}
				else if (strncmp(three, line + li, 5) == 0)
				{
					new_buffer[bi] = '3';
					bi++;
					li += 1;
				}
				else
					li++;
				break;
			case 'f':
				if (strncmp(four, line + li, 4) == 0)
				{
					new_buffer[bi] = '4';
					bi++;
					li += 1;
				}
				else if (strncmp(five, line + li, 4) == 0)
				{
					new_buffer[bi] = '5';
					bi++;
					li += 1;
				}
				else
					li++;
				break;
			case 's':
				if (strncmp(six, line + li, 3) == 0)
				{
					new_buffer[bi] = '6';
					bi++;
					li += 1;
				}
				else if (strncmp(seven, line + li, 5) == 0)
				{
					new_buffer[bi] = '7';
					bi++;
					li += 1;
				}
				else
					li++;
				break;
			case 'e':
				if (strncmp(eight, line + li, 5) == 0)
				{
					new_buffer[bi] = '8';
					bi++;
					li += 1;
				}
				else
					li++;
				break;
			case 'n':
				if (strncmp(nine, line + li, 4) == 0)
				{
					new_buffer[bi] = '9';
					bi++;
					li += 1;
				}
				else
					li++;
				break;
			default:
				li++;
				break;
		}
	}

	new_buffer[bi] = '\0';

	printf("%s\n", new_buffer);

	return get_calibration_value_part1(new_buffer);
}

int main(void)
{
	FILE *f;
	char buffer[BUF_SIZE];
	int calibration_value = 0;

	f = fopen("input", "r");
	if (f == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	while (fgets(buffer, sizeof(buffer), f) != NULL)
	{
		calibration_value += get_calibration_value_part2(buffer);
	}

	fclose(f);

	printf("Calibration value: %d\n", calibration_value);

	return EXIT_SUCCESS;
}
