#pragma once

#include "rule.h"
#include "lexeme.h"

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
	const LexemeList& m_lexemes;

	void AnalyzeLLNaive();
	void AnalyzeLLStack();

	const Rule::Condition& ExpandStack(Stack& stack, const Lexeme& prefix);

public:
	SyntaxAnalyzer(const Grammar& grammar, const LexemeList& lexemes);

    void Process(AnalyzeMode mode);
};
