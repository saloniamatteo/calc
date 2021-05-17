/* See LICENSE file for copyright and license details.
 *
 * calc is a Simple Calculator written in C by Salonia Matteo.
 * It takes input from stdin using libreadline, and prints the
 * result from the requested operation to stdout.
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 *
 */

#define _GNU_SOURCE
#define _FORTIFY_SOURCE 2
#define _POSIX_C_SOURCE 200809L

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
#include "rpn.c"

/* Calc version */
#define __CALC_VERSION_MAJOR "2"
#define __CALC_VERSION_MINOR "0"
#define __CALC_VERSION __CALC_VERSION_MAJOR "." __CALC_VERSION_MINOR

/* Magic number that lets us check if the operator number is valid */
/* NOTE: if Calc returns the first number, even when the operator and
 * the second number were entered, try to recompile (rare issue on ARM) */
/* Also, define ARCHNUM (1 is x86, 2 is ARM) */
#ifdef ARCH
	#ifdef ARCH_x86
	#ifdef OS_WIN32
		#define __CALC_OPVAL 99
	#else
		/* TODO: get rid of OpVal; Tested CPUs: */
		/* Intel Core i5-4340M  (2C4T) = 0x1400000000000 */
		/* Intel Core i7-4700MQ (4C8T) = 0x2000000000000 */
		#define __CALC_OPVAL 0x2000000000000
	#endif
	#define ARCHNUM 1
	#elif defined ARCH_ARM
	#define __CALC_OPVAL 0x7500000000
	#define ARCHNUM 2
	#endif
#else
	#warning "Architecture is unknown! Using default x86 value for __CALC_OPVAL (0x1400000000000)"
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

/* Allow debugging Opval? (Default: no (0), set to 1 to enable */
#ifndef DEBUG_OPVAL
#define DEBUG_OPVAL 0
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
void calculate(double, char *, double);
void clearScr(void);
void parseInput(char *);
void printHelp(void);
void printOps(void);
void printSpecVals(void);
void sigHandler(int);

/* Define "long unsigned int" as uint64_ct */
typedef long unsigned int uint64_ct;

/* Flags */
enum calc_flags {
	usecolor = 1 << 0,	/* Print colored output? (Default: yes) */
	showcmp = 1 << 1,	/* Print compilation info? (Default: yes) */
	showflags = 1 << 2,	/* Print program flags? (Default: yes) */
	showsamp = 1 << 3,	/* Print examples? (Default: yes) */
	justcalc = 1 << 4,	/* Enter just-calculator mode? (Default: no (0); yes (1)) */
};

/* This variable will hold the flags */
unsigned short int flags = 0;

