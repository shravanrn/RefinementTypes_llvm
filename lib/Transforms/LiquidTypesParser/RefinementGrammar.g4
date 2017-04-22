grammar RefinementGrammar;

options 
{
    language  =  Cpp;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
//Run generate.cmd after making any changes to this file
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

WS : [ \t\r\n]+ -> channel(HIDDEN); // ignore spaces, tabs, newlines in the parsing structure
VALUE : '__value';
INTCONSTANT : [0-9]+;
VARIABLE : [a-zA-Z_] [a-zA-Z0-9_]*;
RELATIONALOPERATOR : '<' | '>' | '<=' | '>=' | '==' | '!=';
BINARYOPERATOR : '+' | '-' | '*' | '/' | '%' | '^';
NOT : '!';
OR : '||';
AND : '&&';
TRUE : 'true';
FALSE : 'false';

binaryoperator : BINARYOPERATOR; 
variable : VARIABLE;

valueExpression : 
	INTCONSTANT 			| 
	TRUE 					|
	FALSE 					|
	variable				|
	'(' valueExpression ')' |
	valueExpression binaryoperator valueExpression;

singleConstraint : 
	valueExpression RELATIONALOPERATOR valueExpression |
	NOT singleConstraint;

disjunctions : DisjunctiveConstraints+=singleConstraint (OR DisjunctiveConstraints+=singleConstraint)*;

conjunctiveNormalForm : ConjunctiveConstraints+=disjunctions (AND ConjunctiveConstraints+=disjunctions)*;

parse : conjunctiveNormalForm? EOF;
