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

WS : [ \t\r\n]+ -> skip ; // skip spaces, tabs, newlines
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

valueExpression : 
	INTCONSTANT 			| 
	TRUE 					|
	FALSE 					|
	VARIABLE				|
	'(' valueExpression ')' |
	valueExpression BINARYOPERATOR valueExpression;

singleConstraint : 
	valueExpression RELATIONALOPERATOR valueExpression |
	NOT singleConstraint;

disjunctions : DisjunctiveConstraints+=singleConstraint (OR DisjunctiveConstraints+=singleConstraint)*;

conjunctiveNormalForm : ConjunctiveConstraints+=disjunctions (AND ConjunctiveConstraints+=disjunctions)*;