#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "calc.h"
#include "optimizations.h"
#include "compiler.h"

/* Basic Calculator by Salonia Matteo */

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
