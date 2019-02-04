#ifndef _LAB2_H_
#define _LAB2_H_

typedef struct {
	char name[16];
	double price;
	int stock;
} Food;

int count_food(FILE* f);
void read_food(FILE* f, Food* pfood, int i);
void write_food(FILE* f, Food* pfood, int i);
void print_food(FILE* f, int i);
FILE* open_database();

#endif