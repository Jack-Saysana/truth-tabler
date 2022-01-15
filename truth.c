#include <stdio.h>
#include <math.h>

#define MAX_SYMS 6
#define MAX_OPS 50 
#define MAX_ROWS 64

int get_ln(FILE *, char *);
int parse_ln(char *);
int pop_args(char [][BUFSIZ], int *, char[][BUFSIZ], char *, int);

int gen_table(char [][BUFSIZ], char[][4], char *, int, int);
int set(char *, char *); 

int index_of(char *, char);

int main() {
	printf("Enter input file:\n");
	fflush(stdout);
	char f_name[BUFSIZ];
	scanf("%s", f_name);
	printf("Opening file...\n");

	FILE *fp;
	fp = fopen(f_name, "r");
	if (fp == NULL) {
		printf("File not found\n");
	} else {
		printf("File found\n");
		char line[BUFSIZ];
		get_ln(fp, line);
		printf("First line: %s", line);

		parse_ln(line);
	}
}

int get_ln(FILE *fp, char *line) {
	char next;
	int len = 0;
	while ((next = getc(fp)) != EOF) {
		line[len] = next;
		len++;
	}
	line[len] = '\0';

	return 0;
}

int parse_ln(char *line) {
	//Stack that keeps track of operands
	int op_stack[100];
	int stack_len = 0;
	//Pointer to operand on top of the stack
	int *op = op_stack;

	//Holds symbols used in statement
	char symbols[MAX_SYMS];
	int num_syms = 0;
	//Holds operations of statement
	char operations[MAX_OPS][BUFSIZ];
	char op_codes[MAX_OPS][4];
	int num_operations = 0;

	char next;
	int i = 0;

	while ((next = line[i]) != '\0') {
		char args[2][BUFSIZ];
		//Implication
		if (next == '>') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			sprintf(operations[num_operations], "(%s -> %s)", args[1], args[0]);
			op_codes[num_operations][2] = '>';
			num_operations++;
			*op = (num_operations * -1);
			op++;
			stack_len++;		
		} else
		//Biconditional
		if (next == 'Z') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			sprintf(operations[num_operations], "(%s <-> %s)", args[1], args[0]);
			op_codes[num_operations][2] = 'Z';
			num_operations++;
			*op = (num_operations * -1);
			op++;
			stack_len++;
		} else
		//And
		if (next == '^') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			sprintf(operations[num_operations], "(%s ^ %s)", args[1], args[0]);
			op_codes[num_operations][2] = '^';
			num_operations++;
			*op = (num_operations * -1);
			op++;
			stack_len++;
		} else
		//Or
		if (next == 'v') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			sprintf(operations[num_operations], "(%s v %s)", args[1], args[0]);
			op_codes[num_operations][2] = 'v';
			num_operations++;
			*op = (num_operations * -1);
			op++;
			stack_len++;
		} else
		//XOR
		if (next == 'x') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			sprintf(operations[num_operations], "(%s x %s)", args[1], args[0]);
			op_codes[num_operations][2] = 'x';
			num_operations++;
			*op = (num_operations * -1);
			op++;
			stack_len++;
		} else 
		//Negation
		if (next == '-') {
			pop_args(args, op, operations, op_codes[num_operations], 1);
			op--;
			stack_len--;
			sprintf(operations[num_operations], "-%s", args[0]);
			op_codes[num_operations][1] = '-';
			op_codes[num_operations][2] = '\0';
			num_operations++;
			*op = (num_operations * -1);
			op++;
			stack_len++;
		} else if (next >= 'a' && next <= 'z') { 
			if (op < &op_stack[100]) {	
				int exists = 0;
				for (int i = 0; i < num_syms; i++) {
					if (symbols[i] == next) {
						exists = 1;
					}
				}
				if (exists == 0) {
					if (num_syms == 10) {
						printf("Exceeded max symbol count\n");
						return -1;
					}
					symbols[num_syms] = next;
					num_syms++;
				}
				*op = next;
				op++;
				stack_len++;
			} else {
				printf("Exceeded max statement length\n");
				return -1;
			}
		}
		i++;
	}
	
	/*printf("Operations:\n");
	for (int i = 0; i < num_operations; i++) {
		printf("Operation: %s\n", operations[i]);
		printf("Op code: ");
		for (int j = 0; j < 3; j++) {
			if (op_codes[i][j] >= 0) {
				printf("%c", op_codes[i][j]);
			} else {
				printf("%d", op_codes[i][j]);
			}
		}
		printf("\n");
	}

	printf("Symbols:\n");
	for (int i = 0; i < num_syms; i++) {
		printf("%c\n", symbols[i]);
	}*/

	gen_table(operations, op_codes, symbols, num_syms, num_operations);

	return 0;
}

