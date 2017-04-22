
// Generated from RefinementGrammar.g4 by ANTLR 4.5.3


#include "RefinementGrammarListener.h"
#include "RefinementGrammarVisitor.h"

#include "RefinementGrammarParser.h"


using namespace antlrcpp;
using namespace antlr4;

RefinementGrammarParser::RefinementGrammarParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

RefinementGrammarParser::~RefinementGrammarParser() {
  delete _interpreter;
}

std::string RefinementGrammarParser::getGrammarFileName() const {
  return "RefinementGrammar.g4";
}

const std::vector<std::string>& RefinementGrammarParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& RefinementGrammarParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- BinaryoperatorContext ------------------------------------------------------------------

RefinementGrammarParser::BinaryoperatorContext::BinaryoperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* RefinementGrammarParser::BinaryoperatorContext::BINARYOPERATOR() {
  return getToken(RefinementGrammarParser::BINARYOPERATOR, 0);
}


size_t RefinementGrammarParser::BinaryoperatorContext::getRuleIndex() const {
  return RefinementGrammarParser::RuleBinaryoperator;
}

void RefinementGrammarParser::BinaryoperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBinaryoperator(this);
}

void RefinementGrammarParser::BinaryoperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBinaryoperator(this);
}


antlrcpp::Any RefinementGrammarParser::BinaryoperatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (dynamic_cast<RefinementGrammarVisitor*>(visitor) != nullptr)
    return ((RefinementGrammarVisitor *)visitor)->visitBinaryoperator(this);
  else
    return visitor->visitChildren(this);
}

RefinementGrammarParser::BinaryoperatorContext* RefinementGrammarParser::binaryoperator() {
  BinaryoperatorContext *_localctx = _tracker.createInstance<BinaryoperatorContext>(_ctx, getState());
  enterRule(_localctx, 0, RefinementGrammarParser::RuleBinaryoperator);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(14);
    match(RefinementGrammarParser::BINARYOPERATOR);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableContext ------------------------------------------------------------------

RefinementGrammarParser::VariableContext::VariableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* RefinementGrammarParser::VariableContext::VARIABLE() {
  return getToken(RefinementGrammarParser::VARIABLE, 0);
}


size_t RefinementGrammarParser::VariableContext::getRuleIndex() const {
  return RefinementGrammarParser::RuleVariable;
}

void RefinementGrammarParser::VariableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariable(this);
}

void RefinementGrammarParser::VariableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariable(this);
}


antlrcpp::Any RefinementGrammarParser::VariableContext::accept(tree::ParseTreeVisitor *visitor) {
  if (dynamic_cast<RefinementGrammarVisitor*>(visitor) != nullptr)
    return ((RefinementGrammarVisitor *)visitor)->visitVariable(this);
  else
    return visitor->visitChildren(this);
}

RefinementGrammarParser::VariableContext* RefinementGrammarParser::variable() {
  VariableContext *_localctx = _tracker.createInstance<VariableContext>(_ctx, getState());
  enterRule(_localctx, 2, RefinementGrammarParser::RuleVariable);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(16);
    match(RefinementGrammarParser::VARIABLE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueExpressionContext ------------------------------------------------------------------

RefinementGrammarParser::ValueExpressionContext::ValueExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* RefinementGrammarParser::ValueExpressionContext::INTCONSTANT() {
  return getToken(RefinementGrammarParser::INTCONSTANT, 0);
}

tree::TerminalNode* RefinementGrammarParser::ValueExpressionContext::TRUE() {
  return getToken(RefinementGrammarParser::TRUE, 0);
}

tree::TerminalNode* RefinementGrammarParser::ValueExpressionContext::FALSE() {
  return getToken(RefinementGrammarParser::FALSE, 0);
}

RefinementGrammarParser::VariableContext* RefinementGrammarParser::ValueExpressionContext::variable() {
  return getRuleContext<RefinementGrammarParser::VariableContext>(0);
}

std::vector<RefinementGrammarParser::ValueExpressionContext *> RefinementGrammarParser::ValueExpressionContext::valueExpression() {
  return getRuleContexts<RefinementGrammarParser::ValueExpressionContext>();
}

RefinementGrammarParser::ValueExpressionContext* RefinementGrammarParser::ValueExpressionContext::valueExpression(size_t i) {
  return getRuleContext<RefinementGrammarParser::ValueExpressionContext>(i);
}

RefinementGrammarParser::BinaryoperatorContext* RefinementGrammarParser::ValueExpressionContext::binaryoperator() {
  return getRuleContext<RefinementGrammarParser::BinaryoperatorContext>(0);
}


size_t RefinementGrammarParser::ValueExpressionContext::getRuleIndex() const {
  return RefinementGrammarParser::RuleValueExpression;
}

void RefinementGrammarParser::ValueExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueExpression(this);
}

void RefinementGrammarParser::ValueExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueExpression(this);
}


