
// Generated from RefinementGrammar.g4 by ANTLR 4.5.3

#pragma once


#include "antlr4-runtime.h"
#include "RefinementGrammarParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by RefinementGrammarParser.
 */
class RefinementGrammarVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by RefinementGrammarParser.
   */
    virtual antlrcpp::Any visitValueExpression(RefinementGrammarParser::ValueExpressionContext *context) = 0;

    virtual antlrcpp::Any visitSingleConstraint(RefinementGrammarParser::SingleConstraintContext *context) = 0;

    virtual antlrcpp::Any visitDisjunctions(RefinementGrammarParser::DisjunctionsContext *context) = 0;

    virtual antlrcpp::Any visitConjunctiveNormalForm(RefinementGrammarParser::ConjunctiveNormalFormContext *context) = 0;


};

