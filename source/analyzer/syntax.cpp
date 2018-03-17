#include <iomanip>

#include "syntax.h"
#include "grammar.h"
#include "stack.h"

SyntaxAnalyzer::SyntaxAnalyzer(const Grammar& grammar, const LexemeList& lexemes)
	:m_grammar(grammar),
	m_lexemes(lexemes)
{
}

void SyntaxAnalyzer::Process(AnalyzeMode mode) const
{
	void (SyntaxAnalyzer::*functions[])
	(const Rule&, const Rule::Condition&, DerivationNode&, LexemeList::const_iterator&, const LexemeList::const_iterator&) const =
    {
        &SyntaxAnalyzer::AnalyzeLLNaive, // LL_NAIVE
        &SyntaxAnalyzer::AnalyzeLLStack // LL_STACK
    };

	const Rule& root = m_grammar.GetRule("root");
    const Rule::Condition rootCond("<root>");
	const Rule::Proposal rootProposal({rootCond}, 0);
    DerivationNode derivationRoot(rootCond, rootProposal, Lexeme::empty);

	LexemeList::const_iterator it = m_lexemes.begin();
	LexemeList::const_iterator end = m_lexemes.end();

    (this->*functions[mode])(root, rootCond, derivationRoot, it, end);

	if (it != end) {
		ErrorM("Failed to match entire expression.")
	}

	derivationRoot.Print(0);
	derivationRoot.Export("derivation_tree.dot");

	AbstractNode abstractTree = derivationRoot.ConstructAbstractTree();
// 	abstractTree.Prune();
	abstractTree.Print(0);
	abstractTree.Export("abstract_tree.dot");
}

static void printTokenIt(const LexemeList::const_iterator& it, const LexemeList::const_iterator& end)
{
	std::cout << termcolor::dark << termcolor::grey << termcolor::bold;
	for (LexemeList::const_iterator cit = it; cit != end; ++cit) {
		std::cout << termcolor::on_white << cit->GetToken() << termcolor::on_grey << " ";
	}
	std::cout << termcolor::reset << std::endl;
}

static bool matchRule(const Rule& rule, const Rule::Condition& ruleCond,
		const Grammar& grammar, DerivationNode& parentNode,
		LexemeList::const_iterator& it, const LexemeList::const_iterator& end)
{
	const LexemeList::const_iterator previt = it;
	for (const Rule::Proposal& proposal : rule.GetProposals(*it)) {
		/* Construct a derivation node with the current condition corresponding to this rule
		 * and the proposal currently tested.
		 */
		DerivationNode node(ruleCond, proposal, Lexeme::empty);
		bool fail = false;

		DebugRuleM(rule, "Testing proposal: " << proposal);
		for (const Rule::Condition& cond : proposal.GetConditions()) {
			DebugRuleM(rule, "Testing condition: " << cond);

			/*if (it == end) { // TODO test if there's no E proposal available.
				FailureRuleM(rule, "Reach end of line with staying condition.");
				fail = true;
				break;
			}*/

			if (it == end) {
				break;
			}

			switch (cond.m_type) {
				case Rule::Condition::TERMINAL:
				case Rule::Condition::TERMINAL_TYPE:
				{
					printTokenIt(it, end);

					const Lexeme& lexeme = *it;
					if (cond.Match(lexeme)) {
						const DerivationNode childNode(cond, Rule::Proposal::empty, lexeme);
						node.AddChild(childNode);
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
					if (!matchRule(childRule, cond, grammar, node, it, end)) {
						fail = true;
					}
					break;
				}
				case Rule::Condition::EMPTY:
				{
					const DerivationNode childNode(cond, Rule::Proposal::empty, Lexeme::empty);
					node.AddChild(childNode);
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
			parentNode.AddChild(node);
			return true;
		}
	}

	return false;
}

void SyntaxAnalyzer::AnalyzeLLNaive(const Rule& root, const Rule::Condition& rootCond, DerivationNode& derivationRoot,
            LexemeList::const_iterator& it, const LexemeList::const_iterator& end) const
{
	matchRule(root, rootCond, m_grammar, derivationRoot, it, end);
}

void SyntaxAnalyzer::AnalyzeLLStack(const Rule& root, const Rule::Condition& rootCond, DerivationNode& derivationRoot,
            LexemeList::const_iterator& it, const LexemeList::const_iterator& end) const
{
	Stack stack;
	stack.ExpandTop(root, root.GetProposals(*it));

	while (true) {
		const bool expanded = ExpandStack(stack, *it);
		const Rule& rule = stack.GetTopRule();
		const Rule::Condition& top = stack.TopCondition();

		if (expanded) {
			const Rule::Proposal& proposal = stack.GetTopCurrentProposal();
			DebugRuleM(rule, "Testing proposal: " << proposal);
		}

		stack.Debug();

		DebugRuleM(rule, "Testing condition: " << top);

		printTokenIt(it, end);

		bool match = false;
		if (top.m_type == Rule::Condition::EMPTY) {
			// Don't consume a lexeme.
			match = true;
		}
		else if (top.Match(*it)) {
			// Consume a lexeme.
			++it;
			match = true;
		}
		else {
			FailureM("Failed with condition : " << top);
			const short delta = stack.ChangeTop(false);
			// -1 mean that no alternative proposals are available.
			if (delta == -1) {
				ErrorM("Failure");
				break;
			}
			// If the proposal consumed lexemes before failed they are restored for the next proposal.
			else if (delta > 0) {
				FailureM("Restore " << delta << " lexemes");
				it -= delta;
			}
		}

		if (match && stack.ValidateTop()) {
			SuccessM("Succeed");
			break;
		}
	}
}

bool SyntaxAnalyzer::ExpandStack(Stack& stack, const Lexeme& prefix) const
{
	/* Unneeded loop if the proposals are get from a prefix table as the prefix computation
	 * compute proposals with always a terminal at its beginning.
	 * In this case it behaves only as a condition.
	 */
	bool expanded = false;
	while (stack.TopCondition().m_type == Rule::Condition::NON_TERMINAL) {
		const Rule::Condition& top = stack.TopCondition();
		const Rule& rule = m_grammar.GetRule(top.m_value);
		const Rule::ProposalSet& proposals = rule.GetProposals(prefix);
		stack.ExpandTop(rule, proposals);
		expanded = true;
	}

	return expanded;
}

