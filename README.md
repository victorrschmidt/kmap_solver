# Kmap solver

## How to use

### Create a sum of products expression

In `notation.in`, insert a sum of products expression in the following format:

```
P + P + ... + P
```

Where `P` denotes a sum of products expression in the following format:

```
ABC...Z
```

Where each letter is a unique char, representing a variable of the boolean
expression. Also every letter can be preceeded by the '!' char (by default), to
represent the complement of the following variable.

### Example of a valid expression

```
ABC!D + AB!CD + !A!BC!D + ABCD
```

## Program configs

The amount of chars used to represent variables are defined in program.h, in
the macro:

```c
MAX_VARIABLE_COUNT
```

You can change it, but its value should not be greater than 30, since it will
cause bit shifting errors.

The set of chars used to represent variables are defined in program.h, in the
macro:

```c
ACCEPTED_VARIABLE_CHARS
```

You can change the set of chars as you like, but make sure that the string
length is less or equal to MAX_VARIABLE_COUNT.

You can also change the chars used to represent the negation and sum symbols,
that are defined in program.h, respectively, at:

```c
NEGATION_CHAR
```

and

```c
SUM_CHAR
```

The resulting expression will be shown at `result.out`.

## How to run

### Compile the files

```bash
gcc main.c src/*.c -Iinclude -O2 -o run
```

### Run the executable

#### Windows

```bash
run.exe
```

#### Linux

```bash
./run
```