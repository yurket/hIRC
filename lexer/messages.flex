


/*message    (:{prefix}{SPACE})?({command}{params}{crlf})



*/
message  (:{prefix}{SPACE})?{command}{params}{crlf}

prefix     {nick}?(!{user})?('@'{host})?
command    {letter}+|{number}{3}
params     {SPACE}[(":"{trailing})|({middle}{params})]*
middle     [^:][^\x20\x0\r\n]*
trailing   [^\x0\r\n]*


nick       {letter}({letter}|{number}|{special})+
/******* simple ********/
SPACE      (" ")+
letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}]
crlf       "\r\n"

%%

{message}   printf("<msg %s>", yytext);
{prefix}    printf("<prefix %s>", yytext);
{command}   printf("<command %s>", yytext);
{params}    printf("<params %s>", yytext);
{middle}    printf("<mid %s>", yytext);
{trailing}  printf("<trail %s>", yytext);
{crlf}      printf("<crlf>\n");

