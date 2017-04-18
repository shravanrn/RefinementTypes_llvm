
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

tree::TerminalNode* RefinementGrammarParser::ValueExpressionContext::VARIABLE() {
  return getToken(RefinementGrammarParser::VARIABLE, 0);
}

std::vector<RefinementGrammarParser::ValueExpressionContext *> RefinementGrammarParser::ValueExpressionContext::valueExpression() {
  return getRuleContexts<RefinementGrammarParser::ValueExpressionContext>();
}

RefinementGrammarParser::ValueExpressionContext* RefinementGrammarParser::ValueExpressionContext::valueExpression(size_t i) {
  return getRuleContext<RefinementGrammarParser::ValueExpressionContext>(i);
}

tree::TerminalNode* RefinementGrammarParser::ValueExpressionContext::BINARYOPERATOR() {
  return getToken(RefinementGrammarParser::BINARYOPERATOR, 0);
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
  size_t startState = 0;
  enterRecursionRule(_localctx, 0, RefinementGrammarParser::RuleValueExpression, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(17);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case RefinementGrammarParser::INTCONSTANT: {
        setState(9);
        match(RefinementGrammarParser::INTCONSTANT);
        break;
      }

      case RefinementGrammarParser::TRUE: {
        setState(10);
        match(RefinementGrammarParser::TRUE);
        break;
      }

      case RefinementGrammarParser::FALSE: {
        setState(11);
        match(RefinementGrammarParser::FALSE);
        break;
      }

      case RefinementGrammarParser::VARIABLE: {
        setState(12);
        match(RefinementGrammarParser::VARIABLE);
        break;
      }

      case RefinementGrammarParser::T__0: {
        setState(13);
        match(RefinementGrammarParser::T__0);
        setState(14);
        valueExpression(0);
        setState(15);
        match(RefinementGrammarParser::T__1);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(24);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<ValueExpressionContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleValueExpression);
        setState(19);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(20);
        match(RefinementGrammarParser::BINARYOPERATOR);
        setState(21);
        valueExpression(2); 
      }
      setState(26);
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
  enterRule(_localctx, 2, RefinementGrammarParser::RuleSingleConstraint);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(33);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case RefinementGrammarParser::T__0:
      case RefinementGrammarParser::INTCONSTANT:
      case RefinementGrammarParser::VARIABLE:
      case RefinementGrammarParser::TRUE:
      case RefinementGrammarParser::FALSE: {
        enterOuterAlt(_localctx, 1);
        setState(27);
        valueExpression(0);
        setState(28);
        match(RefinementGrammarParser::RELATIONALOPERATOR);
        setState(29);
        valueExpression(0);
        break;
      }

      case RefinementGrammarParser::NOT: {
        enterOuterAlt(_localctx, 2);
        setState(31);
        match(RefinementGrammarParser::NOT);
        setState(32);
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
  enterRule(_localctx, 4, RefinementGrammarParser::RuleDisjunctions);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(35);
    dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext = singleConstraint();
    dynamic_cast<DisjunctionsContext *>(_localctx)->DisjunctiveConstraints.push_back(dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext);
    setState(40);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == RefinementGrammarParser::OR) {
      setState(36);
      match(RefinementGrammarParser::OR);
      setState(37);
      dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext = singleConstraint();
      dynamic_cast<DisjunctionsContext *>(_localctx)->DisjunctiveConstraints.push_back(dynamic_cast<DisjunctionsContext *>(_localctx)->singleConstraintContext);
      setState(42);
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
  enterRule(_localctx, 6, RefinementGrammarParser::RuleConjunctiveNormalForm);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(43);
    dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext = disjunctions();
    dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->ConjunctiveConstraints.push_back(dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext);
    setState(48);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == RefinementGrammarParser::AND) {
      setState(44);
      match(RefinementGrammarParser::AND);
      setState(45);
      dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext = disjunctions();
      dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->ConjunctiveConstraints.push_back(dynamic_cast<ConjunctiveNormalFormContext *>(_localctx)->disjunctionsContext);
      setState(50);
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

bool RefinementGrammarParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 0: return valueExpressionSempred(dynamic_cast<ValueExpressionContext *>(context), predicateIndex);

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
  "valueExpression", "singleConstraint", "disjunctions", "conjunctiveNormalForm"
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
    0x3, 0xf, 0x36, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 
    0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x5, 0x2, 0x14, 0xa, 0x2, 
    0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x7, 0x2, 0x19, 0xa, 0x2, 0xc, 0x2, 0xe, 
    0x2, 0x1c, 0xb, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x5, 0x3, 0x24, 0xa, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x7, 
    0x4, 0x29, 0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x2c, 0xb, 0x4, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x7, 0x5, 0x31, 0xa, 0x5, 0xc, 0x5, 0xe, 0x5, 0x34, 0xb, 
    0x5, 0x3, 0x5, 0x2, 0x3, 0x2, 0x6, 0x2, 0x4, 0x6, 0x8, 0x2, 0x2, 0x39, 
    0x2, 0x13, 0x3, 0x2, 0x2, 0x2, 0x4, 0x23, 0x3, 0x2, 0x2, 0x2, 0x6, 0x25, 
    0x3, 0x2, 0x2, 0x2, 0x8, 0x2d, 0x3, 0x2, 0x2, 0x2, 0xa, 0xb, 0x8, 0x2, 
    0x1, 0x2, 0xb, 0x14, 0x7, 0x7, 0x2, 0x2, 0xc, 0x14, 0x7, 0xe, 0x2, 0x2, 
    0xd, 0x14, 0x7, 0xf, 0x2, 0x2, 0xe, 0x14, 0x7, 0x8, 0x2, 0x2, 0xf, 0x10, 
    0x7, 0x3, 0x2, 0x2, 0x10, 0x11, 0x5, 0x2, 0x2, 0x2, 0x11, 0x12, 0x7, 
    0x4, 0x2, 0x2, 0x12, 0x14, 0x3, 0x2, 0x2, 0x2, 0x13, 0xa, 0x3, 0x2, 
    0x2, 0x2, 0x13, 0xc, 0x3, 0x2, 0x2, 0x2, 0x13, 0xd, 0x3, 0x2, 0x2, 0x2, 
    0x13, 0xe, 0x3, 0x2, 0x2, 0x2, 0x13, 0xf, 0x3, 0x2, 0x2, 0x2, 0x14, 
    0x1a, 0x3, 0x2, 0x2, 0x2, 0x15, 0x16, 0xc, 0x3, 0x2, 0x2, 0x16, 0x17, 
    0x7, 0xa, 0x2, 0x2, 0x17, 0x19, 0x5, 0x2, 0x2, 0x4, 0x18, 0x15, 0x3, 
    0x2, 0x2, 0x2, 0x19, 0x1c, 0x3, 0x2, 0x2, 0x2, 0x1a, 0x18, 0x3, 0x2, 
    0x2, 0x2, 0x1a, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x1b, 0x3, 0x3, 0x2, 0x2, 
    0x2, 0x1c, 0x1a, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x1e, 0x5, 0x2, 0x2, 0x2, 
    0x1e, 0x1f, 0x7, 0x9, 0x2, 0x2, 0x1f, 0x20, 0x5, 0x2, 0x2, 0x2, 0x20, 
    0x24, 0x3, 0x2, 0x2, 0x2, 0x21, 0x22, 0x7, 0xb, 0x2, 0x2, 0x22, 0x24, 
    0x5, 0x4, 0x3, 0x2, 0x23, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x23, 0x21, 0x3, 
    0x2, 0x2, 0x2, 0x24, 0x5, 0x3, 0x2, 0x2, 0x2, 0x25, 0x2a, 0x5, 0x4, 
    0x3, 0x2, 0x26, 0x27, 0x7, 0xc, 0x2, 0x2, 0x27, 0x29, 0x5, 0x4, 0x3, 
    0x2, 0x28, 0x26, 0x3, 0x2, 0x2, 0x2, 0x29, 0x2c, 0x3, 0x2, 0x2, 0x2, 
    0x2a, 0x28, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x2b, 
    0x7, 0x3, 0x2, 0x2, 0x2, 0x2c, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x32, 
    0x5, 0x6, 0x4, 0x2, 0x2e, 0x2f, 0x7, 0xd, 0x2, 0x2, 0x2f, 0x31, 0x5, 
    0x6, 0x4, 0x2, 0x30, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x31, 0x34, 0x3, 0x2, 
    0x2, 0x2, 0x32, 0x30, 0x3, 0x2, 0x2, 0x2, 0x32, 0x33, 0x3, 0x2, 0x2, 
    0x2, 0x33, 0x9, 0x3, 0x2, 0x2, 0x2, 0x34, 0x32, 0x3, 0x2, 0x2, 0x2, 
    0x7, 0x13, 0x1a, 0x23, 0x2a, 0x32, 
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
