## Lexical elements [C11 Draft Standard 6.4 & 6.10]

Each preprocessing token that is converted to a token shall have the lexical form of a either a keyword, 
an identifier, a constant, a string literal, or a punctuator. \

#### Syntax 

*token:*\
&nbsp;&nbsp;&nbsp;&nbsp;*keyword*
&nbsp;&nbsp;&nbsp;&nbsp;*identifier*
&nbsp;&nbsp;&nbsp;&nbsp;*constant*
&nbsp;&nbsp;&nbsp;&nbsp;*string-literal*
&nbsp;&nbsp;&nbsp;&nbsp;*punctuator*


*preprocessing-token:*\
&nbsp;&nbsp;&nbsp;&nbsp;*header-name*
&nbsp;&nbsp;&nbsp;&nbsp;*identifier*
&nbsp;&nbsp;&nbsp;&nbsp;*pp-number*
&nbsp;&nbsp;&nbsp;&nbsp;*character-constant*
&nbsp;&nbsp;&nbsp;&nbsp;*string-literal*
&nbsp;&nbsp;&nbsp;&nbsp;*punctuator*

#### Semantics 
> A preprocessing token is the minimal lexical element of the language in translation phases 3 to 6. Categories are 
mentioned above (*preprocessing-token*). \
> A token is the minimal lexical element of the language in translation phases 7 and 8. Categories are mentioned 
above (*token*). \
> There seems to be this tight coupling between preprocessing and lexing. My current understanding tells me that 
we're essentially breaking the input into tokens, which we're calling preprocessing tokens. Then, we're kind of 
doing a context dependent parsing focused on preprocessing. After recursively acting on the preprocessing tokens 
we'll deem the program to be called a translation unit. This translation unit is what one would typically call lexing. \
> The preprocessing tokenisation is greedy. An excerpt from standard "*If the input stream has been parsed into preprocessing tokens up to a given character, the next preprocessing token is the longest sequence of characters that could constitute a preprocessing token. There is one exception to this rule: header name preprocessing tokens are recognized only within #include preprocessing directives and in implementation-defined locations within #pragma directives. In such contexts, a sequence of characters that could be either a header name or a string literal is recognized as the former*" \
> Given that we've understood preprocessing to be context dependent lexing, we shouldn't be too surprised to see a
large overlap between the non-terminals enlisted under each category above. Below we'll define the union of the two \
categories. 

#### Keywords 


##### Syntax 

*keyword:* one of\
&nbsp;&nbsp;&nbsp;&nbsp;{**auto**, **break**, **case**, **char**, **const**, **continue**, 
**defaulti**, **do**, **double**, **else**, **enum**, **extern**, **float**, **for**, 
**goto**, **if**, **inline**, **int**, **long**, **register**, **restrict**, **return**, 
**short**, **signed**, **sizeof**, **static**, **struct**, **switch**, **typedef**, 
**union**, **unsigned**, **void**, **volatile**, **while**, **_Alignas**, **_Alignof**, 
**_Atomic**, **_Bool**, **_Complex**, **Generic**, **_Imaginary**, **Noreturn**,
**_Static_assert**, **_Thread_local**}\

##### Semantics

1. The above tokens (case sensitive) are reserved (in translation phases 7 and 8) for use as
keywords, and shall not be user otherwise.


#### Identifiers 


##### Syntax 

*identifier:*\
 &nbsp;&nbsp;&nbsp;&nbsp;*identifier-nondigit*\
 &nbsp;&nbsp;&nbsp;&nbsp;*identifier identifier-nondigit*\
 &nbsp;&nbsp;&nbsp;&nbsp;*identifier identifierdigit*\

 *identifier-nondigit:*\
 &nbsp;&nbsp;&nbsp;&nbsp;*nondigit*\
 &nbsp;&nbsp;&nbsp;&nbsp;*universal-character-name*\
 &nbsp;&nbsp;&nbsp;&nbsp;other implementation-defined characters\

 *nondigit:* one of\
  &nbsp;&nbsp;&nbsp;&nbsp;**_ a b c d e f g h i j k l m** \
  &nbsp;&nbsp;&nbsp;&nbsp;**n o p q r s t u v w x y z** \
  &nbsp;&nbsp;&nbsp;&nbsp;**A B C D E F G H I J K L M** \
  &nbsp;&nbsp;&nbsp;&nbsp;**N O P Q R S T U V W X Y Z** \
  
  *nondigit*: one of\
   &nbsp;&nbsp;&nbsp;&nbsp;**0 1 2 3 4 5 6 7 8 9** \


##### Semantics
1. The grammar specifies the structure of a valid identifier. There is no specific limit on 
the maximum length of an identifier.\
2. The identifier **__func__** shall be implicity declared by the translator as if, 
immediately following the opening brace of each function definition, the declaration\
`static const char __func__[] = "function-name";`\
appeared, where *function-name* is the name of the lexically-enclosing function.


#### Constants

##### Syntax 

*constant:*\
 &nbsp;&nbsp;&nbsp;&nbsp;*integer-constant*\
 &nbsp;&nbsp;&nbsp;&nbsp;*float-constant*\
 &nbsp;&nbsp;&nbsp;&nbsp;*enumeration-constant*\ 
 &nbsp;&nbsp;&nbsp;&nbsp;*character-constant*\
    integer

##### Semantics




