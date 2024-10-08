#include <stdio.h>
#include <math.h>

#define MAX_SYMS 6
#define MAX_OPS 50 
#define MAX_ROWS 64

int get_ln(FILE *, char *);
int parse_ln(char *);
int pop_args(char [][64], int *, char[][64], char *, int);
int op_exists(char [][64], int, char *);

int gen_table(char [][64], char[][4], char *, int, int);
int set(char *, char *); 

int index_of(char *, char);

int alphabetize(char *, int);

int main() {
	printf("Enter input file:\n");
	fflush(stdout);
	char f_name[64];
	scanf("%s", f_name);
	printf("Opening file...\n");

	FILE *fp;
	fp = fopen(f_name, "r");
	if (fp == NULL) {
		printf("File not found\n");
	} else {
		printf("File found\n");
		char line[64];
		get_ln(fp, line);
		printf("First line: %s", line);

		parse_ln(line);
	}
}

//Retrieves first line of file
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

//Parse reverse polish notation line to generate appropriate proposition info
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
	char operations[MAX_OPS][64];
	char op_codes[MAX_OPS][4];
	int num_operations = 0;

	char next;
	int i = 0;
	//Iterate through line, processing each character
	while ((next = line[i]) != '\0') {
		char args[2][64];
		//Implication
		if (next == '>') {
			//Pop 2 arguments off the operation stack for implication	
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			
			//Check if exact operation exists
			char temp_op[64];
			sprintf(temp_op, "(%s -> %s)", args[1], args[0]);
			int op_index = op_exists(operations, num_operations, temp_op);
			//printf("operation: %s\nindex: %d\n", temp_op, op_index);
			if (op_index == -1) {
				//Add opperation to opperation list if it does not already exist
				sprintf(operations[num_operations], "(%s -> %s)", args[1], args[0]);
				op_codes[num_operations][2] = '>';
				num_operations++;
				//Add operation onto the stack
				*op = (num_operations * -1);
			} else {
				//Add operation onto the stack	
				*op = (op_index + 1) * -1;
			}
			//Increment stack pointer to empty element
			op++;
			stack_len++;		
		} else
		//Biconditional
		if (next == 'Z') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			
			char temp_op[64];
			sprintf(temp_op, "(%s <-> %s)", args[1], args[0]);
			int op_index = op_exists(operations, num_operations, temp_op);
			//printf("operation: %s\nindex: %d\n", temp_op, op_index);
			if (op_index == -1) {
				sprintf(operations[num_operations], "(%s <-> %s)", args[1], args[0]);
				op_codes[num_operations][2] = 'Z';
				num_operations++;
				*op = (num_operations * -1);
			} else {
				*op = (op_index + 1) * -1;
			}
			op++;
			stack_len++;
		} else
		//And
		if (next == '^') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			
			char temp_op[64];
			sprintf(temp_op, "(%s ^ %s)", args[1], args[0]);
			int op_index = op_exists(operations, num_operations, temp_op);
			//printf("operation: %s\nindex: %d\n", temp_op, op_index);
			if (op_index == -1) {
				sprintf(operations[num_operations], "(%s ^ %s)", args[1], args[0]);
				op_codes[num_operations][2] = '^';
				num_operations++;
				*op = (num_operations * -1);
			} else {
				*op = (op_index + 1) * -1;
			}	
			op++;
			stack_len++;
		} else
		//Or
		if (next == 'v') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			
			char temp_op[64];
			sprintf(temp_op, "(%s v %s)", args[1], args[0]);
			int op_index = op_exists(operations, num_operations, temp_op);
			//printf("operation: %s\nindex: %d\narg 0: %s\narg 1: %s\n", temp_op, op_index, args[1], args[0]);
			if (op_index == -1) {
				sprintf(operations[num_operations], "(%s v %s)", args[1], args[0]);
				op_codes[num_operations][2] = 'v';
				num_operations++;
				*op = (num_operations * -1);
			} else {
				*op = (op_index + 1) * -1;
			}	
			op++;
			stack_len++;
		} else
		//XOR
		if (next == 'x') {
			pop_args(args, op, operations, op_codes[num_operations], 2);
			op -= 2;
			stack_len -= 2;
			
			char temp_op[64];
			sprintf(temp_op, "(%s x %s)", args[1], args[0]);
			int op_index = op_exists(operations, num_operations, temp_op);
			//printf("operation: %s\nindex: %d\n", temp_op, op_index);
			if (op_index == -1) {
				sprintf(operations[num_operations], "(%s x %s)", args[1], args[0]);
				op_codes[num_operations][2] = 'x';
				num_operations++;
				*op = (num_operations * -1);
			} else {
				*op = (op_index + 1) * -1;
			}	
			op++;
			stack_len++;
		} else 
		//Negation
		if (next == '-') {
			//Because negation only takes one argument, we only pop one argument off the operation stack
			pop_args(args, op, operations, op_codes[num_operations], 1);
			op--;
			stack_len--;
			
			//Check if operation already exists
			char temp_op[64];
			sprintf(temp_op, "-%s", args[0]);
			int op_index = op_exists(operations, num_operations, temp_op);
			//printf("operation: %s\nindex: %d\n", temp_op, op_index);
			if (op_index == -1) {
				sprintf(operations[num_operations], "-%s", args[0]);
				op_codes[num_operations][1] = '-';
				op_codes[num_operations][2] = '\0';
				num_operations++;
				//Add operation onto stack
				*op = (num_operations * -1);
			} else {
				//Add operation onto stack
				*op = (op_index + 1) * -1;
			}
			//Increment stack pointer
			op++;
			stack_len++;
		} else if (next >= 'a' && next <= 'z') { 
			//Handle proposition variables
			if (op < &op_stack[100]) {
				//Check if variable already exists	
				int exists = 0;
				for (int i = 0; i < num_syms; i++) {
					if (symbols[i] == next) {
						exists = 1;
					}
				}
				if (exists == 0) {
					//Ensure max number of symbols has not been exceeded	
					if (num_syms == MAX_SYMS) {
						printf("Exceeded max symbol count\n");
						return -1;
					}
					symbols[num_syms] = next;
					num_syms++;
				}
				//Add variable onto operation stack
				*op = next;
				//Increment stack pointer
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

	alphabetize(symbols, num_syms);
	gen_table(operations, op_codes, symbols, num_syms, num_operations);

	return 0;
}

//Pops a given number of arguments off the operation stack and stores them into the args array
int pop_args(char args[][64], int *op, char operations[][64], char *op_code, int num) {
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

//Check if an operation exists within the operations array
int op_exists(char operations[][64], int num_ops, char *op) {
	char next;
	int length = 0;	
	while ((next = op[length]) != '\0') {
		length++;
	}	
	
	for (int i = 0; i < num_ops; i++) {
		int op_length = 0;
		while ((next = operations[i][op_length]) != '\0') {
			op_length++;
		}
		if (op_length == length) {
			int same = 1;
			for (int j = 0; j < length; j++) {
				if (op[j] != operations[i][j]) {
					same = 0;
				}
			}

			if (same) {
				return i;
			}
		} else {
			break;
		}
	}
	return -1;
}

//Generating a truth table from table info
int gen_table(char operations[][64], char op_codes[][4], char *symbols, int num_symbols, int num_ops) {
	//A truth tables number of rows is 2^n where n is the number of variables
	int rows = (int) pow(2.0, num_symbols);
		
	// array which represents the truth table
	// 2 additional rows for header and character count
	char table[MAX_ROWS + 2][MAX_OPS][64];

	// Iterate through the table and create each column of info
	int cols = 0;
	for (int i = 0; i < num_symbols + num_ops; i++) {
		cols++;
		//Truth values for an individual variable
		if (i < num_symbols) {
			//Sets the character width of the column to 3	
			table[0][i][0] = 3;	
			//Set the column header to the variable
			table[1][i][0] = symbols[i];
			//Calculate the number of times a variable repeats its value in it's column
			int streak = (int) (((double) rows) * (1.0/(pow(2.0, i + 1))));
			//printf("Symbol: %c\ni: %d\nStreak: %d\n", symbols[i], i, streak);
			//printf("for symbol %c:\n", symbols[i]);
			
			//Populate initial truth values
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
			//Sets character width of column to the length of the expression + 2 while also simultaneously setting the table header to the expression	
			table[0][i][0] = set(table[1][i], operations[i - num_symbols]) + 2;
			//printf("operation: %s\nlength: %d\n", table[1][i], table[0][i][0]);
			//Get the operation code, which specifies which arguments and what operation is represented by the current expression
			char *op_code = op_codes[i - num_symbols];
			int args[2];
			char op;
			//Handling incase an operation takes only 1 argument (such as negation)
			if (op_code[2] == '\0') {
				if (op_code[0] < 0) {
					args[0] = (op_code[0] * -1) + num_symbols - 1;
				} else {
					args[0] = index_of(symbols, op_code[0]);
				}
				args[1] = -1;
				op = op_code[1];
			} else {
				//Handling for operations that take 2 arguments
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
			// Determine truth value of expression based on the row's variable's combonation of truth values
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
					if (table[j + 2][args[0]][0] != table[j + 2][args[1]][0]) {
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
	dest[i] = '\0';

	return i;	
}

//Retrieves the index at which a character occurs in a string, if any
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

//Alphabetizes variables utilizing bubble sort (reatively slow)
int alphabetize(char *arr, int count) {
	char temp;
	for (int i = 0; i < count - 1; i++) {
		for (int j = 0; j < count - i - 1; j++) {
			if (arr[j] > arr[j+1]) {
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}

	return 0;
}
