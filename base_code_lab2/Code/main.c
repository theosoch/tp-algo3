// Need this to use the getline C function on Linux. Works without this on MacOs. Not tested on Windows.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "token.h"
#include "queue.h"
#include "stack.h"

// 

#define LINE_LENGTH_MAX FILENAME_MAX

#define POSITIVE_SIGNED_LONG_MAX_VALUE = ((256*256*256*256)/2)-1

// 

/**
 * @brief Indicates if a given char is c
 * 
 * @param c 
 * @return true 
 * @return false 
 */
bool isSymbol(const char c) {
	switch(c) {
		case '+': return true;
		case '-': return true;
		case '*': return true;
		case '/': return true;
		case '^': return true;
		case '(': return true;
		case ')': return true;
		default: return false;
	}
}

// 

/**
 * @brief Indicates if a given char is a digit (0, 1, ..., 9).
 * 
 * @param c 
 * @return true 
 * @return false 
 */
bool isDigit(const char c) {
	return '0' <= c && c <= '9';
}

// 

/**
 * @brief This is a dumpFunction used to print a token queue.
 * 
 * @param f 
 * @param e 
 */
void printToken(FILE* f, void* e) {
	Token* token = (Token*) e;

	if(tokenIsNumber(token))
		fprintf(f, "%f", tokenGetValue(token));
	else if(tokenIsOperator(token))
		fprintf(f, "%c", tokenGetOperatorSymbol(token));
	else if(tokenIsParenthesis(token))
		fprintf(f, "%c", tokenGetParenthesisSymbol(token));
	
	printf(" ");
}

// 

/**
 * @brief Convert a string expression to infix token queue expression.
 * 
 * @param expression 
 * @return Queue* 
 */
Queue* stringToTokenQueue(const char* expression) {
	Queue* q = createQueue();

	char* curpos = (char*) expression;

	char* current_number_pos;
	int current_number_length;

	while(*curpos != '\0') {
		if(*curpos != ' ' && *curpos != '\n') {
			Token* token = NULL;

			if(isSymbol(*curpos)) {
				token = createTokenFromString(curpos, 1);
			}
			else if(isDigit(*curpos)) {
				current_number_pos = curpos;
				current_number_length = 0;

				while(*(curpos) != '\0' && isDigit(*(curpos))) {
					curpos++;
					current_number_length++;
				}
				curpos--;

				token = createTokenFromString(current_number_pos, current_number_length);
			}

			assert(token != NULL);
			queuePush(q, token);
		}

		curpos++;
	}

	return q;
}

// 

/**
 * @brief Implementation of the Shutting Yard algorithm to convert infix token queue expression to a postfix one.
 * 
 * @param infix 
 * @return Queue* 
 */
Queue* shuntingYard(Queue* infix) {
	Queue* postfix = createQueue();
	Stack* operator_stack = createStack(0);

	Token* top_operator;

	while(!queueEmpty(infix)) {
		Token* token = (Token*) queueTop(infix);
		infix = queuePop(infix);

		if(tokenIsNumber(token)) queuePush(postfix, token);
		else if(tokenIsOperator(token)) {
			while(
				!stackEmpty(operator_stack)
				&& (
					!tokenIsParenthesis(top_operator = stackTop(operator_stack))
					|| tokenGetParenthesisSymbol(top_operator) != '('
				)
				&& (
					tokenGetOperatorPriority(top_operator) > tokenGetOperatorPriority(token)
					|| (
						tokenGetOperatorPriority(top_operator) == tokenGetOperatorPriority(token)
						&& tokenOperatorIsLeftAssociative(token)
					)
				)
			) {
				stackPop(operator_stack);
				queuePush(postfix, top_operator);
			}
			
			stackPush(operator_stack, token);
		}
		else if(tokenIsParenthesis(token)) {
			if(tokenGetParenthesisSymbol(token) == '(')
				stackPush(operator_stack, token);
			else {
				while(
					!stackEmpty(operator_stack)
					&& (
						!tokenIsParenthesis(top_operator = stackTop(operator_stack))
						|| tokenGetParenthesisSymbol(top_operator) != '('
					)
				) {
					stackPop(operator_stack);
					queuePush(postfix, top_operator);
				}

				bool left_parenthesis_found = false;
				while(!stackEmpty(operator_stack) && !left_parenthesis_found) {
					top_operator = stackTop(operator_stack);
					
					stackPop(operator_stack);
					left_parenthesis_found = tokenIsParenthesis(top_operator) && tokenGetParenthesisSymbol(top_operator) == '(';
					
					deleteToken(&top_operator);
				}
				assert(left_parenthesis_found);

				deleteToken(&token);
			}
		}
	}

	while(!stackEmpty(operator_stack)) {
		Token* top_operator = stackTop(operator_stack);
		assert(!tokenIsParenthesis(top_operator));

		stackPop(operator_stack);
		queuePush(postfix, top_operator);
	}

	deleteStack(&operator_stack);

	return postfix;
}

