/*
to         {channel}|{user}@{servername}|{nick}|{mask}
servername {hostname}
hostname   {shortname}("."{shortname})*
shortname  ({letter}|{number})({letter}|{number}|"-")*({letter}|{number})*
*/

target     {to}
to         ({channel}|{user})@{user}
channel    (#|&){chstring}
nick       {letter}({letter}|{number}|{special})+
mask       (#|"$"){chstring}
chstring   [^\t\r\n\7\0, ]+

/*********************** Other parameter syntaxes are: ************************/
user       {nonwhite}+
letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}]

/* <any 8bit code except SPACE (0x20), NUL (0x0), CR(0xd), and LF (0xa)> */
nonwhite   [^\x0\xd\xa\x20]
crlf       "\r\n"


%%

{to}         printf("<to %s>", yytext);
{channel}    printf("<chan %s>", yytext);

{nick}       printf("<nick %s>", yytext);
{mask}       printf("<mask %s>", yytext);
{user}       printf("<user %s>", yytext);
{crlf}       printf("<crlf>\n");
