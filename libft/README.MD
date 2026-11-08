*This project has been created as part of the 42 curriculum by \<tschwab>*

# Libft - a library by TomSwb

## Description
	
#### Goal:
Create my own C library (a collection of functions) to use during the 42 curriculum

#### Brief overview:	
Contains a total of 43 functions made according to:
1. Libc functions (with some adaptations).
2. Additional functions (according to specific instructions).
3. Linked list related functions (according to specific instructions).
	
Additionally contains: 
-	libft.h.
-	Makefile.
-	README.

## Instructions

1. Clone the repo at the root of your project, creating the libft directory in the process:

```bash
git clone https://github.com/TomSwb/libft.git libft
```

*optional: remove the .with_mains folder, it is a test folder*

2. Navigate to the newly created ./libft then run:

```bash
make
``` 

3. In your project Makefile:
- Add this flag to your compiling: 

	```text
	-I./libft
	```
	
- Add this var: 

	```text
	LIBFT = ./libft/libft.a
	```

- Compile e.g.: 

	```text
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT)
	```

4. 
	
	```text
	#include "libft.h"
	```

	can be included as a header in any file requiring the libft function declarations.

5. If you decide to alter any functions in the library don't forget to adapt the library Makefile and to run:

```bash
make re
```

6. You can also clean the .o files:

```bash
make clean
```

7. And/or clean both the .o files and the .a library;

```bash
make fclean
```

## Resources

#### Editor:
- `VSCodium` : Used to avoid Microsoft telemetry, and to avoid AI integrated temptations.

#### 42:
- `libft.pdf` version: 19.3

#### System:
- `Linux man` for the libc related functions

### Github:

##### Before beginning of cursus: 
- https://github.com/mrochedy/42-Libft/tree/main

