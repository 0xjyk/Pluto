## Lexical elements [C11 Draft Standard 6.4 & 6.10]

Each preprocessing token that is converted to a token shall have the lexical form of a either a keyword, 
an identifier, a constant, a string literal, or a punctuator. 

#### Syntax 

*token:* 
&nbsp;&nbsp;&nbsp;&nbsp;*keyword*
&nbsp;&nbsp;&nbsp;&nbsp;*identifier*
&nbsp;&nbsp;&nbsp;&nbsp;*constant*
&nbsp;&nbsp;&nbsp;&nbsp;*string-literal*
&nbsp;&nbsp;&nbsp;&nbsp;*punctuator*


*preprocessing-token:*
&nbsp;&nbsp;&nbsp;&nbsp;*header-name*
&nbsp;&nbsp;&nbsp;&nbsp;*identifier*
&nbsp;&nbsp;&nbsp;&nbsp;*pp-number*
&nbsp;&nbsp;&nbsp;&nbsp;*character-constant*
&nbsp;&nbsp;&nbsp;&nbsp;*string-literal*
&nbsp;&nbsp;&nbsp;&nbsp;*punctuator*

#### Semantics 
> A preprocessing token is the minimal lexical element of the language in translation phases 3 to 6. Categories are 
mentioned above (*preprocessing-token*).
> A token is the minimal lexical element of the language in translation phases 7 and 8. Categories are mentioned 
above (*token*).
> There seems to be this tight coupling between preprocessing and lexing. My current understanding tells me that 
we're essentially breaking the input into tokens, which we're calling preprocessing tokens. Then, we're kind of 
doing a context dependent parsing focused on preprocessing. After recursively acting on the preprocessing tokens 
we'll deem the program to be called a translation unit. This translation unit is what one would typically call lexing. 
> The preprocessing tokenisation is greedy. An excerpt from standard "*If the input stream has been parsed into preprocessing tokens up to a given character, the next preprocessing token is the longest sequence of characters that could constitute a preprocessing token. There is one exception to this rule: header name preprocessing tokens are recognized only within #include preprocessing directives and in implementation-defined locations within #pragma directives. In such contexts, a sequence of characters that could be either a header name or a string literal is recognized as the former*"
> Given that we've understood preprocessing to be context dependent lexing, we shouldn't be too surprised to see a
large overlap between the non-terminals enlisted under each category above. Below we'll define the union of the two
categories. 
> List of keywords -   
&nbsp;&nbsp;&nbsp;&nbsp;{**auto**, **break**, **case**, **char**, }

