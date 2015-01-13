letter     [a-zA-Z]
number     [0-9]
special    [\-\[\]\\\`\^\{\}]
NICK       {letter}({letter}|{number}|{special})+


%%


^:{NICK}    printf("nick");
