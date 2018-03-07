#include <iomanip>

#include "syntax.h"
#include "grammar.h"
#include "stack.h"

SyntaxAnalyzer::SyntaxAnalyzer(const Grammar& grammar, const LexemeList& lexemes)
	:m_grammar(grammar),
	m_lexemes(lexemes)
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

static void printTokenIt(const LexemeList::const_iterator& it, const LexemeList& lexemes)
{
	for (const Lexeme& lexeme : lexemes) {
		std::cout << lexeme.GetToken();
	}
	std::cout << std::endl;
	for (LexemeList::const_iterator cit = lexemes.cbegin(); cit != it; ++cit) {
		for (unsigned short i = 0, size = cit->GetToken().size(); i < size; ++i) {
			std::cout << " ";
		}
	}
	std::cout << "^" << std::endl;
}

static bool matchRule(const Rule& rule, const Grammar& grammar, LexemeList::const_iterator& it, const LexemeList& lexemes)
{
	LexemeList::const_iterator previt = it;
	for (const Rule::ConditionList& proposal : rule.GetProposals(*it)) {
		bool fail = false;
		DebugRuleM(rule, "Testing proposal: " << proposal);
		for (const Rule::Condition& cond : proposal) {
			DebugRuleM(rule, "Testing condition: " << cond);

			if (it == lexemes.end()) {
				FailureRuleM(rule, "Reach end of line with staying condition.");
				fail = true;
				break;
			}

			switch (cond.m_type) {
				case Rule::Condition::TERMINAL:
				case Rule::Condition::TERMINAL_TYPE:
				{
					printTokenIt(it, lexemes);
					if (cond.Match(*it)) {
						++it;
					}
					else {
						fail = true;
					}
					break;
				}
				case Rule::Condition::NON_TERMINAL:
				{
					const Rule& childRule = grammar.GetRule(cond.m_value);
					if (!matchRule(childRule, grammar, it, lexemes)) {
						fail = true;
					}
					break;
				}
				default:
				{
					break;
				}
			}
			if (fail) {
				FailureRuleM(rule, "Failed with condition: " << cond.m_value);
				it = previt;
				break;
			}
		}
		if (!fail) {
			SuccessRuleM(rule, "Succeed with proposal: " << proposal);
			return true;
		}
	}

	return false;
}

void SyntaxAnalyzer::AnalyzeLLNaive()
{
	const Rule& root = m_grammar.GetRule("root");
	LexemeList::const_iterator it = m_lexemes.begin();
	matchRule(root, m_grammar, it, m_lexemes);

	if (it != m_lexemes.end()) {
		ErrorM("Failed to match entire expression.")
	}
}

void SyntaxAnalyzer::AnalyzeLLStack()
{
	const Rule& root = m_grammar.GetRule("root");
	LexemeList::const_iterator it = m_lexemes.begin();
	LexemeList::const_iterator end = m_lexemes.end();

	Stack stack;
	stack.ExpandTop(root, root.GetProposals(*it));

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
		const Rule::Condition& top = ExpandStack(stack, *it);
		for (LexemeList::const_iterator jt = it; jt != end; ++jt) {
			std::cout << jt->GetToken() << " ";
		}
		std::cout << std::endl;

		stack.Debug();
		if (top.m_type == Rule::Condition::EMPTY) {
			// Don't consume a char.
			if (stack.ValidateTop()) {
				SuccessM("success");
				break;
			}
		}
		else if (top.Match(*it)) {
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

const Rule::Condition& SyntaxAnalyzer::ExpandStack(Stack& stack, const Lexeme& prefix)
{
	/* Unneeded loop if the proposals are get from a prefix table as the prefix computation
	 * compute proposals with the lexeme key at its left.
	 * In this case it behaves only as a condition.
	 */
	while (stack.TopCondition().m_type == Rule::Condition::NON_TERMINAL) {
		const Rule::Condition& top = stack.TopCondition();
		const Rule& rule = m_grammar.GetRule(top.m_value);
		const Rule::ProposalSet& proposals = rule.GetProposals(prefix);
		stack.ExpandTop(rule, proposals);
	}

	return stack.TopCondition();
}