int
main(int argc, char **argv)
{
	/* Initialise flags */
	/* Print colored output? (Default: yes) */
	flags |= usecolor;

	/* Print compilation info? (Default: yes) */
	flags |= showcmp;

	/* Print program flags? (Default: yes) */
	flags |= showflags;

	/* Print examples? (Default: yes) */
	flags |= showsamp;

	/* Enter just-calculator mode? (Default: no (0); yes (1)) */
	flags &= ~justcalc;

	/* Struct containing program options/flags */
	static struct option longopts[] = {
		{"just-calc", no_argument, 0, 'c'},
		{"no-examples", no_argument, 0, 'e'},
		{"no-flags", no_argument, 0, 'f'},
		{"help", no_argument, 0, 'h'},
		{"no-cmp", no_argument, 0, 'm'},
		{"no-color", no_argument, 0, 'n'},
	 	{"rpn", no_argument, 0, 'r'},
	};

	int optind = 0;

	/* Check if flags have been passed */
	while ((optind = getopt_long(argc, argv, ":cefhmnr", longopts, &optind)) != 1) {
		switch (optind) {

		/* Enter "just-calculator" mode */
		case 'c':
			flags |= justcalc;
			flags &= ~usecolor;
			fprintf(stderr, "[Enabled just-calculator mode]\n");
			break;

		/* Don't show examples when printing help */
		case 'e':
			flags &= ~showsamp;
			fprintf(stderr, "[Disabled examples]\n");
			break;

		/* Don't show flags when printing help */
		case 'f':
			flags &= ~showflags;
			fprintf(stderr, "[Disabled flags]\n");
			break;

		/* Print help and exit */
		case '?': case 'h':
			printHelp();
			return 0;
			break;

		/* Don't show program compilation info */
		case 'm':
			flags &= ~showcmp;
			fprintf(stderr, "[Disabled compilation info]\n");
			break;

		/* Disable colored output */
		case 'n':
			flags &= ~usecolor;
			fprintf(stderr, "[Disabled colored output]\n");
			break;

		/* Enter rpn mode */
		case 'r':
			fprintf(stderr, "[Entered RPN mode (exit with CTRL+D)]\n");
			rpnInit();
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
		char *input;
		if ((flags & usecolor) != 0)
			input = readline("\e[1;4mcalc>\e[0m ");
		else
			input = readline("calc>\e[0m ");

		/* Parse the input */
		parseInput(input);

		/* Free input */
		free(input);
	}

	/* If this point is somehow reached, exit gracefully */
	return 0;
}

/* Print a result, based on the parsed string */
void
calculate(double first, char *operand, double second)
{
	/* Make the output bold, if we are using colors,
	 * and if we aren't in just-calc mode */
	if ((flags & usecolor) != 0 && !(flags & justcalc))
		printf("\e[1m");

	/* Check the operand and print the result */
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

	/* Reset the colors, if we are using them,
	 * and if we aren't in just-calc mode */
	if ((flags & usecolor) != 0 && !(flags & justcalc))
		printf("\e[0m");
}

/* Clear the screen */
void
clearScr(void)
{
	/* Run "clear" if the user is using Unix/a Unix-Like OS */
	#ifdef __unix__
	int sysretval = system("clear");

	/* Run "cls" if the user is using windows */
	#elif defined _MSC_VER
	int sysretval = system("cls");
	#endif

	/* Check if command above failed */
	if (sysretval == -1)
		fprintf(stderr, "Warning: unable to clear screen!\n");
}

/* Parse user input */
void
parseInput(char *input)
{
	/* Enter just-calculator mode, if we aren't already inside */
	if (!strcasecmp(input, "calc") && !(flags & justcalc)) {
		flags |= justcalc;
		fprintf(stderr, "[Entered just-calculator mode]\n");

	/* Clear the screen */
	} else if (!strcasecmp(input, "clear"))
		clearScr();

	/* Enable colors, if we aren't in just-calc mode */
	else if (!strcasecmp(input, "color") && !(flags & justcalc)) {
		flags |= usecolor;
		fprintf(stderr, "[Enabled color]\n");

	#if DEBUG != 0
	/* Debug info (hidden if DEBUG macro is not 0) */
	} else if (!strcasecmp(input, "debug")) {
		char archNumStr[10];

		if (ARCHNUM == 1)
			strncpy(archNumStr, "x86", 4);
		else if (ARCHNUM == 2)
			strncpy(archNumStr, "ARM", 4);
		else
			strncpy(archNumStr, "Unknown", 8);

		/* Firstly, print if we are in just-calculator mode */
		fprintf(stderr, "Just-Calculator Mode: %d (yes: 1, no: 0)\n",
				(!(flags & justcalc) ? 0 : 1));

		/* Print architecture, compiler, date & time info */
		fprintf(stderr, "Architecture, Compiler, Date & Time.\n"
				"ARCH: %s\n"
				"ARCHNUM: %d (%s)\n"
				"CC: %s %s\n"
				"DATE: %s\n"
				"TIME: %s\n\n",
			ARCH, ARCHNUM, archNumStr, CC, OPTS, __DATE__, __TIME__);

		/* Print the size of data types */
		fprintf(stderr, "Data Type Size.\n"
				"(char) size: %ld\n"
				"(char *) size: %ld\n"
				"(int) size: %ld\n"
				"(int *) size: %ld\n\n",
			sizeof(char), sizeof(char *), sizeof(int), sizeof(int *));

		/* Print POSIX C source, last modify date */
		fprintf(stderr, "Miscellaneous.\n"
				"POSIX C Source: %ld\n"
				"Calc version (Major): %s\n"
				"Calc version (Minor): %s\n"
				"Calc version (Full): %s\n\n",
			_POSIX_C_SOURCE, __CALC_VERSION_MAJOR, __CALC_VERSION_MINOR, __CALC_VERSION);

		/* Print opval */
		fprintf(stderr, "Opval.\n"
				"Hex:\t\t%#lx\n"
				"Decimal:\t%ld\n",
			__CALC_OPVAL, __CALC_OPVAL);
	#endif

	/* Exit without errors */
	} else if (!strcasecmp(input, "exit") || !strcasecmp(input, "quit"))
		exit(0);

	/* Enable examples in help section,
	 * if we aren't in just-calc mode */
	else if (!strcasecmp(input, "examples") && !(flags & justcalc)) {
		flags |= showsamp;
		fprintf(stderr, "[Enabled examples]\n");

	/* Show flags, if we aren't in just-calc mode */
	} else if (!strcasecmp(input, "flags") && !(flags & justcalc)) {
		flags |= showflags;
		fprintf(stderr, "[Enabled flags]\n");

	/* Print this program's help */
	} else if (!strcasecmp(input, "help"))
		printHelp();

	/* Print available operands,
	 * if we aren't in just-calc mode */
	else if ((!strcasecmp(input, "operands") || !strcasecmp(input, "ops")) && !(flags & justcalc))
		printOps();

	/* Exit out of just-calculator mode, if we're inside */
	else if (!strcasecmp(input, "nocalc") && ((flags & justcalc) != 0)) {
		flags &= ~justcalc;
		fprintf(stderr, "[Disabled just-calculator mode]\n");

	/* Disable color, if we
	 * aren't in just-calc mode */
	} else if (!strcasecmp(input, "nocolor") && !(flags & justcalc)) {
		flags &= ~usecolor;
		fprintf(stderr, "[Disabled color]\n");

	/* Don't show examples, if we
	 * aren't in just-calc mode */
	} else if (!strcasecmp(input, "noexamples") && !(flags & justcalc)) {
		flags &= ~showsamp;
		fprintf(stderr, "[Disabled examples]\n");

	/* Don't show flags, if we
	 * aren't in just-calc mode */
	} else if (!strcasecmp(input, "noflags") && !(flags & justcalc)) {
		flags &= ~showflags;
		fprintf(stderr, "[Disabled flags]\n");

	/* Enter RPN mode, regardless of our mode */
	} else if (!strcasecmp(input, "rpn")) {
		fprintf(stderr, "[Entered RPN mode (exit with CTRL+D)]\n");
		rpnInit();

	/* Print special values, if we
	 * aren't in just-calc mode */
	} else if (!strcasecmp(input, "specvals") && !(flags & justcalc))
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
		char operand[2];

		/* Convert operand value to long unsigned int */
		uint64_ct opnum = (uint64_ct)array[1];

		#if DEBUG_OPVAL != 0
		fprintf(stderr, "[DEBUG] Opval: %ld, %#lx\n", __CALC_OPVAL, __CALC_OPVAL);
		#endif

		/* Check if operand exists, using a magic number (this somehow works) */
		#ifdef ARCH_ARM
		if (opnum > __CALC_OPVAL) {
		#else
		if (opnum < __CALC_OPVAL) {
		#endif
			strncpy(operand, array[1], 1);
			calculate(first, operand, second);
		} else {
			/* Operand does not exist, print only the first number (10 decimal places) */
			if ((flags & usecolor) != 0 && !(flags & justcalc))
				printf("\e[1m%.10f\e[0m\n", first);
			else
				printf("%.10f\n", first);
		}
		/* End TODO */

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
	/* Print program info */
	printf("Basic Calculator by Salonia Matteo, made on 25/01/2021, version %s\n", __CALC_VERSION);

	/* Show program compilation info,
	 * if we aren't in just-calc mode */
	if ((flags & showcmp) != 0 && !(flags & justcalc)) {
		printf("Compiled on %s at %s %s, "
			"using compiler %s, targeting platform %s, "
			"operating system %s.\n", __DATE__, __TIME__, OPTS, CC, ARCH, OS);
	}

	/* Show flags using color, if color is enabled, flags
	 * are enabled, and we aren't in just-calc mode */
	if ((flags & (showflags | usecolor)) == (showflags | usecolor) && !(flags & justcalc)) {
		printf("Flags:\n"
		"%s \t| %s \tEnter \"just-calculator\" mode\n"
		"%s \t| %s \tDon't show examples\n"
		"%s \t| %s \tDon't show these flags\n"
		"%s \t\t| %s \tShow this help\n"
		"%s \t| %s \tDon't show program compilation info\n"
		"%s \t| %s \tDon't color the output\n"
		"%s \t\t| %s \tEnter \"RPN\" mode (Reverse Polish Notation)\n",
color_rvid("--just-calc"), color_rvid("-c"),
color_rvid("--no-examples"), color_rvid("-e"),
color_rvid("--no-flags"), color_rvid("-f"),
color_rvid("--help"), color_rvid("-h"),
color_rvid("--no-cmp"), color_rvid("-m"),
color_rvid("--no-color"), color_rvid("-n"),
color_rvid("--rpn"), color_rvid("-r"));

	/* Show flags without colors, if color is disabled, flags
	 * are enabled, and we aren't in just-calc mode */
	} else if ((flags & showflags) != 0 && !(flags & usecolor) && !(flags & justcalc)) {
		printf("Flags:\n"
		"--just-calc \t| -c \tEnter \"just-calculator\" mode\n"
		"--no-examples \t| -e \tDon't show examples\n"
		"--no-flags \t| -f \tDon't show these flags\n"
		"--help \t\t| -h \tShow this help\n"
		"--no-cmp \t| -m \tDon't show program compilation info\n"
		"--no-color \t| -n \tDon't color the output\n"
		"--rpn \t\t| -r \tEnter \"RPN\" mode (Reverse Polish Notation)\n");
	}

	/* Show available commands with colors, if color
	 * is enabled, and we aren't in just-calc mode */
	if (!(flags & justcalc) && (flags & usecolor) == usecolor) {
		printf("\nAvailable commands: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s (or %s), %s, %s, %s.\n",
			color_rvid("calc"), color_rvid("clear"), color_rvid("color"), color_rvid("examples"),
			color_rvid("exit"), color_rvid("flags"), color_rvid("help"),
			color_rvid("nocolor"), color_rvid("noexamples"), color_rvid("noflags"),
			color_rvid("operands"), color_rvid("ops"), color_rvid("quit"),
			color_rvid("rpn"), color_rvid("specvals"));

	/* Show available commands without colors, if color
	 * is disabled, and we aren't in just-calc mode */
	} else if (!(flags & usecolor) && !(flags & justcalc)) {
		printf("\nAvailable commands: calc, clear, color, examples, "
			"exit, flags, help, nocolor, noexamples, noflags, "
			"operands (or ops), quit, rpn, specvals.\n");

	/* Show reduced command list without colors,
	 * if we aren't in just-calc mode */
	} else if ((flags & justcalc) == justcalc) {
		printf("\n(Just-calculator mode)\nAvailable commands: clear, exit, help, quit, nocalc, rpn.\n");
	}

	/* Show examples with colors, if color is
	 * enabled, and we aren't in just-calc mode */
	if ((flags & (showsamp | usecolor)) == (showsamp | usecolor) && !(flags & justcalc)) {
		printf("Examples:\n"
			"%s\n"
			"1 + 1\t\t1 p 1\t\tAddition\tReturns 2\n"
			"1 - 1\t\t1 s 1\t\tSubtraction\tReturns 0\n"
			"2 * 2\t\t2 t 2\t\tMultiplication\tReturns 4\n"
			"4 / 2\t\t4 d 2\t\tDivision\tReturns 2\n"
			"4 %% 2\t\t4 m 2\t\tModulus\t\tReturns 0\n"
			"1 < 16\t\t1 l 16\t\tBit-shifting\tReturns 65536\n"
			"4096 > 1\t4096 r 1\tBit-shifting\tReturns 2048\n",
			color_bu("[Cmd]\t\t[Alt sign]\t[Description]\t[Result]"));

	/* Show examples without colors, if color is 
	 * disabled, and we aren't in just-calc mode */
	} else if ((flags & showsamp) != 0 && !(flags & usecolor) && !(flags & justcalc)) {
		printf("Examples:\n"
			"[Cmd]\t\t[Alt sign]\t[Description]\t[Result]\n"
			"1 + 1\t\t1 p 1\t\tAddition\tReturns 2\n"
			"1 - 1\t\t1 s 1\t\tSubtraction\tReturns 0\n"
			"2 * 2\t\t2 t 2\t\tMultiplication\tReturns 4\n"
			"4 / 2\t\t4 d 2\t\tDivision\tReturns 2\n"
			"4 %% 2\t\t4 m 2\t\tModulus\t\tReturns 0\n"
			"1 < 16\t\t1 l 16\t\tBit-shifting\tReturns 65536\n"
			"4096 > 1\t4096 r 1\tBit-shifting\tReturns 2048\n");
	}

}

/* Print available operands and their short notation */
void
printOps(void)
{
	/* Show operands with color, if color is
	 * enabled, and we aren't in just-calc mode */
	if ((flags & usecolor) != 0 && !(flags & justcalc)) {
		printf("Available operands:\n"
			"%s Can be written as %s\n"
			"+\t\t\t\tp\n"
			"-\t\t\t\ts\n"
			"*\t\t\t\tt\n"
			"/\t\t\t\td\n"
			"%%\t\t\t\tm\n"
			"<\t\t\t\tl\n"
			">\t\t\t\tr\n",
			color_bu("[Symbol]"), color_bu("[Latin letter]"));

	/* Show operands without color, if color is
	 * disabled, and we aren't in just-calc mode */
	// if usecolor = 0 && justcalc = 0
	} else if ((flags & (usecolor | justcalc)) == (usecolor | justcalc)) {
		printf("Available operands:\n"
			"[Symbol] Can be written as [Latin letter]\n"
			"+\t\t\t\tp\n"
			"-\t\t\t\ts\n"
			"*\t\t\t\tt\n"
			"/\t\t\t\td\n"
			"%%\t\t\t\tm\n"
			"<\t\t\t\tl\n"
			">\t\t\t\tr\n");
	}
}

/* Print special values */
void
printSpecVals(void)
{
	/* Print special values with color, if color is
	 * enabled, and we aren't in just-calc mode */
	if ((flags & usecolor) != 0 && !(flags & justcalc)) {
		printf("Special values: you can type these words to automatically get their value.\n"
			"%s These are case insensitive, so you can type them in all lowercase, uppercase, etc.\n"
			"%s\t%s\n"
			"pi\t\tThe value of Pi\n"
			"pi2\t\tPi / 2\n"
			"pi4\t\tPi / 4\n"
			"1pi\t\t1 / Pi\n"
			"2pi\t\t2 / Pi\n"
			"pisq\t\tPi * Pi\n"
			"e\t\tThe value of e\n",
			color_bu("NOTE:"), color_bu("[Symbol]"), color_bu("[Description]"));

	/* Print special values without color, if color is
	 * disabled, and we aren't in just-calc mode */
	} else if ((flags & (usecolor | justcalc)) == (usecolor | justcalc)) {
		printf("Special values: you can type these words to automatically get their value.\n"
			"NOTE: These are case insensitive, so you can type them in all lowercase, uppercase, etc.\n"
			"[Symbol]\t[Description]\n"
			"pi\t\tThe value of Pi\n"
			"pi2\t\tPi / 2\n"
			"pi4\t\tPi / 4\n"
			"1pi\t\t1 / Pi\n"
			"2pi\t\t2 / Pi\n"
			"pisq\t\tPi * Pi\n"
			"e\t\tThe value of e\n");
	}
}

/* Handle signals */
void
sigHandler(int sigNum)
{
	/* Inform user what signal was sent */
	char sigName[10] = {0};

	/* Check if signal is CTRL+C */
	if (sigNum == 2)
		strncpy(sigName, "(CTRL+C)", 9);
	/* Check if signal is CTRL+D */
	else if (sigNum == 11)
		strncpy(sigName, "(CTRL+D)", 9);
	else
		strncpy(sigName, "", 0);

	/* Print detected signal and exit gracefully */
	fprintf(stderr, "[Detected Signal %d %s]\n", sigNum, sigName);

	/* Set variable to 0 */
	memset(&sigNum, 0, sizeof(sigNum));

	/* Free allocated memory to prevent memory leaks */
	if (_free_color() != 0)
		fprintf(stderr, "WARNING! Failed to free coloredStr!\n");

	exit(0);
}

