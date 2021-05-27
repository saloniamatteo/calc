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

#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <readline/readline.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_BUGREPORT "saloniamatteo@pm.me"
#define PACKAGE_DONATE "https://saloniamatteo.top/donate.html"
#define PACKAGE_STRING "calc 2.1"
#endif

#include "color.h"
#include "rpn.c"
#include "parse.h"

/* Instead of constantly writing color(string, 1, rvideo),
 * only write color_rvid(string) */
#define color_rvid(str) color(str, 1, rvideo)

/* Instead of constantly writing color(string, 2, bold, underline),
 * only write color_bu(string) */
#define color_bu(str) color(str, 2, bold, underline)

/* Define "long unsigned int" as uint64_ct */
typedef long unsigned int uint64_ct;

/* Flags */
enum calc_flags {
	usecolor = 1 << 0,	/* Print colored output? (Default: yes) */
	showflags = 1 << 1,	/* Print program flags? (Default: yes) */
	showsamp = 1 << 2,	/* Print examples? (Default: yes) */
	justcalc = 1 << 3,	/* Enter just-calculator mode? (Default: no (0); yes (1)) */
};

static const char *JC_CMDS[] = {
	"clear", "exit", "help", "quit", "nocalc", "rpn", NULL
};
static const char *NORM_CMDS[] = {
    	"calc", "clear", "color", "examples", "exit", "flags",
	"help", "nocolor", "noexamples", "noflags", "operands",
	"ops", "quit", "rpn",
	NULL
};

static const char *FLAGS[] = {
	"--just-calc, -c",	"Enter just-calculator mode",
	"--no-examples, -e",	"Don't show examples",
	"--no-flags, -f",	"\tDon't show these flags",
	"--help, -h",		"\tShow this help",
	"--no-color, -n",	"\tDon't color the output",
	"--rpn, -r",		"\tEnter RPN mode (Reverse Polish Notation)",
	NULL
};

static const char *EXAMPLE_HEAD[] = {
	"[Cmd]\t\t[Description]\t\t[Result]", NULL
};
static const char *EXAMPLE_STR[] = {
	"1 + 1\t",	"\tAddition\t",			"\tReturns 2\n",
	"1 - 1\t",	"\tSubtraction\t",		"\tReturns 0\n",
	"2 * 2\t",	"\tMultiplication\t",		"\tReturns 4\n",
	"4 / 2\t",	"\tDivision\t",			"\tReturns 2\n",
	"4 %% 2\t",	"\tModulus\t",			"\t\tReturns 0\n",
	"1 < 16\t",	"\tLeft Bit-shifting\t",	"Returns 65536\n"
	"128 > 1\t",	"\tRight Bit-shifting\t",	"Returns 64\n",
	NULL
};

static const char *OPS_HEAD[] = {
	"[Symbol]\t[Description]",
	NULL
};

static const char *OPS_STR[] = {
	"+",	"Plus operator: add 2 numbers.\n",
	"-",	"Minus operator: subtract 2 numbers.\n",
	"*",	"Multiplication operator: multiply 2 numbers.\n",
	"/",	"Division operator: divide 2 numbers.\n",
	"<",	"Left bit-shift operator: shift bits to the left.\n",
	">",	"Right bit-shift operator: shift bits to the right.\n",
	"^",	"Power operator: multiply a number by itself.\n",
	"%",	"Modulus operator: get the remainder of a division.\n",
	NULL
};

static unsigned short int flags = 0;	/* Used to store flags */

/* Function prototypes */
static void clearScr(void);
static void parseInput(char *);
static void printHelp(void);
static void printOps(void);
static void sigHandler(int);

/* Clear the screen */
static void
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
static void
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
	else if ((!strcasecmp(input, "operands") || !strcasecmp(input, "ops"))
		 && !(flags & justcalc))
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

	/* Parse the entered string */
	} else {
		if (parse(input)) {
			build_ast(&token_head);
			uint64_t result = solve(token_head.next);

			if (result != 1) {
				if ((flags & usecolor) != 0
				    && !(flags & justcalc))
					printf("\e[1m%ld\e[0m\n", result);
				else
					printf("%ld\n", result);
			}
		}
	}
}

