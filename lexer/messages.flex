/*message    (:{prefix}{SPACE})?({command}{params}{crlf})

message  (:{prefix}{SPACE})?{command}{params}{crlf}

prefix     {nick}?(!{user})?('@'{host})?

params     {SPACE}[(":"{trailing})|({middle}{params})]*
middle     [^:][^\x20\x0\r\n]*
*/



message    (:{prefix}{SPACE})?{command}{SPACE}{channel}?{SPACE}?{trailing}{crlf}
prefix     {nick}!.*
command    {letter}+|({number}{3})
trailing   [^\x0\r\n]*



nick       {letter}({letter}|{number}|{special})+
channel    (#|&){chstring}
chstring   [^\t\r\n\7\0]+

/******* simple ********/
SPACE      (" ")+
letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}]
crlf       "\r\n"

%%

{message}   printf("<message %s>", yytext);
{trailing}  printf("<trail %s>", yytext);
{crlf}      printf("<crlf>\n");