// 

/**
 * @brief Returns the token resulting of the operation of token `op` on the two argument tokens `arg1` and `arg2`.
 * 
 * @param arg1 
 * @param op 
 * @param arg2 
 * @return Token* 
 */
Token* evaluateOperator(Token* arg1, Token* op, Token* arg2) {
	assert(tokenIsNumber(arg1));
	assert(tokenIsOperator(op));
	assert(tokenIsNumber(arg2));

	switch (tokenGetOperatorSymbol(op)) {
		case '+': return createTokenFromValue(tokenGetValue(arg1) + tokenGetValue(arg2));
		case '-': return createTokenFromValue(tokenGetValue(arg1) - tokenGetValue(arg2));
		case '*': return createTokenFromValue(tokenGetValue(arg1) * tokenGetValue(arg2));
		case '/': return createTokenFromValue(tokenGetValue(arg1) / tokenGetValue(arg2));
		case '^': return createTokenFromValue(powf(tokenGetValue(arg1), tokenGetValue(arg2)));
		default: return NULL;
	}
}

/**
 * @brief Evaluates and returns the result of a given postfix expression queue.
 * 
 * @param postfix 
 * @return float 
 */
float evaluateExpression(Queue* postfix) {
	Stack* value_stack = createStack(0);

	while(!queueEmpty(postfix)) {
		Token* token = queueTop(postfix);
		postfix = queuePop(postfix);

		assert(!tokenIsParenthesis(token));

		Token* value;

		if(tokenIsNumber(token)) value = token;
		else {
			Token* arg2 = stackTop(value_stack);
			stackPop(value_stack);
			Token* arg1 = stackTop(value_stack);
			stackPop(value_stack);

			value = evaluateOperator(arg1, token, arg2);

			deleteToken(&arg1);
			deleteToken(&arg2);
			deleteToken(&token);
		}

		stackPush(value_stack, value);
	}

	assert(!stackEmpty(value_stack));

	Token* result_value = stackTop(value_stack);
	float result = tokenGetValue(result_value);

	stackPop(value_stack);
	deleteToken(&result_value);

	assert(stackEmpty(value_stack));
	deleteStack(&value_stack);

	return result;
}

// 

/**
 * @brief Computes the expressions that can be read from the given input and prints the computation traces and results.
 * 
 * @param input 
 */
void computeExpressions(FILE* input) {
	char* line = malloc(sizeof(char) * LINE_LENGTH_MAX);
	size_t line_length_max = LINE_LENGTH_MAX;

	/* Requires a long type instead of size_t because the last loop lap with make it negative. */
	ssize_t line_length;

	size_t total_readed_char_count = 0;
	size_t old_total_readed_char_count = total_readed_char_count;

	while(!feof(input)) {
		char* line_cursor = line;
		total_readed_char_count = getline(&line_cursor, &line_length_max, input);

		line_length = total_readed_char_count - old_total_readed_char_count;
		
		if(line_length > 0) line[--line_length] = '\0';
		
		if(line_length > 0) {
			printf("Input : %s\n", line);

			// 

			Queue* infix = stringToTokenQueue(line);

			printf("Infix : ");
			queueDump(stdout, infix, printToken);
			printf("\n");

			Queue* postfix = shuntingYard(infix);

			printf("Postfix : ");
			queueDump(stdout, postfix, printToken);
			printf("\n");

			float result = evaluateExpression(postfix);

			printf("Expression : %f\n", result);
			
			printf("\n");

			// 

			while(!queueEmpty(infix)) {
				Token* token = (Token*) queueTop(infix);
				infix = queuePop(infix);
				deleteToken(&token);
			}

			while(!queueEmpty(postfix)) {
				Token* token = (Token*) queueTop(postfix);
				postfix = queuePop(postfix);
				deleteToken(&token);
			}

			deleteQueue(&infix);
			deleteQueue(&postfix);
		}
	}

	free(line);
}

// 

/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */
int main(int argc, char **argv){
	FILE *input;
	
	if (argc<2) {
		fprintf(stderr,"usage : %s filename\n", argv[0]);
		return 1;
	}
	
	input = fopen(argv[1], "r");

	if ( !input ) {
		perror(argv[1]);
		return 1;
	}

	computeExpressions(input);

	fclose(input);
	return 0;
}