# Grammar Play

A C++ program to analyze context-free grammars. The program reads a grammar description and performs specific tasks based on command line arguments, such as printing lists of terminals and non-terminals, calculating FIRST and FOLLOW sets, left factoring the grammar, and eliminating left recursion.

The input grammar is defined using a context-free grammar specification with ID tokens representing terminals and non-terminals. Rules define how these tokens can be transformed, where each rule starts with a non-terminal symbol followed by an arrow and a sequence of terminals and non-terminals or epsilon.

Program processes this input and depending on the provided command line argument, executes one of five specific analyses or transformations on the grammar. These include:

1. Listing terminals and non-terminals in their appearance order.
2. Calculating FIRST sets for each non-terminal.
3. Calculating FOLLOW sets for each non-terminal.
4. Left factoring the grammar to eliminate common prefixes.
5. Eliminating left recursion to make the grammar suitable for recursive descent parsing.


### Examples

**Sample Input:**
```
S -> C B C D A B C *
A -> C B C D *
A -> C B C B *
A -> C B D *
A -> C B B *
B -> b *
C -> c *
D -> d *
#
```
**Task 1 Output:**
*Listing terminals and non-terminals in their appearance order.*
```
b c d S C B D A 
```

**Task 2 Output:**
*Calculating FIRST sets for each non-terminal.*
```FIRST(S) = { c }
FIRST(C) = { c }
FIRST(B) = { b }
FIRST(D) = { d }
FIRST(A) = { c }
```

**Task 3 Output:**
*Calculating FOLLOW sets for each non-terminal.*
```FOLLOW(S) = { $ }
FOLLOW(C) = { $, b, d }
FOLLOW(B) = { b, c, d }
FOLLOW(D) = { b, c }
FOLLOW(A) = { b }
```

**Task 4 Output:**
*Left factoring the grammar to eliminate common prefixes.*
```
A -> C B A2 #
A1 -> B #
A1 -> D #
A2 -> B #
A2 -> C A1 #
A2 -> D #
B -> b #
C -> c #
D -> d #
S -> C B C D A B C #
```

**Task 5 Output:**
*Eliminating left recursion to make the grammar suitable for recursive descent parsing.*
```A -> C B B #
A -> C B C B #
A -> C B C D #
A -> C B D #
B -> b #
C -> c #
D -> d #
S -> c B C D A B C #
```
