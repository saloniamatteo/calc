/* See LICENSE file for copyright and license details.
 *
 * calc is a Simple Calculator written in C by Salonia Matteo.
 * It takes input from stdin using libreadline, and prints the
 * result from the requested operation to stdout.
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 *
 */

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#include <readline/readline.h>

#include "platform.h"
#include "optimizations.h"
#include "compiler.h"

/* Use POSIX.1-2008 */
#define _POSIX_C_SOURCE 200809L

/* When was this last modified */
#define _CALC_LAST_MOD_DATE "14/03/2021"

/* Magic number that lets us check if the operator number is valid */
/* NOTE: if Calc returns the first number, even when the operator and
 * the second number were entered, try to recompile (rare issue on ARM) */
#ifdef ARCH
	#ifdef ARCH_x86
	#define __CALC_OPVAL 0x1400000000000
	#elif defined ARCH_ARM
	#define __CALC_OPVAL 0x7500000000
	#endif
#else
	#warning "Your architecture is unknown!"
	#warning "Using default x86 value..."
	#define __CALC_OPVAL 0x1400000000000
#endif

/* Function prototypes */
void calculate(double first, char *operand, double second);
void clearScr(void);
void parseInput(char *input);
void printHelp(void);
void printOps(void);
void printSpecVals(void);
void sigHandler(int sigNum);

/* Define "long unsigned int" as uint64_t */
typedef long unsigned int uint64_t;

/* Print colored output? (Default: yes (1)) */
int usecolor = 1;

int
main(int argc, char **argv)
{
	int optind = 0;

	/* Struct containing program options/flags */
	static struct option longopts[] = {
		{"help", no_argument, 0, 'h'},
		{"no-color", no_argument, 0, 'n'}
	};

	/* Check if flags have been passed */
	while ((optind = getopt_long(argc, argv, ":hn", longopts, &optind)) != 1) {
		switch (optind) {

		/* Unknown option */
		case '?':
			fprintf(stderr, "[Option \"%c\" is unknown, ignoring]\n", optopt);

		/* Print help and exit */
		case 'h':
			printHelp();
			return 0;
			break;

		/* Disable colored output */
		case 'n':
			usecolor = 0;
			fprintf(stderr, "[Disabled colored output]\n");
			break;
		}

		if (optind <= 0)
			break;
	}

	/* Print program info */
	printHelp();

	/* Run indefinitely */
	for (;;) {

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

		/* Parse the input */
		parseInput(input);
	}

	/* If this point is somehow reached, exit gracefully */
	return 0;
}

/* Print a result, based on the parsed string */
void
calculate(double first, char *operand, double second)
{
	/* Make the output bold, if we are using colors */
	if (usecolor != 0)
		printf("\e[1m");

	/* Check the operand */
	switch (operand[0]) {
	case '+': case 'p':
		printf("%.10f\n", first + second);
		break;
	case '-': case 's':
		printf("%.10f\n", first - second);
		break;
	case '*': case 't':
		printf("%.10f\n", first * second);
		break;
	case '/': case 'd':
		printf("%.10f\n", first / second);
		break;
	case '%': case 'm':
		printf("%ld\n", (long int)first % (long int)second);
		break;
	default:
		printf("Unknown operand \"%s\"\n", operand);
	}

	/* Back to normal (if using colors) */
	if (usecolor != 0)
		printf("\e[0m");
}

/* Clear the screen */
void
clearScr(void)
{
	/* Run "clear" if the user is using Unix/a Unix-Like OS */
	#ifdef __unix__
	system("clear");

	/* Run "cls" if the user is using windows */
	#elif defined _MSC_VER
	system("cls");
	#endif
}

/* Print this program's help */
void
printHelp(void)
{
	printf("Basic Calculator by Salonia Matteo, made on 25/01/2021, last modified %s\n\
Compiled on %s at %s %s, using compiler %s, targeting platform %s.\n\
Available commands:\
\e[7mclear\e[0m, \e[7mhelp\e[0m, \e[7mexit\e[0m, \e[7mquit\e[0m, \
\e[7moperands\e[0m (or \e[7mops\e[0m), \e[7mspecvals\e[0m, \
\e[7mnocolor\e[0m, \e[7mcolor\e[0m.\n\
Examples:\n\
\e[1;4m[Cmd]\t[Alt sign]\t[Description]\t[Result]\e[0m\n\
1 + 1\t1 p 1\t\tAddition\tReturns 2\n\
1 - 1\t1 s 1\t\tSubtraction\tReturns 0\n\
2 * 2\t2 t 2\t\tMultiplication\tReturns 4\n\
4 / 2\t4 d 2\t\tDivision\tReturns 2\n\
4 %% 2\t4 m 2\t\tModulus\t\tReturns 0\n",
_CALC_LAST_MOD_DATE,
__DATE__, __TIME__,
OPTS, CC, ARCH);
}

