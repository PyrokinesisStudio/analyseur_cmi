#pragma once

#include "rule.h"

class Grammar;
class Stack;

class SyntaxAnalyzer
{
public:
    enum AnalyzeMode {
        LL_NAIVE = 0,
        LL_STACK
    };

private:
	const Grammar& m_grammar;
	const StringList& m_tokens;

	void AnalyzeLLNaive();
	void AnalyzeLLStack();

	const Rule::Lexeme& ExpandStack(Stack& stack);

public:
	SyntaxAnalyzer(const Grammar& grammar, const StringList& tokens);

    void Process(AnalyzeMode mode);
};
