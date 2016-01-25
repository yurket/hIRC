message    ({TIME}?:{NICK}{HOSTNAME2}{SPACE}{COMMAND}{SPACE}?{CHANNEL}?{printable3}{crlf})

TIME       ({number}{2}:){3}" "

NICK       {letter}{printable}+
HOSTNAME2  (!{HOSTNAME})
HOSTNAME   ({printable2}+@{printable2}+)
CHANNEL    (#{letter}+)
COMMAND    ("PRIVMSG"|"JOIN"|"QUIT")
SPACE      (" "|\t)+

printable  ({letter}|{number}|{special}|_)
printable2  ({printable}|\.)
printable3  (" ":{chstring})
letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}|]
chstring   [^\t\r\n\7\0]+
crlf       "\n"|"\r\n"

%%

{TIME}        printf("<time: %s>", yytext);
{COMMAND}    printf("<COM:%s>", yytext);
" :"{NICK}    printf(":<nick:%s>", yytext);
{SPACE}     printf("<space>");
{crlf}      printf("<\\n>\n");
{CHANNEL}    printf("<chan:%s>", yytext);

{HOSTNAME2}    printf("<HOST:%s>", yytext);
{printable3}  {REJECT;  printf("<print3:%s>", yytext); }
{message}   printf("<msg:%s>", yytext);

%%



