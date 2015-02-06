
message    ":"{prefix}{SPACE}
prefix     {nick}("!"{text})?("@"{text})?
command    ({upper}+)|({number}{3})


params     {SPACE}(":"{trailing})
/*params     {SPACE}((":"abc)|(dfg{d}))**/
middle     [^:][^\x20\x0\r\n]*
trailing   [^\x0\r\n]*


nick       {letter}({letter}|{number}|{special})+

text       [^:][^!@\x20\x0\r\n]*
SPACE      (" ")+

upper      [A-Z]
letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}]

nonwhite   [^\x0\xd\xa\x20]

crlf       "\r\n"

%%

{command}    printf("<com %s>", yytext);
{prefix}     printf("<pref %s>", yytext);
{params}     printf("<params %s>", yytext);
{crlf}       printf("<crlf>\n");
