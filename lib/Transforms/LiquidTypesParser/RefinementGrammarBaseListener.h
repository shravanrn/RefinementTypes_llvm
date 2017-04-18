
// Generated from RefinementGrammar.g4 by ANTLR 4.5.3

#pragma once


#include "antlr4-runtime.h"
#include "RefinementGrammarListener.h"


/**
 * This class provides an empty implementation of RefinementGrammarListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class RefinementGrammarBaseListener : public RefinementGrammarListener {
public:

  virtual void enterValueExpression(RefinementGrammarParser::ValueExpressionContext * /*ctx*/) override { }
  virtual void exitValueExpression(RefinementGrammarParser::ValueExpressionContext * /*ctx*/) override { }

  virtual void enterSingleConstraint(RefinementGrammarParser::SingleConstraintContext * /*ctx*/) override { }
  virtual void exitSingleConstraint(RefinementGrammarParser::SingleConstraintContext * /*ctx*/) override { }

  virtual void enterDisjunctions(RefinementGrammarParser::DisjunctionsContext * /*ctx*/) override { }
  virtual void exitDisjunctions(RefinementGrammarParser::DisjunctionsContext * /*ctx*/) override { }

  virtual void enterConjunctiveNormalForm(RefinementGrammarParser::ConjunctiveNormalFormContext * /*ctx*/) override { }
  virtual void exitConjunctiveNormalForm(RefinementGrammarParser::ConjunctiveNormalFormContext * /*ctx*/) override { }


  virtual void enterEveryRule(ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(tree::ErrorNode * /*node*/) override { }

};

