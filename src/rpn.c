/* See LICENSE file for copyright and license details.
 *
 * calc is a Simple Calculator written in C by Salonia Matteo.
 * It takes input from stdin using libreadline, and prints the
 * result from the requested operation to stdout.
 *
 * This file, rpn.c, *does not* use libreadline to take input;
 * Instead, it uses fgetc.
 *
 * This file is used to parse an RPN string, aka Reverse Polish Notation.
 *
 * Example:
 * 	((1 + 2) + (3 + 4)) * 2
 * 	becomes 1 2 + 3 4 + + 2 *
 * 	both should output 20.
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFSIZE 100	/* size of buffer for getch/ungetch */
#define MAXVAL 100	/* maximum depth of val stack */
#define MAXOP 100	/* max size of operand or operator */
#define NUMBER '0'	/* signal that a number was found */

#define rpnErr(err) fprintf(stderr, "RPN: %s", err);

/* Global variables */
static char buf[BUFSIZE];	/* buffer for ungetch */
static int bufp = 0;		/* next free position in buf */
static int sp = 0;		/* next free stack position */
static double val[MAXVAL];	/* value stack */

/* Function prototypes */
static int getch(void);
static void ungetch(int);
static int getop(char []);
static void push(double);
static double pop(void);

/* Function declarations */
/* getch: get a (possibly pushed back) character */
static int
getch(void)
{
	return (bufp > 0) ? buf[--bufp] : fgetc(stdin);
}

/* ungetch: push character back on input */
static void
ungetch(int c)
{
	if (bufp >= BUFSIZE) {
		rpnErr("Ungetch Error: Too Many Characters!\n");
	} else {
		buf[bufp++] = c;
	}
}

/* getop: get next operator or numeric operand */
static int
getop(char s[])
{
	int i, c;

	while ((s[0] = c = getch()) == ' ' || c == '\t')
		;
	s[1] = '\0';

	if (!isdigit(c) && c != '.')
		return c;	/* not a number */
	i = 0;

	if (isdigit(c))		/* collect integer part */
		while (isdigit(s[++i] = c = getch()))
				;

	if (c == '.')		/* collect fraction part */
		while (isdigit(s[++i] = c = getch()))	
			;
	s[i] = '\0';

	if (c != EOF)
		ungetch(c);

	return NUMBER;
}

/* push: push f onto value stack */
static void
push(double f)
{
	if (sp < MAXVAL)
		val[sp++] = f;
	else
		rpnErr("Error: Stack is Full!\n");
}

/* pop: pop and return top value from stack */
static double
pop(void)
{
	if (sp > 0)
		return val[--sp];
	else {
		rpnErr("Error: Stack is Empty!\n");
		return 0.0;
	}
}

/* rpmInit: enter RPM mode */
int
rpnInit(void)
{
	int type;
	double op2;
	char s[MAXOP];

	while ((type = getop(s)) != EOF) {
		switch(type) {
		case NUMBER:
			push(atof(s));
			break;
		case '+':
			push(pop() + pop());
			break;
		case '*':
			push(pop() * pop());
			break;
		case '-':
			op2 = pop();
			push(pop() - op2);
			break;
		case '/':
			op2 = pop();
			if (op2 != 0.0)
				push(pop() / op2);
			else
				rpnErr("Error: Division by Zero!\n");
			break;
		case '\n':
			printf("\t%.8g\n", pop());
			break;
		default:
			rpnErr("Error: Could not Parse String!\n");
			break;
		}
	}
	return 0;
}
