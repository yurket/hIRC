/*message    ":"(.*)?.*{crlf}
prefix     servername | <nick> [ '!' <user> ] [ '@' <host> ]


*/

command    ({letter}+|{number}{3})

chstring   [^\t\r\n\7\0, ]+
NICK       {letter}({letter}|{number}|{special})+
SPACE      (" "|\t)+
letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}]



crlf       "\n"

%%

^:{NICK}    printf(":<nick>");
{SPACE}     printf("<space>");
{crlf}      printf("<\\n>\n");
{command}   printf("<com:%s>", yytext);
#{chstring}    printf("<chan:%s>", yytext);