antlrcpp::Any RefinementGrammarParser::ValueExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (dynamic_cast<RefinementGrammarVisitor*>(visitor) != nullptr)
    return ((RefinementGrammarVisitor *)visitor)->visitValueExpression(this);
  else
    return visitor->visitChildren(this);
}


RefinementGrammarParser::ValueExpressionContext* RefinementGrammarParser::valueExpression() {
   return valueExpression(0);
}

RefinementGrammarParser::ValueExpressionContext* RefinementGrammarParser::valueExpression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  RefinementGrammarParser::ValueExpressionContext *_localctx = _tracker.createInstance<ValueExpressionContext>(_ctx, parentState);
  RefinementGrammarParser::ValueExpressionContext *previousContext = _localctx;
  size_t startState = 4;
  enterRecursionRule(_localctx, 4, RefinementGrammarParser::RuleValueExpression, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(27);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case RefinementGrammarParser::INTCONSTANT: {
        setState(19);
        match(RefinementGrammarParser::INTCONSTANT);
        break;
      }

      case RefinementGrammarParser::TRUE: {
        setState(20);
        match(RefinementGrammarParser::TRUE);
        break;
      }

      case RefinementGrammarParser::FALSE: {
        setState(21);
        match(RefinementGrammarParser::FALSE);
        break;
      }

      case RefinementGrammarParser::VARIABLE: {
        setState(22);
        variable();
        break;
      }

      case RefinementGrammarParser::T__0: {
        setState(23);
        match(RefinementGrammarParser::T__0);
        setState(24);
        valueExpression(0);
        setState(25);
        match(RefinementGrammarParser::T__1);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(35);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<ValueExpressionContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleValueExpression);
        setState(29);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(30);
        binaryoperator();
        setState(31);
        valueExpression(2); 
      }
      setState(37);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- SingleConstraintContext ------------------------------------------------------------------

RefinementGrammarParser::SingleConstraintContext::SingleConstraintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<RefinementGrammarParser::ValueExpressionContext *> RefinementGrammarParser::SingleConstraintContext::valueExpression() {
  return getRuleContexts<RefinementGrammarParser::ValueExpressionContext>();
}

RefinementGrammarParser::ValueExpressionContext* RefinementGrammarParser::SingleConstraintContext::valueExpression(size_t i) {
  return getRuleContext<RefinementGrammarParser::ValueExpressionContext>(i);
}

tree::TerminalNode* RefinementGrammarParser::SingleConstraintContext::RELATIONALOPERATOR() {
  return getToken(RefinementGrammarParser::RELATIONALOPERATOR, 0);
}

tree::TerminalNode* RefinementGrammarParser::SingleConstraintContext::NOT() {
  return getToken(RefinementGrammarParser::NOT, 0);
}

RefinementGrammarParser::SingleConstraintContext* RefinementGrammarParser::SingleConstraintContext::singleConstraint() {
  return getRuleContext<RefinementGrammarParser::SingleConstraintContext>(0);
}


