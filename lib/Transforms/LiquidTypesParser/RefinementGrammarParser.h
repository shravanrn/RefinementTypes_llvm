
// Generated from RefinementGrammar.g4 by ANTLR 4.5.3

#pragma once


#include "antlr4-runtime.h"


using namespace antlr4;



class RefinementGrammarParser : public Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, WS = 3, VALUE = 4, INTCONSTANT = 5, VARIABLE = 6, 
    RELATIONALOPERATOR = 7, BINARYOPERATOR = 8, NOT = 9, OR = 10, AND = 11, 
    TRUE = 12, FALSE = 13
  };

  enum {
    RuleBinaryoperator = 0, RuleVariable = 1, RuleValueExpression = 2, RuleSingleConstraint = 3, 
    RuleDisjunctions = 4, RuleConjunctiveNormalForm = 5, RuleParse = 6
  };

  RefinementGrammarParser(TokenStream *input);
  ~RefinementGrammarParser();

  virtual std::string getGrammarFileName() const override;
  virtual const atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual dfa::Vocabulary& getVocabulary() const override;


  class BinaryoperatorContext;
  class VariableContext;
  class ValueExpressionContext;
  class SingleConstraintContext;
  class DisjunctionsContext;
  class ConjunctiveNormalFormContext;
  class ParseContext; 

  class BinaryoperatorContext : public ParserRuleContext {
  public:
    BinaryoperatorContext(ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    tree::TerminalNode *BINARYOPERATOR();

    virtual void enterRule(tree::ParseTreeListener *listener) override;
    virtual void exitRule(tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(tree::ParseTreeVisitor *visitor) override;
   
  };

  BinaryoperatorContext* binaryoperator();

  class VariableContext : public ParserRuleContext {
  public:
    VariableContext(ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    tree::TerminalNode *VARIABLE();

    virtual void enterRule(tree::ParseTreeListener *listener) override;
    virtual void exitRule(tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableContext* variable();

  class ValueExpressionContext : public ParserRuleContext {
  public:
    ValueExpressionContext(ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    tree::TerminalNode *INTCONSTANT();
    tree::TerminalNode *TRUE();
    tree::TerminalNode *FALSE();
    tree::TerminalNode *VALUE();
    VariableContext *variable();
    std::vector<ValueExpressionContext *> valueExpression();
    ValueExpressionContext* valueExpression(size_t i);
    BinaryoperatorContext *binaryoperator();

    virtual void enterRule(tree::ParseTreeListener *listener) override;
    virtual void exitRule(tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueExpressionContext* valueExpression();
  ValueExpressionContext* valueExpression(int precedence);
  class SingleConstraintContext : public ParserRuleContext {
  public:
    SingleConstraintContext(ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ValueExpressionContext *> valueExpression();
    ValueExpressionContext* valueExpression(size_t i);
    tree::TerminalNode *RELATIONALOPERATOR();
    tree::TerminalNode *NOT();
    SingleConstraintContext *singleConstraint();

    virtual void enterRule(tree::ParseTreeListener *listener) override;
    virtual void exitRule(tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(tree::ParseTreeVisitor *visitor) override;
   
  };

  SingleConstraintContext* singleConstraint();

  class DisjunctionsContext : public ParserRuleContext {
  public:
    RefinementGrammarParser::SingleConstraintContext *singleConstraintContext = nullptr;;
    std::vector<SingleConstraintContext *> DisjunctiveConstraints;;
    DisjunctionsContext(ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SingleConstraintContext *> singleConstraint();
    SingleConstraintContext* singleConstraint(size_t i);
    std::vector<tree::TerminalNode *> OR();
    tree::TerminalNode* OR(size_t i);

    virtual void enterRule(tree::ParseTreeListener *listener) override;
    virtual void exitRule(tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(tree::ParseTreeVisitor *visitor) override;
   
  };

  DisjunctionsContext* disjunctions();

  class ConjunctiveNormalFormContext : public ParserRuleContext {
  public:
    RefinementGrammarParser::DisjunctionsContext *disjunctionsContext = nullptr;;
    std::vector<DisjunctionsContext *> ConjunctiveConstraints;;
    ConjunctiveNormalFormContext(ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DisjunctionsContext *> disjunctions();
    DisjunctionsContext* disjunctions(size_t i);
    std::vector<tree::TerminalNode *> AND();
    tree::TerminalNode* AND(size_t i);

    virtual void enterRule(tree::ParseTreeListener *listener) override;
    virtual void exitRule(tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(tree::ParseTreeVisitor *visitor) override;
   
  };

  ConjunctiveNormalFormContext* conjunctiveNormalForm();

  class ParseContext : public ParserRuleContext {
  public:
    ParseContext(ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    tree::TerminalNode *EOF();
    ConjunctiveNormalFormContext *conjunctiveNormalForm();

    virtual void enterRule(tree::ParseTreeListener *listener) override;
    virtual void exitRule(tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(tree::ParseTreeVisitor *visitor) override;
   
  };

  ParseContext* parse();


  virtual bool sempred(RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;
  bool valueExpressionSempred(ValueExpressionContext *_localctx, size_t predicateIndex);

private:
  static std::vector<dfa::DFA> _decisionToDFA;
  static atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static dfa::Vocabulary _vocabulary;
  static atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