/* Print this program's help */
static void
printHelp(void)
{
	/* Print program info */
	printf("Welcome to %s by Salonia Matteo. Report any bugs to %s.\n"
	       "Support this project: %s\n",
	       PACKAGE_STRING, PACKAGE_BUGREPORT, PACKAGE_DONATE);

	/* Show flags using color, if color is enabled, flags
	 * are enabled, and we aren't in just-calc mode */
	if ((flags & showflags) == showflags && !(flags & justcalc)) {
		printf("\nFlags:\n");

		for (uint64_t i = 0; i < sizeof(FLAGS); i++) {
			if (FLAGS[i] != NULL) {
				if ((i % 2) == 0) {
					/* Check if we have to use colors */
					if ((flags & usecolor) == usecolor)
						printf("\e[1m%s:\e[0m",
						       FLAGS[i]);
					else
						printf("%s:", FLAGS[i]);
				} else
					printf("\t%s\n", FLAGS[i]);
			} else
				break;
		}
	}

	/* Show available commands, if we aren't in just-calc mode */
	if (!(flags & justcalc)) {
		printf("\nAvailable commands:\n");

		for (uint64_t i = 0; i < sizeof(NORM_CMDS); i++) {
			if (NORM_CMDS[i] != NULL) {
				/* Check if we have to show colors */
				if ((flags & usecolor) == usecolor)
					printf("%s",
					       color_rvid((char *)
							  NORM_CMDS[i]));
				else
					printf("%s", NORM_CMDS[i]);

				if (NORM_CMDS[i + 1] != NULL)
					printf(", ");
				else
					printf(".\n");
			} else
				break;
		}
	}

	/* Show reduced command list, if we are in just-calc mode */
	if ((flags & justcalc) == justcalc) {
		printf("\nAvailable commands:\n");

		for (uint64_t i = 0; i < sizeof(JC_CMDS); i++) {
			if (JC_CMDS[i] != NULL) {
				printf("%s", JC_CMDS[i]);

				if (JC_CMDS[i + 1] != NULL)
					printf(", ");
				else
					printf(".\n");
			} else
				break;
		}
	}

	/* Show examples, if we aren't in just-calc mode */
	if ((flags & showsamp) == showsamp && !(flags & justcalc)) {
		printf("\nExamples:\n");

		/* Check if we have to show colors */
		if ((flags & usecolor) == usecolor)
			puts(color_bu((char *)EXAMPLE_HEAD[0]));
		else
			puts((char *)EXAMPLE_HEAD[0]);

		for (uint64_t i = 0; i < sizeof(EXAMPLE_STR); i++) {
			if (EXAMPLE_STR[i] != NULL) {
				printf("%s", EXAMPLE_STR[i]);
			} else
				break;
		}
	}
}

/* Print available operands and their short notation */
static void
printOps(void)
{
	/* Show operands, if we aren't in just-calc mode */
	if (!(flags & justcalc)) {
		printf("Available operands:\n");

		/* Check if we have to use colors */
		if ((flags & usecolor) == usecolor)
			puts(color_bu((char *)OPS_HEAD[0]));
		else
			puts((char *)OPS_HEAD[0]);

		for (uint64_t i = 0; i < sizeof(OPS_STR); i++) {
			if (OPS_STR[i] != NULL) {
				if ((i % 2) == 0) {
					/* Check if we have to use colors */
					if ((flags & usecolor) == usecolor)
						printf("\e[1m%s\e[0m",
						       OPS_STR[i]);
					else
						printf("%s", OPS_STR[i]);
				} else
					printf("\t\t%s", OPS_STR[i]);
			} else
				break;
		}
	}
}

/* Handle signals */
static void __attribute__((noreturn))
sigHandler(int sigNum)
{
	/* Inform user what signal was sent */
	char sigName[10] = { 0 };

	/* Check if signal is CTRL+C */
	if (sigNum == 2)
		strncpy(sigName, "(CTRL+C)", 9);
	/* Check if signal is CTRL+D */
	else if (sigNum == 11)
		strncpy(sigName, "(CTRL+D)", 9);

	/* Print detected signal and exit gracefully */
	fprintf(stderr, "[Detected Signal %d %s]\n", sigNum, sigName);

	/* Set variable to 0 */
	memset(&sigNum, 0, sizeof(sigNum));

	/* Free allocated memory to prevent memory leaks */
	if (_free_color() != 0)
		fprintf(stderr, "WARNING! Failed to free coloredStr!\n");

	exit(0);
}

int
main(int argc, char **argv)
{
	/* Initialise flags */
	/* Print colored output? (Default: yes) */
	flags |= usecolor;

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
		{"no-color", no_argument, 0, 'n'},
		{"rpn", no_argument, 0, 'r'},
	};

	int ind = 0;

	/* Check if flags have been passed */
	while ((ind = getopt_long(argc, argv, ":cefhnr", longopts, &ind)) != 1) {
		switch (ind) {

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
		case '?':
		case 'h':
			printHelp();
			return 0;
			break;

			/* Disable colored output */
		case 'n':
			flags &= ~usecolor;
			fprintf(stderr, "[Disabled colored output]\n");
			break;

			/* Enter rpn mode */
		case 'r':
			fprintf(stderr,
				"[Entered RPN mode (exit with CTRL+D)]\n");
			rpnInit();
			break;
		}

		if (ind <= 0)
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
