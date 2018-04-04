#include <iterator>
#include <assert.h>

#include "stack.h"

Stack::Item::Item(const Rule& rule, const Rule::Condition& ruleCond, const Rule::ProposalSet& proposals)
	:m_rule(rule),
	m_nbProposals(proposals.size()),
	m_currentProposal(proposals.begin()),
	m_endProposal(proposals.end()),
	m_validated(0),
	m_ruleCond(ruleCond),
	m_node(m_ruleCond, *m_currentProposal, Lexeme::empty)
{
}

const Rule& Stack::Item::GetRule() const
{
	return m_rule;
}

const Rule::Proposal& Stack::Item::GetCurrentProposal() const
{
	return *m_currentProposal;
}

const Rule::Condition& Stack::Item::GetTopCondition() const
{
	const Rule::ConditionList& conditions = m_currentProposal->GetConditions();

	return conditions[m_validated];
}

unsigned short Stack::Item::GetValidated() const
{
	return m_validated;
}

bool Stack::Item::Validate()
{
	++m_validated;
	return (m_validated == m_currentProposal->GetConditions().size());
}

bool Stack::Item::Next()
{
	// Reset the number of consumed lexemes.
	m_validated = 0;
	// Pass to the next proposal.
	++m_currentProposal;

	// Return false if 
	if (m_currentProposal == m_endProposal) {
		return false;
	}

	// Recreate the derivation node with the current proposal.
	m_node = DerivationNode(m_ruleCond, *m_currentProposal, Lexeme::empty);

	return true;
}

const DerivationNode& Stack::Item::GetNode() const
{
	return m_node;
}

void Stack::Item::AddNode(const DerivationNode& node)
{
	m_node.AddChild(node);
}

Stack::Stack(DerivationNode& derivationRoot)
	:m_derivationRoot(derivationRoot)
{
}

const Rule::Condition& Stack::TopCondition() const
{
	return m_items.back().GetTopCondition();
}

void Stack::ExpandTop(const Rule& rule, const Rule::Condition& ruleCond, const Rule::ProposalSet& proposals)
{
	m_items.emplace_back(rule, ruleCond, proposals);
}

short Stack::ChangeTop()
{
	// If there's no parent the analyze failed.
	if (m_items.empty()) {
		return -1;
	}

	Item& item = m_items.back();
	const unsigned short validated = item.GetValidated();

	if (!item.Next()) {
		// The current rule failed for all proposals, asking for an other proposal in the parent rule.
		m_items.pop_back();

		// Change of proposal in the parent rule.
		return ChangeTop();
	}

	return validated;
}

bool Stack::ValidateItem()
{
	if (m_items.empty()) {
		return true;
	}

	Item& item = m_items.back();
	// Notify that a condition was validated.
	if (item.Validate()) {
		// Manage derivation nodes.
		if (!m_items.empty()) {
			Item& parentItem = *(m_items.end() - 2);
			// If it's the last item, copy to the derivation root.
			if (m_items.size() == 1) {
				m_derivationRoot.AddChild(item.GetNode());
			}
			// Add the node to the parent item.
			else {
				parentItem.AddNode(item.GetNode());
			}
		}

		// If all the condition of the top rule is validated we pass to the parent rule.
		m_items.pop_back();

		// The parent rule has one condition validated indirectly (the previous rule itself).
		return ValidateItem();
	}

	return false;
}

bool Stack::ValidateTop(const Lexeme& lexeme)
{
	Item& item = m_items.back();
	item.AddNode(DerivationNode(item.GetTopCondition(), Rule::Proposal::empty, lexeme));

	return ValidateItem();
}

void Stack::Debug()
{
	std::cout << termcolor::bold;

	for (std::vector<Item>::const_reverse_iterator begin = m_items.crbegin(), end = m_items.crend(), it = begin; it != end; ++it) {
		const Rule::Proposal& proposal = it->GetCurrentProposal();
		const Rule::ConditionList& conditions = proposal.GetConditions();
		const unsigned short validated = it->GetValidated();
		for (unsigned short i = (it == begin) ? validated : validated + 1, size = conditions.size(); i < size; ++i) {
			std::cout << conditions[i].m_value << " ";
		}
	}

	std::cout << std::endl;

	if (!m_items.empty()) {
		std::cout << m_items.back();
		for (std::vector<Item>::const_reverse_iterator it = ++m_items.crbegin(), end = m_items.crend(); it != end; ++it) {
			std::cout << "<-" << *it;
		}
	}

	std::cout << termcolor::reset << std::endl;

}

const Rule& Stack::GetTopRule() const
{
	const Item& item = m_items.back();
	return item.GetRule();
}

const Rule::Proposal& Stack::GetTopCurrentProposal() const
{
	const Item& item = m_items.back();
	return item.GetCurrentProposal();
}