int pop_args(char args[][BUFSIZ], int *op, char operations[][BUFSIZ], char *op_code, int num) {
	for (int i = 0; i < num; i++) {
		op--;
		if (*op < 0) {
			set(args[i], operations[(*op * -1) - 1]);
		} else {
			sprintf(args[i], "%c", *op);
		}
		op_code[i] = *op;
		*op = -500;
	}
	return 0;
}

int gen_table(char operations[][BUFSIZ], char op_codes[][4], char *symbols, int num_symbols, int num_ops) {
	int rows = (int) pow(2.0, num_symbols);
		
	// 2 additional rows for header and character count
	char table[MAX_ROWS + 2][MAX_OPS][BUFSIZ];

	int cols = 0;
	for (int i = 0; i < num_symbols + num_ops; i++) {
		cols++;
		if (i < num_symbols) {
			table[0][i][0] = 3;	
			table[1][i][0] = symbols[i];
			int streak = (int) (((double) rows) * (1.0/(2.0 * (i + 1.0))));
			//printf("for symbol %c:\n", symbols[i]);
			for (int j = 0; j < rows; j++) {
				if ((j / streak) % 2 == 0) {
					table[j + 2][i][0] = 'T';
					//printf("%c\n", 'T');
				} else {
					table[j + 2][i][0] = 'F';
					//printf("%c\n", 'F');
				}
			}
		} else {
			table[0][i][0] = set(table[1][i], operations[i - num_symbols]) + 2;
			//printf("operation: %s\nlength: %d\n", table[1][i], table[0][i][0]);
			char *op_code = op_codes[i - num_symbols];
			int args[2];
			char op;
			if (op_code[1] == '\0') {
				if (op_code[0] < 0) {
					args[0] = (op_code[0] * -1) + num_symbols - 1;
				} else {
					args[0] = index_of(symbols, op_code[0]);
				}
				args[1] = -1;
				op = op_code[1];
			} else {
				if (op_code[0] < 0) {
					args[0] = (op_code[0] * -1) + num_symbols - 1;
				} else {
					args[0] = index_of(symbols, op_code[0]);
				}
				if (op_code[1] < 0) {
					args[1] = (op_code[1] * -1) + num_symbols - 1;
				} else {
					args[1] = index_of(symbols, op_code[1]);	
				}
				op = op_code[2];
			}
			//printf("op_code: %s\narg 1: %d\narg 2: %d\nop: %c\n", op_code, args[0], args[1], op);
			for (int j = 0; j < rows; j++) {
				if (op == '>') {
					if (table[j + 2][args[0]][0] == 'F' && table[j + 2][args[1]][0] == 'T') {
						table[j + 2][i][0] = 'F';
					} else {
						table[j + 2][i][0] = 'T';
					}
				} else if (op == 'Z') {
					if (table[j + 2][args[0]][0] == table[j + 2][args[1]][0]) {
						table[j + 2][i][0] = 'T';
					} else {
						table[j + 2][i][0] = 'F';
					}	
				} else if (op == '^') {
					if (table[j + 2][args[0]][0] == 'T' && table[j + 2][args[1]][0] == 'T') {
						table[j + 2][i][0] = 'T';
					} else {
						table[j + 2][i][0] = 'F';
					}
				} else if (op == 'v') {
					if (table[j + 2][args[0]][0] == 'T' || table[j + 2][args[1]][0] == 'T') {
						table[j + 2][i][0] = 'T';
					} else {
						table[j + 2][i][0] = 'F';
					}
				} else if (op == 'x') {
					if (table[j + 2][args[0]][0] != table[j + 2][args[0]][0]) {
						table[j + 2][i][0] = 'T';
					} else {
						table[j + 2][i][0] = 'F';
					}
				} else if (op == '-') {
					if (table[j + 2][args[0]][0] == 'T') {
						table[j + 2][i][0] = 'F';
					} else {
						table[j + 2][i][0] = 'T';
					}
				}	
			}
		}
	}

	for (int i = 1; i < rows + 2; i++) {
		if (i == 1) {
			for (int j = 0; j < cols; j++) {
				printf("| %s ", table[i][j]);
			}
			printf("|\n");
		} else {
			for (int j = 0; j < cols; j++) {
				printf("|");	
				int width = table[0][j][0];
				for (int k = 0; k < (width / 2); k++) {
					printf(" ");
				}
				printf("%s", table[i][j]);
				if (width % 2 == 0) {
					for (int k = 0; k < (width / 2) - 1; k++) {
						printf(" ");
					}
				} else {
					for (int k = 0; k < (width / 2); k++) {
						printf(" ");
					}
				}
			}
			printf("|\n");
		}
	}
	
	return 0;
}

// Assigns value of one char* to another then returns the length of the string
int set(char *dest, char *input) {
	char next;
	int i = 0;
	while ((next = input[i]) != '\0') {
		dest[i] = next;
		i++;
	}

	return i;	
}

int index_of(char *arr, char elem) {
	char next;
	int i = 0;
	while ((next = arr[i]) != '\0') {
		if (arr[i] == elem) {
			return i;
		}
		i++;
	}

	return -1;
}