size_t RefinementGrammarParser::SingleConstraintContext::getRuleIndex() const {
  return RefinementGrammarParser::RuleSingleConstraint;
}

void RefinementGrammarParser::SingleConstraintContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSingleConstraint(this);
}

void RefinementGrammarParser::SingleConstraintContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSingleConstraint(this);
}


antlrcpp::Any RefinementGrammarParser::SingleConstraintContext::accept(tree::ParseTreeVisitor *visitor) {
  if (dynamic_cast<RefinementGrammarVisitor*>(visitor) != nullptr)
    return ((RefinementGrammarVisitor *)visitor)->visitSingleConstraint(this);
  else
    return visitor->visitChildren(this);
}

RefinementGrammarParser::SingleConstraintContext* RefinementGrammarParser::singleConstraint() {
  SingleConstraintContext *_localctx = _tracker.createInstance<SingleConstraintContext>(_ctx, getState());
  enterRule(_localctx, 6, RefinementGrammarParser::RuleSingleConstraint);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(44);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case RefinementGrammarParser::T__0:
      case RefinementGrammarParser::INTCONSTANT:
      case RefinementGrammarParser::VARIABLE:
      case RefinementGrammarParser::TRUE:
      case RefinementGrammarParser::FALSE: {
        enterOuterAlt(_localctx, 1);
        setState(38);
        valueExpression(0);
        setState(39);
        match(RefinementGrammarParser::RELATIONALOPERATOR);
        setState(40);
        valueExpression(0);
        break;
      }

      case RefinementGrammarParser::NOT: {
        enterOuterAlt(_localctx, 2);
        setState(42);
        match(RefinementGrammarParser::NOT);
        setState(43);
        singleConstraint();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DisjunctionsContext ------------------------------------------------------------------

RefinementGrammarParser::DisjunctionsContext::DisjunctionsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<RefinementGrammarParser::SingleConstraintContext *> RefinementGrammarParser::DisjunctionsContext::singleConstraint() {
  return getRuleContexts<RefinementGrammarParser::SingleConstraintContext>();
}

RefinementGrammarParser::SingleConstraintContext* RefinementGrammarParser::DisjunctionsContext::singleConstraint(size_t i) {
  return getRuleContext<RefinementGrammarParser::SingleConstraintContext>(i);
}

std::vector<tree::TerminalNode *> RefinementGrammarParser::DisjunctionsContext::OR() {
  return getTokens(RefinementGrammarParser::OR);
}

tree::TerminalNode* RefinementGrammarParser::DisjunctionsContext::OR(size_t i) {
  return getToken(RefinementGrammarParser::OR, i);
}


size_t RefinementGrammarParser::DisjunctionsContext::getRuleIndex() const {
  return RefinementGrammarParser::RuleDisjunctions;
}

void RefinementGrammarParser::DisjunctionsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDisjunctions(this);
}

void RefinementGrammarParser::DisjunctionsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDisjunctions(this);
}


antlrcpp::Any RefinementGrammarParser::DisjunctionsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (dynamic_cast<RefinementGrammarVisitor*>(visitor) != nullptr)
    return ((RefinementGrammarVisitor *)visitor)->visitDisjunctions(this);
  else
    return visitor->visitChildren(this);
}

