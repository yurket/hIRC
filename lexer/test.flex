


letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}]

nonwhite   [^\x0\xd\xa\x20]
crlf       "\n"

%%

{crlf}       printf("<crlf>\n");
