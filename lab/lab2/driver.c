#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lab2.h"

void read_line(char* buffer, int length) {
    fgets(buffer, length, stdin);
    if(strlen(buffer) != 0)
        buffer[strlen(buffer) - 1] = 0;
}

typedef enum {
	Invalid,
	Print,
	Modify,
	Move,
	Exit
} Choice;

Choice get_choice(int food_pos, int num_foods) {
	int choice = Invalid;

	while(choice == Invalid) {
		printf("[food %2d/%2d] Enter your choice: 1 to print, 2 to modify, 3 to move, 4 to exit: ",
			food_pos, num_foods);

		char input[100];
		read_line(input, sizeof(input));
		// sscanf returns the number of items successfully scanned, so 0 in this case for failure.
		if(sscanf(input, "%d", &choice) == 0)
			choice = Invalid;
	}

	return choice;
}

void read_string(char* str, int max_length) {
	char input[100];
	read_line(input, sizeof(input));
	// strncpy is a weird one. it copies at most the number of characters you give it.
	// if it does, it will *not* zero-terminate the string, so we'll do it ourselves.
	strncpy(str, input, max_length);
	str[max_length - 1] = 0;
}

void modify_food(FILE* database, int food_pos) {
	Food f;
	read_food(database, &f, food_pos);

	do {
		printf("Current name is '%s'. Enter new name: ", f.name);
		read_string(f.name, sizeof(f.name));
	} while(strlen(f.name) == 0);

	char input[100];
	do {
		printf("Current price is $%.2f. Enter new price: ", f.price);
		read_line(input, sizeof(input));
	} while(sscanf(input, "%lf", &f.price) == 0 || f.price <= 0);

	do {
		printf("Current stock is %d. Enter new stock: ", f.stock);
		read_line(input, sizeof(input));
	} while(sscanf(input, "%d", &f.stock) == 0 || f.stock <= 0);

	write_food(database, &f, food_pos);
}

int change_food(FILE* database, int food_pos, int num_foods) {
	int new_pos = 0;

	char input[100];
	do {
		printf("Enter new position [0 .. %d]: ", num_foods);
		read_line(input, sizeof(input));
	} while(sscanf(input, "%d", &new_pos) == 0 || new_pos < 0 || new_pos > num_foods);

	// special case when we are adding a new food to the end of the file
	if(new_pos != food_pos && new_pos == num_foods) {
		Food empty = { "", 0, 0 };
		write_food(database, &empty, new_pos);
		modify_food(database, new_pos);
	}

	return new_pos;
}

int main() {
	int food_pos = 0;
	FILE* database = open_database();
	int num_foods = count_food(database);
	bool running = true;

	while(running) {
		switch(get_choice(food_pos, num_foods)) {
			case Print:  print_food(database, food_pos);  break;
			case Modify: modify_food(database, food_pos); break;
			case Move:
				food_pos = change_food(database, food_pos, num_foods);
				num_foods = count_food(database);
				break;
			case Exit: running = false; break;

			// this shouldn't happen cause of how get_choice is implemented but...
			default: break;
		}
	}

	fclose(database);
    return 0;
}