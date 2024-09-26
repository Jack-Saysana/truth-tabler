# Truth Tabler

A simple truth table generator

## Compiling and Running

Compile using gcc:

```
gcc -o truth-tabler truth.c
```

Run the compiled executable to begin the program

```
./truth-tabler
```

## Usage

When beginning the program, the user will be prompted to enter an input file.

```
./truth-tabler
Enter input file:

```
Enter the path of an input file to generate a truth table

*Important* The path to the desired input file must be relative to the program's executable

### Example
Generating a truth table from the provided input file, named test.txt, which is located in the root of the repo.

```
./truth-tabler
Enter input file:
test.txt
Opening file...
File found
First line: pqr^>
| p | q | r | (q ^ r) | (p -> (q ^ r)) |
| T | T | T |    T    |        T       |
| T | T | F |    F    |        F       |
| T | F | T |    F    |        F       |
| T | F | F |    F    |        F       |
| F | T | T |    T    |        T       |
| F | T | F |    F    |        T       |
| F | F | T |    F    |        T       |
| F | F | F |    F    |        T       |
```
 
### Input File Format

The propositional statement should be written on the first line of the input file in reverse polish notation, as only the first line of the file is read.

#### Logical Operators

##### List of reserved operators:

'>' : implication

'Z' : bi-conditional

'x' : exclusive or

'^' : and

'v' : inclusive or

'-' : negation

All lowercase variables (except x and v) are considered propositional variables

All other characters are ignored

#### Example

in test.txt the propositional statement is:

```
pqr^>
```

*Important* Statements must be written in reverse polish notation

So the statement above translates to:
```
p -> (q ^ r)
```

#### TODO
- Dynamic allocation of buffer resources
  - Bounds checking, etc...
  - **Assigned to:** Zack Bostock


