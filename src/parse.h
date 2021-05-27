/* See LICENSE file for copyright and license details.
 *
 * This header, parse.h, parses a string, and outputs
 * the result; supports parentheses, powers,
 * bit-shifting (>> and <<), multiplication, division,
 * addition and subtraction.
 *
 * Usage:
 * parse(str);
 * build_ast(&token_head);
 * uint64_t result = solve(token_head.next);
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 *
 */

#ifndef PARSE_H
#define PARSE_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_GMP_H
#include <gmp.h>
#else
#include <stdint.h>
#endif

typedef enum {
	VAL,
	ADD,			// +
	SUB,			// -
	MUL,			// *
	DIV,			// /
	L_SHIFT,		// <
	R_SHIFT,		// >
	POWER,			// ^
	MOD,			// %
	PAREN_OPEN,		// (
	PAREN_CLOSE,		// )
	FUNCTION,		// +-/* are also functions a.k.a unary operators
	END			// Used in template as terminating symbol
} NODE_TYPE;

/* Token structure */
static struct token {
	NODE_TYPE node_type;
	uint64_t value;
	struct token *next;
	struct token *left;
	struct token *right;
	bool reduced;
} token_head;

/* Available templates */
static NODE_TYPE add_template[] = { VAL, ADD, VAL, END };
static NODE_TYPE sub_template[] = { VAL, SUB, VAL, END };
static NODE_TYPE mul_template[] = { VAL, MUL, VAL, END };
static NODE_TYPE div_template[] = { VAL, DIV, VAL, END };
static NODE_TYPE lshift_template[] = { VAL, L_SHIFT, VAL, END };
static NODE_TYPE rshift_template[] = { VAL, R_SHIFT, VAL, END };
static NODE_TYPE power_template[] = { VAL, POWER, VAL, END };
static NODE_TYPE mod_template[] = { VAL, MOD, VAL, END };
static NODE_TYPE paren_template[] = { PAREN_OPEN, VAL, PAREN_CLOSE, END };
static NODE_TYPE unary_template[] = { FUNCTION, VAL, END };

/* Possible unary operators.
 * If operator is not in this list,
 * then it's not available as unary operator */
static NODE_TYPE unary_whitelist[] = { ADD, SUB, END };

/* Function prototypes */
static struct token *token_add(NODE_TYPE);
static struct token *token_add_val(uint64_t);
static bool matches_template(struct token *, NODE_TYPE *);
static struct token *reduce(struct token *);
static struct token *unwrap(struct token *);
static struct token *unwrap_unary(struct token *);
uint64_t solve(struct token *);
void build_ast(struct token *);
bool parse(char *);

/* Add token */
static struct token *
token_add(NODE_TYPE token)
{
	struct token *curr = &token_head;

	while (curr->next != NULL)
		curr = curr->next;

	struct token *n = malloc(sizeof(struct token));

	n->node_type = token;
	n->left = NULL;
	n->right = NULL;
	n->next = NULL;
	n->reduced = false;
	curr->next = n;

	return n;
}

/* Add value to token */
static struct token *
token_add_val(uint64_t val)
{
	struct token *constructed = token_add(VAL);

	constructed->value = val;
	return constructed;
}

/* Check if token matches template */
static bool
matches_template(struct token *head, NODE_TYPE * tmpl)
{
	int i = 0;
	NODE_TYPE curr_token = tmpl[i];

	while (curr_token != END) {
		if (head == NULL)
			return false;

		NODE_TYPE head_type = head->node_type;

		/* Reduced node is considered a value,
		 * but we can't override it's operator */
		if (head->reduced)
			head_type = VAL;

		bool in_unary_whitelist = false;
		int white_list_iterator = 0;

		while (unary_whitelist[white_list_iterator] != END) {
			if (unary_whitelist[white_list_iterator] == head_type) {
				in_unary_whitelist = true;
				break;
			}
			white_list_iterator++;
		}

		if (curr_token == FUNCTION && head_type != VAL
		    && in_unary_whitelist)
			curr_token = head_type;

		if (head == NULL || head_type != curr_token)
			return false;

		i++;
		head = head->next;
		curr_token = tmpl[i];
	}
	return true;
}

/* Reduce expressions */
static struct token *
reduce(struct token *head)
{
	struct token *combined = head->next->next;

	combined->reduced = true;
	combined->left = head->next;
	combined->right = combined->next;

	if (head->next->next->next->next != NULL)
		combined->next = head->next->next->next->next;
	else
		combined->next = NULL;

	head->next = combined;

	return head;
}

