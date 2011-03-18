%{

%}

%option yylineno

ID     [a-zA-Z][_a-zA-Z0-9]*

NUMBER [0-9]+
STRING (\"[^\n"]*\")
COMMENT (#[^\n]*)

%%

{COMMENT}  {}

controller { return CONTROLLER; }
signal     { return SIGNAL; }

control    { return CONTROL; }
noteon     { return NOTEON; }
noteoff    { return NOTEOFF; }
aftertouch { return AFTERTOUCH; }
programchange { return PROGRAMCHANGE; }
channelpressure { return CHANNELPRESSURE; }
pitchwheel { return PITCHWHEEL; }

status     { return STATUS; }
digital    { return DIGITAL; }
analog     { return ANALOG; }

\{         { return '{'; }
\}         { return '}'; }
:          { return ':'; }
=          { return '='; }
;          { return ';'; }

{NUMBER}   { yylval.ival = atoi(yytext); return NUMBER; }
{STRING}   { return STRING; }
{ID}       { yylval.str = strdup(yytext); return ID; }

.          ;
\n         ;

%%