##### Other:
- [Markdown List Syntax](https://www.markdownlang.com/basic/lists.html) - To learn MD syntax

### AI:

#### Before beginning of cursus

⚠️ Disclaimer: I only ask for actual answer/solutions when I realise that I am missing theoretical knowledge.

e.g. : a new syntax or a new concept I had not explored previously. 

I always write my code to the best of my abilities first.

My AI agents are set to guide me with questions as per 42 pedagogical philosophy.

I use VSCodium on purpose to not have AI integration, this means except my copy-paste into chats for some reviews, AI does not see my code, and AI will never write my code - I do not copy-paste from AI into my code.

- `Perplexity:` 
	1. A quick search to understand how to actually use the libft library in future projects.
	2. Confirm what certain Markdown syntax I found in repos do.
	3. Back and forth conversation to ensure my understanding of more complex functions.

- `ChatGPT:`
	1. Back and forth conversation to ensure my understanding of more complex functions.
	2. Help in learning to figure out by myself edge cases and ensure functions are safe and complete.
	3. Help in theory of pointers and many different syntaxes for them and their related uses.
	4. Help in learning clean architecture.
	5. Help in learning the theory behind linked list.
	6. Final documentation checks.

## Detailed description

#### List of all the available functions:

##### Part 1: Libc replica

- `ft_isprint`: Checks for  any  printable  character  including space.
- `ft_isascii`: Checks if a given int c is a 7-bit unsigned char value that fits inside the ASCII characters set.
- `ft_isalpha`: Checks for an alphabetic character: 'A' through 'Z' and 'a' through 'z'.
- `ft_isdigit`: Checks for a digit: '0' through '9'.
- `ft_isalnum`: Checks for  an  alphanumeric  character; it  is equivalent to (ft_isalpha(int c) || ft_isdigit(int c)).

- `ft_tolower`: If c is an uppercase letter, returns its lowercase equivalent, if a lowercase representation exists in  the  current locale. Otherwise, it returns c.
- `ft_toupper`: If c is a lowercase letter, returns its uppercase equivalent, if an uppercase representation exists in  the  current locale. Otherwise, it returns c.
- `ft_atoi`: Converts the initial numerical portion of the given string to an int, skipping leading whitespace and accounting for a maximum of one + or - sign.

- `ft_strlen`:  Returns the length of the given char *s.
- `ft_strlcpy`: Copies src into dest and returns the len of the wanted result (len of src).
- `ft_strlcat` : Returns the total length of the attempted concatenation, strlen(dst) + strlen(src) if successful or size + strlen(src) if the buffer dst has no '\0' char within size bytes which does not result in a concatenation.
- `ft_strncmp`: Compares up to n characters of s1 and s2 as unsigned char values, stopping at a '\0' character. Returns the difference between the first pair of different characters, or 0 if no difference is found.
- `ft_strnstr`:  Locates the first occurrence of the null-terminated string 'little' in the string 'big', where not more than 'len' characters are searched. Characters that appear after a '\0' character are not searched. 
- `ft_strchr`:  Returns a pointer to the first matched character or NULL  if the character is not found.  The terminating null byte is considered part of the string, so that if c is specified as '\0', this function returns a pointer to the terminator.
- `ft_strrchr`:  Returns a pointer to the last matched character or NULL  if the character is not found.  The terminating null byte is considered part of the string, so that if c is specified as '\0', this function returns a pointer to the terminator.

- `ft_bzero`:  Fills the first n bytes of the given memory area with '\0'.
- `ft_memchr`:  Searches the first n bytes of the given memory area for the byte value represented by c, and returns a pointer to the first matching byte or NULL if not found.
- `ft_memcmp`:  Returns the result of comparing the bytes value up to 'n' number of bytes.
- `ft_memcpy`:  Returns a pointer to the given 'dest' array after copying 'n' bytes values from 'src'. Does not account for overlap in memory.
- `ft_memmove`:  Returns a pointer to the given 'dest' array after copying 'n' len of bytes from 'src' in it, accounting that overlap does not affect the data unlike memcpy.
- `ft_memset`:  Returns the given 's' array filled with the 'c' char up to 'n' bytes.

- `ft_calloc`: Allocates memory for n elements of size bytes each, initializes all allocated bytes to 0, and returns a pointer to the allocated memory.
- `ft_strdup`: Returns an array with the original 's' string duplicated in it (copied) but in the heap instead of the stack, using malloc.

##### Part 2: Additional functions

- `ft_substr` : Returns a malloced char * containing at most 'len' characters from 's', starting at index start. If start is greater than or equal to ft_strlen(s), returns an allocated empty string.
- `ft_strjoin` : Returns a 'char *' containing the concatenation of s1 and s2 in allocated memory.
- `ft_strtrim` : Returns a malloced char * containing s1 with all characters present in set removed from its beginning and end.
- `ft_split` : Returns a malloced char ** containing the strings extracted from '*s' delimited by 'c'.  

- `ft_itoa` : Returns an array 'char *' representing the given int 'n'.

- `ft_strmapi` : Returns a malloced char * containing the characters from s after applying the given function f to each character and its index.
- `ft_striteri` : Iterates by address through each char inside '*s', applying the given function.

- `ft_putchar_fd` : Writes the given char 'c' inside the given file 'fd'.
- `ft_putstr_fd` : Writes the given 'char *s' into the given file 'fd', no new line.
- `ft_putendl_fd` : Writes the given 'char *s' into the given file 'fd', followed by a new line.
- `ft_putnbr_fd` : Writes the given 'int n' into the given file 'fd'.

##### Part 3: Linked list related

- `ft_lstnew` : Returns a new allocated node, with its content set as the given 'content'.
- `ft_lstadd_front` : Appends 'new' to the front of 'lst', making new->next point to either NULL or the address of the original first node of lst.
- `ft_lstsize` : Returns the number of nodes in the given 'lst'.
- `ft_lstlast` : Returns a pointer to the last node of list 'lst'.
- `ft_lstadd_back` : Appends the given node 'new' to the end of 'lst'.

- `ft_lstdelone` : Frees a single node after applying the given del function to its content.
- `ft_lstclear` : Frees all nodes of lst and their contents, using the given del function for each content, and sets the list pointer to NULL.

- `ft_lstiter` : Iterates through each node's content in 'lst', applying the changes from the given function 'f'.
- `ft_lstmap` : Returns a new list created by applying the given function f to the content of each node of the original list. If an allocation fails, clears the whole new list using the given del function and returns NULL.
