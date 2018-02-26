#include "syntax.h"
#include "grammar.h"

#include "stack.h"

SyntaxAnalyzer::SyntaxAnalyzer(const Grammar& grammar, const StringList& tokens)
	:m_grammar(grammar),
	m_tokens(tokens)
{
}

void SyntaxAnalyzer::Process(AnalyzeMode mode)
{
    void (SyntaxAnalyzer::*functions[])() = {
        &SyntaxAnalyzer::AnalyzeLLNaive, // LL_NAIVE
        &SyntaxAnalyzer::AnalyzeLLStack // LL_STACK
    };

    (this->*functions[mode])();
}

static void printTokenIt(StringList::const_iterator& it, const StringList& tokens)
{
	for (const std::string& str : tokens) {
		std::cout << str;
	}
	std::cout << std::endl;
	const unsigned short pos = std::distance(tokens.begin(), it);
	for (unsigned short i = 0; i < pos; ++i) {
		std::cout << " ";
	}
	std::cout << "^" << std::endl;
}

static bool matchRule(const Rule& rule, const Grammar& grammar, StringList::const_iterator& it, const StringList& tokens)
{
	StringList::const_iterator previt = it;
	for (const Rule::LexemeList& option : rule.GetOptions()) {
		bool fail = false;
		DebugRuleM(rule, "Testing option: " << option);
		for (const Rule::Lexeme& lexeme : option) {
			DebugRuleM(rule, "Testing lexeme: " << lexeme);
			switch (lexeme.m_type) {
				case Rule::Lexeme::TERMINAL:
				{
					printTokenIt(it, tokens);
					if (lexeme.m_value == *it) {
						++it;
					}
					else {
						fail = true;
					}
					break;
				}
				case Rule::Lexeme::NON_TERMINAL:
				{
					const Rule& childRule = grammar.GetRule(lexeme.m_value);
					if (!matchRule(childRule, grammar, it, tokens)) {
						fail = true;
					}
					break;
				}
				case Rule::Lexeme::EMPTY:
				{
					break;
				}
			}
			if (fail) {
				FailureRuleM(rule, "Failed with option: " << option);
				it = previt;
				break;
			}
			// TODO
			if (it == tokens.end()) {
				break;
			}
		}
		if (!fail) {
			SuccessRuleM(rule, "Succeed with option: " << option);
			return true;
		}
	}

	return false;
}

void SyntaxAnalyzer::AnalyzeLLNaive()
{
	const Rule& root = m_grammar.GetRule("root");
	StringList::const_iterator it = m_tokens.begin();
	matchRule(root, m_grammar, it, m_tokens);

	if (it != m_tokens.end()) {
		ErrorM("Failed to match entire expression.")
	}
}

void SyntaxAnalyzer::AnalyzeLLStack()
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
		if (top.m_value == "E") {
			// Doesn't consume a char.
			if (stack.ValidateTop()) {
				SuccessM("success");
				break;
			}
		}
		else if (top.m_value == *it) {
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

const Rule::Lexeme& SyntaxAnalyzer::ExpandStack(Stack& stack)
{
	while (stack.TopLexeme().m_type == Rule::Lexeme::NON_TERMINAL) {
		const Rule::Lexeme& top = stack.TopLexeme();
		const Rule& rule = m_grammar.GetRule(top.m_value);
		stack.ExpandTop(rule);
	}

	return stack.TopLexeme();
}

