#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

/* Basic Calculator by Salonia Matteo */

/* This function clears the screen */
void clearScr() {

/* If using a unix-like system, use the sane "clear" */
#	ifdef __unix__
		system("clear");
/* If using Windows, use "cls" */
#	elif defined _MSC_VER
		system("cls");
#	endif
}

/* Function that will calculate the required values */
void calculate(double first, char *operand, double second) {

	/* Color the output (red background) */
	printf("\e[41m");

	/* Check the operand */
	switch(operand[0]) {
		case '+': case 'p':
			printf("%f\n", first + second);
			break;
		case '-': case 's':
			printf("%f\n", first - second);
			break;
		case '*': case 't':
			printf("%f\n", first * second);
			break;
		case '/': case 'd':
			printf("%f\n", first / second);
			break;
		case '%': case 'm':
			printf("%ld\n", (long int)first % (long int)second);
			break;
		default:
			printf("Unknown operand \"%s\"\n", operand);
	}

	/* Back to normal */
	printf("\e[0m");
}

/* Function that prints help */
void printHelp() {
	printf("Basic Calculator by Salonia Matteo, made on 25/01/2021\n\
Compiled on %s at %s.\n\
Available commands: \e[7mclear\e[0m, \e[7mhelp\e[0m, \e[7mexit\e[0m, \e[7mquit\e[0m, \e[7moperands\e[0m (or \e[7mops\e[0m).\n\
Examples:\n\
\e[1;4m[Cmd]\t[Alt sign]\t[Description]\t[Result]\e[0m\n\
1 + 1\t1 p 1\t\tAddition\tReturns 2\n\
1 - 1\t1 s 1\t\tSubtraction\tReturns 0\n\
2 * 2\t2 t 2\t\tMultiplication\tReturns 4\n\
4 / 2\t4 d 2\t\tDivision\tReturns 2\n\
4 %% 2\t4 m 2\t\tModulus\t\tReturns 0\n", __DATE__, __TIME__);
}

/* Function that prints operands */
void printOps() {
	printf("Available operands:\n\
\e[1;4m[Symbol]\e[0m Can be written as \e[1;4m[Latin letter]\e[0m\n\
+\t\t\t\tp\n\
-\t\t\t\ts\n\
*\t\t\t\tt\n\
/\t\t\t\td\n\
%%\t\t\t\tm\n");
}

/* Function to parse input */
void parseInput(char *input) {

	/* Use an array to store available commands,
	 instead of manually checking for each occurrence */
	char *cmdArray[] = {"clear", "help", "exit", "quit", "operands", "ops"};

	/* If input is not at least 5 characters and is not a command, exit
	 (the most basic operation, like 1 + 1, requires 5 characters) */
	if(strlen(input) < 5 && strstr(*cmdArray, input) != 0) {
		printHelp();
		exit(1);
	}

	int i = 0;
	char *array[5];
	char *token = strtok(input, " ");

	/* Save items to array */
	while(token != NULL) {
		array[i++] = token;
		token = strtok(NULL, " ");
	}

	/* Check if input is "clear" */
	if(strcmp(input, "clear") == 0)
		clearScr();

	/* If user wants to quit, exit without errors */
	else if(strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
		printf("Exiting...Goodbye!\n");
		exit(0);

	/* Check if input is "help" */
	} else if(strcmp(input, "help") == 0)
		printHelp();

	/* Check if input is "operands" or "ops" */
	else if(strcmp(input, "operands") == 0 || strcmp(input, "ops") == 0)
		printOps();

	/* Check if array items exist, otherwise exit */
	else if(array[0] <= 0 || array[2] <= 0) {
		printHelp();
		exit(1);

	/* Check if array items are greater than 1 */
	} else {

		/* Assign array items to variables */
		double first = atof(array[0]);

		double second = atof(array[2]);

		/* Even though we only need 1 character, make the array bigger,
		 so if some shit happens, the program does not shit itself */
		char operand[10] = "+";
		strcpy(operand, array[1]);

		calculate(first, operand, second);
	}
}

/* This function will handle signals */
void sigHandler(int sigNum) {

	/* Inform user what signal was sent */
	char *sigName;
	if(sigNum == 2)
		sigName = "(CTRL+C)";
	else if(sigNum == 11)
		sigName = "(CTRL+D)";
	else
		sigName = "";

	printf("Detected Signal %d %s, exiting...Goodbye!\n", sigNum, sigName);
	exit(0);
}

int main() {

	/* Print program info */
	printHelp();

	/* Infinite loop */
	for(;;) {

		/* Handle signals */
		signal(SIGABRT, sigHandler);
		signal(SIGFPE, sigHandler);
		signal(SIGILL, sigHandler);
		/* CTRL+C */
		signal(SIGINT, sigHandler);
		/* CTRL+D */
		signal(SIGSEGV, sigHandler);
		signal(SIGTERM, sigHandler);

		/* Ask user input */
		char *input = readline("\e[1;4mcalc>\e[0m ");

		/* Parse the input, and identify what to do */
		parseInput(input);
	}

	/* If this point is somehow reached, exit without errors */
	return 0;
}
