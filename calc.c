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

#include "color.h"
#include "platform.h"
#include "optimizations.h"
#include "compiler.h"

/* Use POSIX.1-2008 */
#define _POSIX_C_SOURCE 200809L

/* Calc version */
#define __CALC_VERSION 1.7

/* Magic number that lets us check if the operator number is valid */
/* NOTE: if Calc returns the first number, even when the operator and
 * the second number were entered, try to recompile (rare issue on ARM) */
/* Also, define ARCHNUM (1 is x86, 2 is ARM) */
#ifdef ARCH
	#ifdef ARCH_x86
	#ifdef OS_WIN32
		#define __CALC_OPVAL 99
	#else
		#define __CALC_OPVAL 0x1400000000000
	#endif
	#define ARCHNUM 1
	#elif defined ARCH_ARM
	#define __CALC_OPVAL 0x7500000000
	#define ARCHNUM 2
	#endif
#else
	#warning "Architecture is unknown!"
	#warning "Using default x86 value for __CALC_OPVAL (0x1400000000000)"
	#define __CALC_OPVAL 0x1400000000000
#endif

/* Instead of constantly writing color(string, 1, rvideo),
 * only write color_rvid(string) */
#define color_rvid(str) color(str, 1, rvideo)

/* Instead of constantly writing color(string, 2, bold, underline),
 * only write color_bu(string) */
#define color_bu(str) color(str, 2, bold, underline)

/* Allow debug? (Default: yes (1), set to 0 to disable) */
#ifndef DEBUG
#define DEBUG 1
#endif

/* Check which OS is being used, and define OS accordingly */
#ifdef OS_WIN32
#define OS "Win32"
#elif defined OS_UNIX
#define OS "Unix"
#elif defined OS_UNK
#define OS "Unknown"
#endif

/* Function prototypes */
void calculate(double first, char *operand, double second);
void clearScr(void);
void parseInput(char *input);
void printHelp(void);
void printOps(void);
void printSpecVals(void);
void sigHandler(int sigNum);

/* Define "long unsigned int" as uint64_ct */
typedef long unsigned int uint64_ct;

/* Define "short unsigned int" as uint8_ct */
typedef short unsigned int uint8_ct;

/* Print colored output? (Default: yes (1); no (0)) */
uint8_ct usecolor = 1;

/* Print compilation info? (Default: yes (1); no (0)) */
uint8_ct showcmp = 1;

/* Print program flags? (Default: yes (1); no (0)) */
uint8_ct showflags = 1;

/* Print examples? (Default: yes (1); no (0)) */
uint8_ct showsamp = 1;

/* Enter just-calculator mode? (Default: no (0); yes (1)) */
uint8_ct justcalc = 0;