RefinementGrammarParser::DisjunctionsContext* RefinementGrammarParser::disjunctions() {
  DisjunctionsContext *_localctx = _tracker.createInstance<DisjunctionsContext>(_ctx, getState());
  enterRule(_localctx, 8, RefinementGrammarParser::RuleDisjunctions);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(46);
    dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext = singleConstraint();
    dynamic_cast<DisjunctionsContext *>(_localctx)->DisjunctiveConstraints.push_back(dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext);
    setState(51);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == RefinementGrammarParser::OR) {
      setState(47);
      match(RefinementGrammarParser::OR);
      setState(48);
      dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext = singleConstraint();
      dynamic_cast<DisjunctionsContext *>(_localctx)->DisjunctiveConstraints.push_back(dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext);
      setState(53);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConjunctiveNormalFormContext ------------------------------------------------------------------

RefinementGrammarParser::ConjunctiveNormalFormContext::ConjunctiveNormalFormContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<RefinementGrammarParser::DisjunctionsContext *> RefinementGrammarParser::ConjunctiveNormalFormContext::disjunctions() {
  return getRuleContexts<RefinementGrammarParser::DisjunctionsContext>();
}

RefinementGrammarParser::DisjunctionsContext* RefinementGrammarParser::ConjunctiveNormalFormContext::disjunctions(size_t i) {
  return getRuleContext<RefinementGrammarParser::DisjunctionsContext>(i);
}

std::vector<tree::TerminalNode *> RefinementGrammarParser::ConjunctiveNormalFormContext::AND() {
  return getTokens(RefinementGrammarParser::AND);
}

tree::TerminalNode* RefinementGrammarParser::ConjunctiveNormalFormContext::AND(size_t i) {
  return getToken(RefinementGrammarParser::AND, i);
}


size_t RefinementGrammarParser::ConjunctiveNormalFormContext::getRuleIndex() const {
  return RefinementGrammarParser::RuleConjunctiveNormalForm;
}

void RefinementGrammarParser::ConjunctiveNormalFormContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConjunctiveNormalForm(this);
}

void RefinementGrammarParser::ConjunctiveNormalFormContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConjunctiveNormalForm(this);
}


antlrcpp::Any RefinementGrammarParser::ConjunctiveNormalFormContext::accept(tree::ParseTreeVisitor *visitor) {
  if (dynamic_cast<RefinementGrammarVisitor*>(visitor) != nullptr)
    return ((RefinementGrammarVisitor *)visitor)->visitConjunctiveNormalForm(this);
  else
    return visitor->visitChildren(this);
}

RefinementGrammarParser::ConjunctiveNormalFormContext* RefinementGrammarParser::conjunctiveNormalForm() {
  ConjunctiveNormalFormContext *_localctx = _tracker.createInstance<ConjunctiveNormalFormContext>(_ctx, getState());
  enterRule(_localctx, 10, RefinementGrammarParser::RuleConjunctiveNormalForm);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(54);
    dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext = disjunctions();
    dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->ConjunctiveConstraints.push_back(dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext);
    setState(59);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == RefinementGrammarParser::AND) {
      setState(55);
      match(RefinementGrammarParser::AND);
      setState(56);
      dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext = disjunctions();
      dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->ConjunctiveConstraints.push_back(dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext);
      setState(61);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParseContext ------------------------------------------------------------------

RefinementGrammarParser::ParseContext::ParseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* RefinementGrammarParser::ParseContext::EOF() {
  return getToken(RefinementGrammarParser::EOF, 0);
}

RefinementGrammarParser::ConjunctiveNormalFormContext* RefinementGrammarParser::ParseContext::conjunctiveNormalForm() {
  return getRuleContext<RefinementGrammarParser::ConjunctiveNormalFormContext>(0);
}


size_t RefinementGrammarParser::ParseContext::getRuleIndex() const {
  return RefinementGrammarParser::RuleParse;
}

void RefinementGrammarParser::ParseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParse(this);
}

void RefinementGrammarParser::ParseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<RefinementGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParse(this);
}


antlrcpp::Any RefinementGrammarParser::ParseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (dynamic_cast<RefinementGrammarVisitor*>(visitor) != nullptr)
    return ((RefinementGrammarVisitor *)visitor)->visitParse(this);
  else
    return visitor->visitChildren(this);
}

