#include "analyzer.h"
#include "grammar.h"

#include "stack.h"

Analyzer::Analyzer(const Grammar& grammar, const StringList& tokens)
	:m_grammar(grammar),
	m_tokens(tokens)
{
}

void Analyzer::Analyze(AnalyzeMode mode)
{
    void (Analyzer::*functions[])() = {
        &Analyzer::AnalyzeLLNaive, // LL_NAIVE
        &Analyzer::AnalyzeLLStack // LL_STACK
    };

    (this->*functions[mode])();
}

void Analyzer::AnalyzeLLNaive()
{
	const Rule& root = m_grammar.GetRule("root");
	StringList::const_iterator it = m_tokens.begin();
	root.Match(m_grammar, it, m_tokens);

	if (it != m_tokens.end()) {
		ErrorM("Failed to match entire expression.")
	}
}

void Analyzer::AnalyzeLLStack()
{
	const Rule& root = m_grammar.GetRule("root");
	StringList::const_iterator it = m_tokens.begin();
	StringList::const_iterator end = m_tokens.end();

	Stack stack;
	stack.ExpandTop(root);

	/*for (unsigned short i = 0; i < 50; ++i) {
		const Rule::Lexeme& top = ExpandStack(stack);
		stack.Debug();

		if (i == 2 || i == 30) {
			std::cout << "validate: " << stack.ValidateTop() << std::endl;
		}
		else {
			stack.ChangeTop(false);
		}
	}*/

	while (true) {
		const Rule::Lexeme& top = ExpandStack(stack);
		for (StringList::const_iterator jt = it; jt != end; ++jt) {
			std::cout << *jt << " ";
		}
		std::cout << std::endl;

		stack.Debug();
		if (top.m_value == *it) {
			++it;
			if (stack.ValidateTop()) {
				SuccessM("success");
				break;
			}
		}
		else {
			const short delta = stack.ChangeTop(false);
			if (delta == -1) {
				ErrorM("failure");
				break;
			}
			else if (delta > 0) {
				std::cout << "restore " << delta << " tokens" << std::endl;
				it -= delta;
			}
		}
	}

	if (it != end) {
		ErrorM("Failed to match entire expression.")
	}
}

const Rule::Lexeme& Analyzer::ExpandStack(Stack& stack)
{
	while (stack.TopLexeme().m_type == Rule::Lexeme::NON_TERMINAL) {
		const Rule::Lexeme& top = stack.TopLexeme();
		const Rule& rule = m_grammar.GetRule(top.m_value);
		stack.ExpandTop(rule);
	}

	return stack.TopLexeme();
}

