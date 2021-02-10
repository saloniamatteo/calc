/* See LICENSE file for copyright and license details.
 *
 * calc is a Simple Calculator written in C by Salonia Matteo. It
 * takes input from stdin using libreadline, and prints the
 * result from the requested operation to stdout.
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "calc.h"
#include "optimizations.h"
#include "compiler.h"

/* Use POSIX.1-2008 */
#define _POSIX_C_SOURCE 200809L

int
main(int argc, char *argv[])
{

	/* Print program info */
	printHelp();

	/* Infinite loop */
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

		/* Parse the input, and identify what to do */
		parseInput(input);
	}

	/* If this point is somehow reached, exit without errors */
	return 0;
}
