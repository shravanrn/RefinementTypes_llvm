
// Generated from RefinementGrammar.g4 by ANTLR 4.5.3

#pragma once


#include "antlr4-runtime.h"
#include "RefinementGrammarParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by RefinementGrammarParser.
 */
class RefinementGrammarListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterValueExpression(RefinementGrammarParser::ValueExpressionContext *ctx) = 0;
  virtual void exitValueExpression(RefinementGrammarParser::ValueExpressionContext *ctx) = 0;

  virtual void enterSingleConstraint(RefinementGrammarParser::SingleConstraintContext *ctx) = 0;
  virtual void exitSingleConstraint(RefinementGrammarParser::SingleConstraintContext *ctx) = 0;

  virtual void enterDisjunctions(RefinementGrammarParser::DisjunctionsContext *ctx) = 0;
  virtual void exitDisjunctions(RefinementGrammarParser::DisjunctionsContext *ctx) = 0;

  virtual void enterConjunctiveNormalForm(RefinementGrammarParser::ConjunctiveNormalFormContext *ctx) = 0;
  virtual void exitConjunctiveNormalForm(RefinementGrammarParser::ConjunctiveNormalFormContext *ctx) = 0;


};