int
main(int argc, char **argv)
{
	/* Struct containing program options/flags */
	static struct option longopts[] = {
		{"just-calc", no_argument, 0, 'c'},
		{"no-examples", no_argument, 0, 'e'},
		{"no-flags", no_argument, 0, 'f'},
		{"no-cmp", no_argument, 0, 'm'},
		{"no-color", no_argument, 0, 'n'},
		{"help", no_argument, 0, 'h'}
	};

	int optind = 0;

	/* Check if flags have been passed */
	while ((optind = getopt_long(argc, argv, ":cefmnh", longopts, &optind)) != 1) {
		switch (optind) {

		/* Enter "just-calculator" mode */
		case 'c':
			justcalc = 1;
			fprintf(stderr, "[Enabled just-calculator mode]\n");
			break;

		/* Don't show examples when printing help */
		case 'e':
			showsamp = 0;
			fprintf(stderr, "[Disabled examples]\n");
			break;

		/* Don't show flags when printing help */
		case 'f':
			showflags = 0;
			fprintf(stderr, "[Disabled flags]\n");
			break;

		/* Don't show program compilation info */
		case 'm':
			showcmp = 0;
			fprintf(stderr, "[Disabled compilation info]\n");
			break;

		/* Disable colored output */
		case 'n':
			usecolor = 0;
			fprintf(stderr, "[Disabled colored output]\n");
			break;

		/* Print help and exit */
		case '?': case 'h':
			printHelp();
			return 0;
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

		/* Set variables to 0 */
		memset(input, 0, sizeof(*input));
		memset(&optind, 0, sizeof(optind));
	}

	/* If this point is somehow reached, exit gracefully */
	return 0;
}

/* Print a result, based on the parsed string */
void
calculate(double first, char *operand, double second)
{
	/* Make the output bold, if we are using colors */
	if (usecolor != 0 && justcalc != 1)
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
	case '<': case 'l':
		printf("%ld\n", (long int)first << (long int)second);
		break;
	case '>': case 'r':
		printf("%ld\n", (long int)first >> (long int)second);
		break;
	default:
		printf("Unknown operand \"%s\"\n", operand);
	}

	/* Back to normal (if using colors) */
	if (usecolor != 0 && justcalc != 1)
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

/* Parse user input */
void
parseInput(char *input)
{
	/* Enter just-calculator mode */
	if (!strcasecmp(input, "calc") && justcalc != 1) {
		justcalc = 1;
		fprintf(stderr, "[Entered just-calculator mode]\n");

	/* Clear the screen */
	} else if (!strcasecmp(input, "clear"))
		clearScr();

	/* Enable color */
	else if (!strcasecmp(input, "color") && justcalc != 1) {
		usecolor = 1;
		fprintf(stderr, "[Enabled color]\n");

	#if DEBUG != 0
	/* Debug info (hidden) */
	} else if (!strcasecmp(input, "debug")) {
		char archNumStr[10];

		if (ARCHNUM == 1)
			strcpy(archNumStr, "x86");
		else if (ARCHNUM == 2)
			strcpy(archNumStr, "ARM");
		else
			strcpy(archNumStr, "Unknown");

		/* Firstly, print if we are in just-calculator mode */
		fprintf(stderr, "Just-Calculator Mode: %d (yes: 1, no: 0)\n", justcalc);

		/* Print architecture, compiler, date & time info */
		fprintf(stderr, "Architecture, Compiler, Date & Time.\n");
		fprintf(stderr, "ARCH: %s\nARCHNUM: %d (%s)\nCC: %s %s\nDATE: %s\nTIME: %s\n\n", ARCH, ARCHNUM, archNumStr, CC, OPTS, __DATE__, __TIME__);

		/* Print the size of data types */
		fprintf(stderr, "Data Type Size.\n");
		fprintf(stderr, "(char) size: %ld, (char *) size: %ld\n(int) size: %ld, (int *) size: %ld\n\n",
				sizeof(char), sizeof(char *), sizeof(int), sizeof(int *));

		/* Print POSIX C source, last modify date, opval */
		fprintf(stderr, "Miscellaneous.\n");
		fprintf(stderr, "POSIX C Source: %ld\nCalc version: %.1f\nOpVal: 0x%lX (%lu)\n",
				_POSIX_C_SOURCE, __CALC_VERSION, __CALC_OPVAL, __CALC_OPVAL);
	#endif

	/* Exit without errors */
	} else if (!strcasecmp(input, "exit") || !strcasecmp(input, "quit"))
		exit(0);

	/* Enable examples in help section */
	else if (!strcasecmp(input, "examples") && justcalc != 1) {
		showsamp = 1;
		fprintf(stderr, "[Enabled examples]\n");

	/* Show flags */
	} else if (!strcasecmp(input, "flags") && justcalc != 1) {
		showflags = 1;
		fprintf(stderr, "[Enabled flags]\n");

	/* Print this program's help */
	} else if (!strcasecmp(input, "help"))
		printHelp();
	
	/* Print available operands */
	else if ((!strcasecmp(input, "operands") || !strcasecmp(input, "ops")) && justcalc != 1)
		printOps();

	/* Exit out of just-calculator mode */
	else if (!strcasecmp(input, "nocalc") && justcalc != 0) {
		justcalc = 0;
		fprintf(stderr, "[Disabled just-calculator mode]\n");

	/* Disable color */
	} else if (!strcasecmp(input, "nocolor") && justcalc != 1) {
		usecolor = 0;
		fprintf(stderr, "[Disabled color]\n");

	/* Don't show examples */
	} else if (!strcasecmp(input, "noexamples") && justcalc != 1) {
		showsamp = 0;
		fprintf(stderr, "[Disabled examples]\n");

	/* Don't show flags */
	} else if (!strcasecmp(input, "noflags") && justcalc != 1) {
		showflags = 0;
		fprintf(stderr, "[Disabled flags]\n");

	/* Print special values */
	} else if (!strcasecmp(input, "specvals") && justcalc != 1)
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
		uint64_ct opnum = (uint64_ct)array[1];

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
			if (usecolor != 0 && justcalc != 1)
				printf("\e[1m%.10f\e[0m\n", first);
			else
				printf("%.10f\n", first);
		}

	/* Set variables to 0 */
	memset(array, 0, sizeof(*array));
	memset(&i, 0, sizeof(i));
	memset(&first, 0, sizeof(first));
	memset(operand, 0, sizeof(*operand));
	memset(&opnum, 0, sizeof(opnum));
	memset(&second, 0, sizeof(second));
	}
}

