## Translation Phases [C11 Draft Standard 5.1.12]

#### Definitions
1. *source files / preprocessing file* : The text of the program (Input for the compiler)
2. *preprocessing translation unit* : A source file together with all the headers and source files included via the preprocesssing directive **#include**.
3. *translation unit* : After preprocessing, a preprocessing translation unit is called *translation unit*  

#### Translation Phases 
1. If required, the soruce file's character set is mapped to the source character set (the character set known
to the compiler). This step is usually not required and isn't really coverd in the implementation. Additionally, 
Trigraph sequences are replaced by their corresponding single-character internal representations. The latter will 
be included in this implementation. 
2. Each instance of a backlash character (\) immediately followed by a new-line character is deleted. Doing so 
essentially means that physical source lines and converted into logical source lines. For some reason non-empty 
source files need to end with a new-line character which cannot be preceeded with a backspace character.
3. The source file is decomposed into preprocessing tokens and sequences of white-space characters (including
comments). A source file should not end in a partial preprocessing token or in a partial comment. Each comment is
replaced by one space character. New-line characters are retained. Mutiple whitespace characters are folded into
a single whitespace character (implementation defined).
4. Preprocessing directives are executed, macro invocations are expanded and **_Pragma** unary operator expressions are executed. A **#include** preprocessing directive causes the named header or source file to be processed from 
phase 1 to 4, recursively. All preprocessing directives are then deleted.
5. Each source character set member and escape sequence in character constants and string literals is converted to 
the corresponding member of the execution character set. 
6. Adjacent string literal tokens are concatenated 
7. White-space characters separating tokens are no longer significant. Each preprocessing token is converted into
a token. The resulting tokens are syntactically and semantically analyzed and translated as a translation unit. 
8. All exteranl object and function references are resolved. Library components are linked to satisfy external 
references to functions and objects not defined in the current translation. All such translator output is collected 
into a program image which contains information needed for execution in its execution environment. 



#### Open Questions 
1. Why do non-empty files need to end with a new-line character? 
2. Point 5 seems trivially complete. Is it? 
