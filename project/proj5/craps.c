//Siyu Zhang
//siz24
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

void get_line(char* input, int size)
{
	fgets(input, size, stdin);
	int len = strlen(input);
	input[len - 1] = '\0';
}

int streq(const char* a, const char* b)
{
	return strcasecmp(a, b) == 0;
}

FILE* open_file(char* filename)
{
	FILE* f = fopen(filename, "rb");
	if (f == NULL)
	{
		printf("The file cannot be opened.\n");
		exit(1);
	}
		
	return f;
}

unsigned char get_random_dice(FILE* f, unsigned char one_dice)
{
	if (fread(&one_dice, sizeof(one_dice), 1, f) == 0) //get random failed
	{
		printf("Getting random failed.\n");
		fclose(f);
		exit(1);
	}
	//get random succeeded
	one_dice = one_dice % 6 + 1;   //convert a random number into the range [1, 6]
	return one_dice;
}

int the_point(FILE* f, unsigned char point)
{
	int result = -1;
	unsigned char first = 0;
	unsigned char second = 0;
	unsigned char sum;
	
	while (result == -1)
	{
		first = get_random_dice(f, first);
		second = get_random_dice(f, second);
		sum = first + second;
		printf("You have rolled %u + %u = %u\n", first, second, sum);
		if (sum == 7)
			result = 0;  //the player immediately loses
		else if (sum == point)
			result = 1;  //the player immediately wins
	}
	
	return result;
}

int play_game(FILE* f)
{
	/*
		2, 3, or 12, the player immediately loses.
		7 or 11, the player immediately wins.
		Any other number, this number becomes “the point.” In this case, the player keeps rolling dice until:
			The player rolls a 7, in which case the player loses.
			The player rolls the point again, in which case the player wins.
		Any other number means nothing; keep rolling dice automatically until one of the two conditions above occurs.
	*/
	 
	int result = -1;
	unsigned char first = 0;
	unsigned char second = 0;
	unsigned char sum;
	
	first = get_random_dice(f, first);
	second = get_random_dice(f, second);
	sum = first + second;
	
	printf("You have rolled %u + %u = %u\n", first, second, sum);
	
	if (sum == 2 || sum == 3 || sum == 12)
		result = 0;  //the player immediately loses
	else if (sum == 7 || sum == 11)
		result = 1;  //the player immediately wins
	else    //have the point
	{
		printf("You get \"the point\" %u. Keeps rolling dice...\n", sum);
		result = the_point(f, sum);
	}
	
	return result;
}

void show_result(int result)
{
	if (result == 1)
		printf("Congratulations! You win.\n");
	else
		printf("Sorry, you lost!\n");
}

int quit_game(FILE* f, const char* name)
{
	printf("Goodbye, %s!\n", name);
	fclose(f);
	return 0;
}

int main(int argc, char** argv)
{
	/*	1. Ask the player their name, and display a welcome message.
		2. Ask if the player would like to play or quit.
		3. Roll two dice, and display their values and the total.
			If the player won, congratulate the player.
			If the player lost, tell them.
			Otherwise, store the first roll as the point and keep rolling until the player wins or loses. The point is only set on the first roll.
		4. Go back to step 2.
	*/
	char name[100];
	char quit[100];
	char again[100];
	int result;
	
	if (argc < 2)
	{
		printf("Missing arguement. You have to enter a file name.\n");
		return 1;
	}
	FILE* f = open_file(argv[1]);  //try to open the file
	
	printf("Welcome to Siyu's Casino! Please enter your name: ");
	get_line(name, 100);
	do 
	{
		printf("Hello, %s, would you like to Play or Quit? ", name);
		get_line(quit, 100);
	} while (!streq(quit, "quit") && !streq (quit, "play"));
	if (streq(quit, "quit"))
		return quit_game(f, name);		
	
	Game:	
		result = -1;
		result = play_game(f);
	
		show_result(result);
		
		do 
		{
			printf("Would you like to play again? ");
			get_line(again, 100);
		} while (!streq(again, "yes") && !streq (again, "no"));
		if (streq(again, "no"))
			return quit_game(f, name);		
		else
			goto Game;
}