/* Unfold (VAL) into VAL */
static struct token *
unwrap(struct token *head)
{
	struct token *combined = head->next->next;

	combined->reduced = true;

	if (head->next->next->next->next != NULL)
		combined->next = head->next->next->next->next;
	else
		combined->next = NULL;

	head->next = combined;

	return head;
}

/* Unfold OP VAL into VAL based on OP. */
static struct token *
unwrap_unary(struct token *head)
{

	struct token *combined = head->next->next;
	NODE_TYPE op = head->next->node_type;

	if (op == SUB)
		combined->value = 0 - combined->value;

	combined->reduced = true;

	if (head->next->next->next != NULL)
		combined->next = head->next->next->next;
	else
		combined->next = NULL;

	head->next = combined;

	return head;
}

/* Solve AST branch */
uint64_t
solve(struct token *head)
{
	NODE_TYPE op = head->node_type;

	if (head->left == NULL && head->right == NULL)
		return head->value;

	if (op == ADD)
		return solve(head->left) + solve(head->right);
	else if (op == SUB)
		return solve(head->left) - solve(head->right);
	else if (op == MUL)
		return solve(head->left) * solve(head->right);
	else if (op == DIV)
		return solve(head->left) / solve(head->right);
	else if (op == L_SHIFT)
		return solve(head->left) << solve(head->right);
	else if (op == R_SHIFT)
		return solve(head->left) >> solve(head->right);
	else if (op == POWER)
		return pow(solve(head->left), solve(head->right));
	else if (op == MOD)
		return solve(head->left) % solve(head->right);
	else {
		fprintf(stderr, "Unsupported operation: \"%d\"\n", op);
		return -1;
	}
}

/* Build AST */
void
build_ast(struct token *head)
{
	struct token *curr = head;

	while (head->next->next != NULL) {
		/* Parentheses */
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, paren_template))
				curr = unwrap(curr);
			else
				curr = curr->next;
		}

		/* Multiplication and division */
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, mul_template)
			    || matches_template(curr->next, div_template))
				curr = reduce(curr);
			else
				curr = curr->next;
		}

		/* Addition and subtraction */
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, add_template)
			    || matches_template(curr->next, sub_template))
				curr = reduce(curr);
			else
				curr = curr->next;
		}

		/* Left and Right shift */
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, lshift_template)
			    || matches_template(curr->next, rshift_template))
				curr = reduce(curr);
			else
				curr = curr->next;
		}

		/* Power and Modulus */
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, power_template)
			    || matches_template(curr->next, mod_template))
				curr = reduce(curr);
			else
				curr = curr->next;
		}

		/* Unary operators */
		curr = head;
		while (curr->next != NULL) {
			if (matches_template(curr->next, unary_template))
				curr = unwrap_unary(curr);
			else
				curr = curr->next;
		}

	}
}

/* Parse string */
bool
parse(char *code)
{
	int last_numeric_idx = -1;

	for (int i = 0, len = strlen(code); i <= len; i++) {
		char c = code[i];

		if (c == ' ')
			continue;

		if (c >= '0' && c <= '9') {
			if (last_numeric_idx == -1)
				last_numeric_idx = i;
		} else {
			if (last_numeric_idx != -1) {
				int size = i - last_numeric_idx;

				char *buffer = malloc(size);

				strncpy(buffer, (code + last_numeric_idx),
					size);
				buffer[size] = '\0';

				token_add_val(atol(buffer));
				last_numeric_idx = -1;

				free(buffer);
			}

			/* Check operands */
			if (c == '+')
				token_add(ADD);
			else if (c == '-')
				token_add(SUB);
			else if (c == '*')
				token_add(MUL);
			else if (c == '/')
				token_add(DIV);
			else if (c == '<')
				token_add(L_SHIFT);
			else if (c == '>')
				token_add(R_SHIFT);
			else if (c == '^')
				token_add(POWER);
			else if (c == '%')
				token_add(MOD);
			else if (c == '(')
				token_add(PAREN_OPEN);
			else if (c == ')')
				token_add(PAREN_CLOSE);
			else if (c != ' ' && c != 0 && i > 0) {
				fprintf(stderr, "Unsupported token: \"%c\"\n",
					c);
				return false;
			}
		}
	}

	return true;
}

#endif
