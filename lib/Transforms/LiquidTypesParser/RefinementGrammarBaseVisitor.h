
// Generated from RefinementGrammar.g4 by ANTLR 4.5.3

#pragma once


#include "antlr4-runtime.h"
#include "RefinementGrammarVisitor.h"


/**
 * This class provides an empty implementation of RefinementGrammarVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class RefinementGrammarBaseVisitor : public RefinementGrammarVisitor {
public:

  virtual antlrcpp::Any visitValueExpression(RefinementGrammarParser::ValueExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSingleConstraint(RefinementGrammarParser::SingleConstraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDisjunctions(RefinementGrammarParser::DisjunctionsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConjunctiveNormalForm(RefinementGrammarParser::ConjunctiveNormalFormContext *ctx) override {
    return visitChildren(ctx);
  }


};

