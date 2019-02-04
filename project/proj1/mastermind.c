//Siyu Zhang
//siz24
#include <stdio.h>
#include <ctype.h> //for tolower
#include <string.h> //for strlen()
#include <stdlib.h> //for generating random nums
#include <time.h> //for generating random nums

void get_line(char* input, int size)
{
	fgets(input, size, stdin);
	int len = strlen(input);
	input[len - 1] = '\0';
}

int streq(const char* a, const char* b)
{
	return strcmp(a, b) == 0;
}

int check_difficulty(const char* difficulty)
{
	int diff;
	diff = atoi(difficulty); //parse a string into an int
	if (diff == 3||diff == 4||diff == 5||diff == 6)
		return 1;
	else
		return 0;
}

int get_random(int difficulty)
{
	int r;
	return r = rand() % difficulty + 1; //generate a random number from [1, difficulty]
}

void translate(char* input, int index, int rand)  //translate random int to color
{
	switch(rand)
	{
		case 1:
			input[index] = 'r';
			break;
		case 2:
			input[index] = 'g';
			break;
		case 3:
			input[index] = 'b';
			break;
		case 4:
			input[index] = 'y';
			break;
		case 5:
			input[index] = 'o';
			break;
		case 6:
			input[index] = 'p';
			break;
		default:
			break;
	}
}

void get_solution(char* input, int difficulty)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		int r = get_random(difficulty);
		translate(input, i, r);
	}
}

void show_info(char* solution, int diff, int generate)
{
	switch (diff)
	{
		case 3: 
			if (generate == 1)
				get_solution(solution, 3);
			printf("Your guess should be chosen from three colors: \nr/R=red, \ng/G=green, \nb/B=blue.\n");
			break;
		case 4: 
			if (generate == 1)
				get_solution(solution, 4);
			printf("Your guess should be chosen from four colors: \nr/R=red, \ng/G=green, \nb/B=blue, \ny/Y=yellow.\n");
			break;
		case 5:
			if (generate == 1)
				get_solution(solution, 5);
			printf("Your guess should be chosen from five colors: \nr/R=red, \ng/G=green, \nb/B=blue, \ny/Y=yellow, \no/O=orange.\n");
			break;
		case 6: 
			if (generate == 1)
				get_solution(solution, 6);
			printf("Your guess should be chosen from six colors: \nr/R=red, \ng/G=green, \nb/B=blue, \ny/Y=yellow, \no/O=orange, \np/P=purple.\n");
			break;
		default:
			break;
	}
}

int valid_guess(char* input, int diff)
{
	if (strlen(input) != 4) //enter more chars
		return 0;
	int i;
	switch (diff)
	{
		case 3:
			for (i = 0; i < 4; i++)
			{
				if (input[i] == 'r' || input[i] == 'R' || input[i] == 'g' || input[i] == 'G' || input[i] == 'b' || input[i] == 'B')
					continue;
				else
					return 0;
			}
			break;
		case 4:
			for (i = 0; i < 4; i++)
			{
				if (input[i] == 'r' || input[i] == 'R' || input[i] == 'g' || input[i] == 'G' || input[i] == 'b' || input[i] == 'B'
				 || input[i] == 'y' || input[i] == 'Y')
					continue;
				else
					return 0;
			}
		break;
		case 5:
			for (i = 0; i < 4; i++)
			{
				if (input[i] == 'r' || input[i] == 'R' || input[i] == 'g' || input[i] == 'G' || input[i] == 'b' || input[i] == 'B'
					 || input[i] == 'y' || input[i] == 'Y' || input[i] == 'o' || input[i] == 'O')
					continue;
				else
					return 0;
			}
			break;
		case 6:
			for (i = 0; i < 4; i++)
			{
				if (input[i] == 'r' || input[i] == 'R' || input[i] == 'g' || input[i] == 'G' || input[i] == 'b' || input[i] == 'B'
				 || input[i] == 'y' || input[i] == 'Y' || input[i] == 'o' || input[i] == 'O' || input[i] == 'p' || input[i] == 'P')
					continue;
				else
					return 0;
			}
			break;
		default:
			return 0;
			break;	
	}
	return 1;
}

int check_guess(char* guess, char* solution)
{
	int c_in_cp = 4;
	int c_in_wp = 0;
	int i;
	int fsol[4]; //flags for checking matched solutions
	int fgue[4]; //flags for checking matched guesses
	
	for (i = 0; i < 4; i++) //check correct places first, mark correct fsol and fgue
	{
		if (solution[i] == guess[i] || solution[i] == tolower(guess[i])) 
		{
			fsol[i] = 1;
			fgue[i] = 1;
		}
		else 
		{
			c_in_cp--;
			fsol[i] = 0;
			fgue[i] = 0;
		}
			
	}
	
	for (i = 0; i < 4; i++) //check correct color but wrong places
	{
		if (fgue[i] == 1) //this guess[i] is occupied
		{
			continue;
		}
		int j;
		for (j = 0; j < 4; j++)
		{
			if (fsol[j] == 1) //this solution[j] is occupied
			{
				continue;
			}
			else if (guess[i] == solution[j] || tolower(guess[i]) == solution[j] ) //correct color
			{
				c_in_wp++;
				fsol[j] = 1;
				break;
			}
			else // no color matches
			{
				fsol[j] = 0;
			}
		}
	}
	if (c_in_cp == 4) //win the game
		return 1;
	printf("Wrong guess! Your guess is %s.\n", guess);
	printf("But you got %d correct color(s) in correct place and %d correct color(s) in wrong place!\n", c_in_cp, c_in_wp);
	return 0;
	
}


int main()
{
	srand((unsigned)time(NULL));
	char difficulty[100];
	char solution[100];
	char guess[100];
	char again[100];
	int num_guess;
	int diff = 0; 
	
	Game:
	    num_guess = 1;
		printf("Enter a number from 3 to 6 to choose the difficulty: ");
		get_line(difficulty, 100);
		while (check_difficulty(difficulty) == 0)
		{
			printf("Invalid difficulty! Please enter a number between 3 to 6: ");
			get_line(difficulty, 100);
		}
		diff = atoi(difficulty);
		show_info(solution, diff, 1);
		printf("Now, you can begin your 10 guesses.\n");
		while (num_guess <= 10)
		{
			printf("\nGuess %d: \nEnter a sequence of 4 letters which represent 4 colors: ", num_guess);
			printf("\nThe solution is %s\n", solution); //solution!!!!!!!!
			get_line(guess, 100);
			while (valid_guess(guess, diff) == 0)
			{
				printf("Invalid guess! \n");
				show_info(solution, diff, 0);
				printf("Please enter a sequence of valid 4 letters that is corresponding to your choice of difficulty! \n");
				printf("Now, try again: \n");
				get_line(guess, 100);
			}
			
			if (check_guess(guess, solution) == 1) //user won the game
			{
				printf("Game Over: \nCongradualations!!!!!!! You won the Game in only %d guess(es)!\n", num_guess);
				break;
			}
			num_guess++;
		}
		if (num_guess > 10)
		{
			printf("Game Over: \nYou lost!!!!!!! You didn't get the correct solution in 10 guesses. The solution is %s.\n", solution);
		}
		printf("\nDo you want to play again? Y/y for yes and N/n for no: ");
		get_line(again, 100);
		if (again[0] == 'Y' || again[0] == 'y')
			goto Game;//replay???????????
	
	return 0;
}