RefinementGrammarParser::ParseContext* RefinementGrammarParser::parse() {
  ParseContext *_localctx = _tracker.createInstance<ParseContext>(_ctx, getState());
  enterRule(_localctx, 12, RefinementGrammarParser::RuleParse);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(63);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << RefinementGrammarParser::T__0)
      | (1ULL << RefinementGrammarParser::INTCONSTANT)
      | (1ULL << RefinementGrammarParser::VARIABLE)
      | (1ULL << RefinementGrammarParser::NOT)
      | (1ULL << RefinementGrammarParser::TRUE)
      | (1ULL << RefinementGrammarParser::FALSE))) != 0)) {
      setState(62);
      conjunctiveNormalForm();
    }
    setState(65);
    match(RefinementGrammarParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool RefinementGrammarParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 2: return valueExpressionSempred(dynamic_cast<ValueExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool RefinementGrammarParser::valueExpressionSempred(ValueExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> RefinementGrammarParser::_decisionToDFA;
atn::PredictionContextCache RefinementGrammarParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN RefinementGrammarParser::_atn;
std::vector<uint16_t> RefinementGrammarParser::_serializedATN;

std::vector<std::string> RefinementGrammarParser::_ruleNames = {
  "binaryoperator", "variable", "valueExpression", "singleConstraint", "disjunctions", 
  "conjunctiveNormalForm", "parse"
};

std::vector<std::string> RefinementGrammarParser::_literalNames = {
  "", "'('", "')'", "", "'__value'", "", "", "", "", "'!'", "'||'", "'&&'", 
  "'true'", "'false'"
};

std::vector<std::string> RefinementGrammarParser::_symbolicNames = {
  "", "", "", "WS", "VALUE", "INTCONSTANT", "VARIABLE", "RELATIONALOPERATOR", 
  "BINARYOPERATOR", "NOT", "OR", "AND", "TRUE", "FALSE"
};

dfa::Vocabulary RefinementGrammarParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> RefinementGrammarParser::_tokenNames;

RefinementGrammarParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x430, 0xd6d1, 0x8206, 0xad2d, 0x4417, 0xaef1, 0x8d80, 0xaadd, 
    0x3, 0xf, 0x46, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 
    0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 
    0x4, 0x5, 0x4, 0x1e, 0xa, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x7, 0x4, 0x24, 0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x27, 0xb, 0x4, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x2f, 0xa, 
    0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x7, 0x6, 0x34, 0xa, 0x6, 0xc, 0x6, 
    0xe, 0x6, 0x37, 0xb, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x7, 0x7, 0x3c, 
    0xa, 0x7, 0xc, 0x7, 0xe, 0x7, 0x3f, 0xb, 0x7, 0x3, 0x8, 0x5, 0x8, 0x42, 
    0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x2, 0x3, 0x6, 0x9, 0x2, 0x4, 
    0x6, 0x8, 0xa, 0xc, 0xe, 0x2, 0x2, 0x47, 0x2, 0x10, 0x3, 0x2, 0x2, 0x2, 
    0x4, 0x12, 0x3, 0x2, 0x2, 0x2, 0x6, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x8, 0x2e, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x30, 0x3, 0x2, 0x2, 0x2, 0xc, 0x38, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0x41, 0x3, 0x2, 0x2, 0x2, 0x10, 0x11, 0x7, 0xa, 0x2, 
    0x2, 0x11, 0x3, 0x3, 0x2, 0x2, 0x2, 0x12, 0x13, 0x7, 0x8, 0x2, 0x2, 
    0x13, 0x5, 0x3, 0x2, 0x2, 0x2, 0x14, 0x15, 0x8, 0x4, 0x1, 0x2, 0x15, 
    0x1e, 0x7, 0x7, 0x2, 0x2, 0x16, 0x1e, 0x7, 0xe, 0x2, 0x2, 0x17, 0x1e, 
    0x7, 0xf, 0x2, 0x2, 0x18, 0x1e, 0x5, 0x4, 0x3, 0x2, 0x19, 0x1a, 0x7, 
    0x3, 0x2, 0x2, 0x1a, 0x1b, 0x5, 0x6, 0x4, 0x2, 0x1b, 0x1c, 0x7, 0x4, 
    0x2, 0x2, 0x1c, 0x1e, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x14, 0x3, 0x2, 0x2, 
    0x2, 0x1d, 0x16, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x17, 0x3, 0x2, 0x2, 0x2, 
    0x1d, 0x18, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x19, 0x3, 0x2, 0x2, 0x2, 0x1e, 
    0x25, 0x3, 0x2, 0x2, 0x2, 0x1f, 0x20, 0xc, 0x3, 0x2, 0x2, 0x20, 0x21, 
    0x5, 0x2, 0x2, 0x2, 0x21, 0x22, 0x5, 0x6, 0x4, 0x4, 0x22, 0x24, 0x3, 
    0x2, 0x2, 0x2, 0x23, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x24, 0x27, 0x3, 0x2, 
    0x2, 0x2, 0x25, 0x23, 0x3, 0x2, 0x2, 0x2, 0x25, 0x26, 0x3, 0x2, 0x2, 
    0x2, 0x26, 0x7, 0x3, 0x2, 0x2, 0x2, 0x27, 0x25, 0x3, 0x2, 0x2, 0x2, 
    0x28, 0x29, 0x5, 0x6, 0x4, 0x2, 0x29, 0x2a, 0x7, 0x9, 0x2, 0x2, 0x2a, 
    0x2b, 0x5, 0x6, 0x4, 0x2, 0x2b, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x2c, 0x2d, 
    0x7, 0xb, 0x2, 0x2, 0x2d, 0x2f, 0x5, 0x8, 0x5, 0x2, 0x2e, 0x28, 0x3, 
    0x2, 0x2, 0x2, 0x2e, 0x2c, 0x3, 0x2, 0x2, 0x2, 0x2f, 0x9, 0x3, 0x2, 
    0x2, 0x2, 0x30, 0x35, 0x5, 0x8, 0x5, 0x2, 0x31, 0x32, 0x7, 0xc, 0x2, 
    0x2, 0x32, 0x34, 0x5, 0x8, 0x5, 0x2, 0x33, 0x31, 0x3, 0x2, 0x2, 0x2, 
    0x34, 0x37, 0x3, 0x2, 0x2, 0x2, 0x35, 0x33, 0x3, 0x2, 0x2, 0x2, 0x35, 
    0x36, 0x3, 0x2, 0x2, 0x2, 0x36, 0xb, 0x3, 0x2, 0x2, 0x2, 0x37, 0x35, 
    0x3, 0x2, 0x2, 0x2, 0x38, 0x3d, 0x5, 0xa, 0x6, 0x2, 0x39, 0x3a, 0x7, 
    0xd, 0x2, 0x2, 0x3a, 0x3c, 0x5, 0xa, 0x6, 0x2, 0x3b, 0x39, 0x3, 0x2, 
    0x2, 0x2, 0x3c, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x3d, 0x3b, 0x3, 0x2, 0x2, 
    0x2, 0x3d, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x3e, 0xd, 0x3, 0x2, 0x2, 0x2, 
    0x3f, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x40, 0x42, 0x5, 0xc, 0x7, 0x2, 0x41, 
    0x40, 0x3, 0x2, 0x2, 0x2, 0x41, 0x42, 0x3, 0x2, 0x2, 0x2, 0x42, 0x43, 
    0x3, 0x2, 0x2, 0x2, 0x43, 0x44, 0x7, 0x2, 0x2, 0x3, 0x44, 0xf, 0x3, 
    0x2, 0x2, 0x2, 0x8, 0x1d, 0x25, 0x2e, 0x35, 0x3d, 0x41, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

RefinementGrammarParser::Initializer RefinementGrammarParser::_init;
