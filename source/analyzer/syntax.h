#pragma once

#include "rule.h"
#include "lexeme.h"
#include "derivation_tree.h"

class Grammar;
class Stack;

class SyntaxAnalyzer
{
public:
    enum AnalyzeMode {
        LL_NAIVE = 0,
        LL_STACK,
		SLR
    };

private:
	enum StackAction {
		STACK_NO_PROPOSALS,
		STACK_EXPANDED,
		STACK_UNCHANGED
	};

	const Grammar& m_grammar;
	const LexemeList& m_lexemes;

	void AnalyzeLlNaive(const Rule& root, const Rule::Condition& rootCond, DerivationNode& derivationRoot,
            LexemeList::const_iterator& it, const LexemeList::const_iterator& end) const;
	void AnalyzeLlStack(const Rule& root, const Rule::Condition& rootCond, DerivationNode& derivationRoot,
            LexemeList::const_iterator& it, const LexemeList::const_iterator& end) const;
	void AnalyzeSlr(const Rule& root, const Rule::Condition& rootCond, DerivationNode& derivationRoot,
            LexemeList::const_iterator& it, const LexemeList::const_iterator& end) const;

	StackAction ExpandStack(Stack& stack, const Lexeme& prefix) const;

public:
	SyntaxAnalyzer(const Grammar& grammar, const LexemeList& lexemes);

    void Process(AnalyzeMode mode) const;
};