/* Parse user input */
void
parseInput(char *input)
{
	/* Clear the screen */
	if (!strcasecmp(input, "clear"))
		clearScr();

	/* Enable color */
	else if (!strcasecmp(input, "color")) {
		usecolor = 1;
		fprintf(stderr, "[Enabled color]\n");

	/* Exit without errors */
	} else if (!strcasecmp(input, "exit") || !strcasecmp(input, "quit"))
		exit(0);

	/* Print this program's help */
	else if (!strcasecmp(input, "help"))
		printHelp();
	
	/* Print available operands */
	else if (!strcasecmp(input, "operands") || !strcasecmp(input, "ops"))
		printOps();

	/* Disable color */
	else if (!strcasecmp(input, "nocolor")) {
		usecolor = 0;
		fprintf(stderr, "[Disabled color]\n");

	/* Print special values */
	} else if (!strcasecmp(input, "specvals"))
		printSpecVals();

	else {
		/* TODO: use a better string parsing method */
		int i = 0;
		char *array[5], *token = strtok(input, " ");
		double first = 0, second = 0;
	
		/* Save items to array */
		while (token != NULL) {
			array[i++] = token;
			token = strtok(NULL, " ");
		}
		/* End TODO */

		/* Assign array items to variables */
		/* Special values: pi, pi2, pi4, 1pi, 2pi, pisq, e */
		if (!strcasecmp(array[0], "pi"))
			first = M_PI;
		else if (!strcasecmp(array[0], "pi2"))
			first = M_PI_2;
		else if (!strcasecmp(array[0], "pi4"))
			first = M_PI_4;
		else if (!strcasecmp(array[0], "1pi"))
			first = M_1_PI;
		else if (!strcasecmp(array[0], "2pi"))
			first = M_2_PI;
		else if (!strcasecmp(array[0], "pisq"))
			first = M_PI * M_PI;
		else if (!strcasecmp(array[0], "e"))
			first = M_E;
		else
			first = atof(array[0]);

		if (!strcasecmp(array[2], "pi"))
			second = M_PI;
		else if (!strcasecmp(array[2], "pi2"))
			second = M_PI_2;
		else if (!strcasecmp(array[2], "pi4"))
			second = M_PI_4;
		else if (!strcasecmp(array[2], "1pi"))
			second = M_1_PI;
		else if (!strcasecmp(array[2], "2pi"))
			second = M_2_PI;
		else if (!strcasecmp(array[2], "pisq"))
			second = M_PI * M_PI;
		else if (!strcasecmp(array[2], "e"))
			second = M_E;
		else
			second = atof(array[2]);

		/* Store operand in a single character */
		char operand[1];
		/* Convert operand value to long unsigned int */
		uint64_t opnum = (uint64_t)array[1];

		/* Check if operand exists, using a magic number (this somehow works) */
		#ifdef ARCH_ARM
		if (opnum > __CALC_OPVAL) {
		#else
		if (opnum < __CALC_OPVAL) {
		#endif
			strcpy(operand, array[1]);
			calculate(first, operand, second);
		} else {
			/* Operand does not exist, print only the first number (10 decimal places) */
			if (usecolor != 0)
				printf("\e[1m%.10f\e[0m\n", first);
			else
				printf("%.10f\n", first);
		}
	}
}

/* Print available operands and their short notation */
void
printOps(void)
{
	printf("Available operands:\n\
\e[1;4m[Symbol]\e[0m Can be written as \e[1;4m[Latin letter]\e[0m\n\
+\t\t\t\tp\n\
-\t\t\t\ts\n\
*\t\t\t\tt\n\
/\t\t\t\td\n\
%%\t\t\t\tm\n");
}

/* Print special values */
void
printSpecVals(void)
{
	printf("Special values: you can type these words to automatically get their value.\n\
\e[1;4mNOTE: \e[0mThese are case insensitive, so you can type them in all lowercase, uppercase, etc.\n\
\e[1;4m[Symbol]\e[0m\t\e[1;4m[Description]\e[0m\n\
pi\t\tThe value of Pi\n\
pi2\t\tPi / 2\n\
pi4\t\tPi / 4\n\
1pi\t\t1 / Pi\n\
2pi\t\t2 / Pi\n\
pisq\t\tPi * Pi\n\
e\t\tThe value of e\n");
}

/* Handle signals */
void
sigHandler(int sigNum)
{
	/* Inform user what signal was sent */
	char *sigName;

	/* Check if signal is CTRL+C */
	if (sigNum == 2)
		sigName = "(CTRL+C)";
	/* Check if signal is CTRL+D */
	else if (sigNum == 11)
		sigName = "(CTRL+D)";
	else
		sigName = "";

	/* Print detected signal and exit gracefully */
	fprintf(stderr, "[Detected Signal %d %s]\n", sigNum, sigName);
	exit(0);
}