/* Print this program's help */
void
printHelp(void)
{
	printf("Basic Calculator by Salonia Matteo, made on 25/01/2021, version %.1f\n", __CALC_VERSION);

	/* Show program compilation info */
	if (showcmp != 0 && justcalc != 1)
		printf("Compiled on %s at %s %s, using compiler %s, targeting platform %s, operating system %s.\n", __DATE__, __TIME__, OPTS, CC, ARCH, OS);

	/* Show flags */
	if (showflags != 0 && justcalc != 1)
		printf("Flag order: [cefmnh]\nFlags:\n\
%s \t| %s \tEnter \"just-calculator\" mode\n\
%s \t| %s \tDon't show examples\n\
%s \t| %s \tDon't show these flags\n\
%s \t\t| %s \tShow this help\n\
%s \t| %s \tDon't show program compilation info\n\
%s \t| %s \tDon't color the output\n",
color_rvid("--just-calc"), color_rvid("-c"),
color_rvid("--no-examples"), color_rvid("-e"),
color_rvid("--no-flags"), color_rvid("-f"),
color_rvid("--help"), color_rvid("-h"),
color_rvid("--no-cmp"), color_rvid("-m"),
color_rvid("--no-color"), color_rvid("-n"));

	/* If we are in just-calculator mode, print reduced command list */
	if (justcalc != 1)
		printf("\nAvailable commands: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s (or %s), %s, %s.\n",
			color_rvid("calc"), color_rvid("clear"), color_rvid("color"), color_rvid("examples"),
			color_rvid("exit"), color_rvid("flags"), color_rvid("help"),
			color_rvid("nocolor"), color_rvid("noexamples"), color_rvid("noflags"),
			color_rvid("operands"), color_rvid("ops"), color_rvid("quit"), color_rvid("specvals"));
	else
		printf("\n(Just-calculator mode)\nAvailable commands: %s, %s, %s, %s, %s.\n",
			color_rvid("clear"), color_rvid("exit"), color_rvid("help"), color_rvid("quit"), color_rvid("nocalc"));

	/* Show examples */
	if (showsamp != 0 && justcalc != 1)
		printf("Examples:\n\
%s\n\
1 + 1\t\t1 p 1\t\tAddition\tReturns 2\n\
1 - 1\t\t1 s 1\t\tSubtraction\tReturns 0\n\
2 * 2\t\t2 t 2\t\tMultiplication\tReturns 4\n\
4 / 2\t\t4 d 2\t\tDivision\tReturns 2\n\
4 %% 2\t\t4 m 2\t\tModulus\t\tReturns 0\n\
1 < 16\t\t1 l 16\t\tBit-shifting\tReturns 65536\n\
4096 > 1\t4096 r 1\tBit-shifting\tReturns 2048\n",
color_bu("[Cmd]\t\t[Alt sign]\t[Description]\t[Result]"));
}

/* Print available operands and their short notation */
void
printOps(void)
{
	printf("Available operands:\n\
%s Can be written as %s\n\
+\t\t\t\tp\n\
-\t\t\t\ts\n\
*\t\t\t\tt\n\
/\t\t\t\td\n\
%%\t\t\t\tm\n\
<\t\t\t\tl\n\
>\t\t\t\tr\n",
color_bu("[Symbol]"), color_bu("[Latin letter]"));
}

/* Print special values */
void
printSpecVals(void)
{
	printf("Special values: you can type these words to automatically get their value.\n\
%s These are case insensitive, so you can type them in all lowercase, uppercase, etc.\n\
%s\t%s\n\
pi\t\tThe value of Pi\n\
pi2\t\tPi / 2\n\
pi4\t\tPi / 4\n\
1pi\t\t1 / Pi\n\
2pi\t\t2 / Pi\n\
pisq\t\tPi * Pi\n\
e\t\tThe value of e\n",
color_bu("NOTE:"), color_bu("[Symbol]"), color_bu("[Description]"));
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

	/* Set variable to 0 */
	memset(&sigNum, 0, sizeof(sigNum));

	/* Free "coloredStr" to prevent memory leaks */
	if (_free_color() != 0)
		fprintf(stderr, "WARNING! Failed to free coloredStr!\n");

	exit(0);
}
