#include <iterator>

#include "stack.h"

Stack::Item::Item(const Rule& rule)
	:m_rule(rule),
	m_validated(0)
{
	const Rule::ProposalList& proposals = rule.GetProposals();
	m_currentProposal = proposals.begin();
	m_endProposal = proposals.end();
}

const Rule& Stack::Item::GetRule() const
{
	return m_rule;
}

void Stack::Item::Unpack(std::deque<Rule::Condition>& stack)
{
	const Rule::ConditionList& proposal = *m_currentProposal;
	stack.insert(stack.end(), proposal.crbegin(), proposal.crend());
}

unsigned short Stack::Item::Pack(std::deque<Rule::Condition>& stack, unsigned short delta)
{
	const Rule::ConditionList& proposal = *m_currentProposal;
	stack.resize(stack.size() - proposal.size() + m_validated + delta);

	return m_validated;
}

bool Stack::Item::Validate()
{
	++m_validated;
	return (m_validated == m_currentProposal->size());
}

bool Stack::Item::Next()
{
	m_validated = 0;
	++m_currentProposal;

	if (m_currentProposal == m_endProposal) {
		return false;
	}

	return true;
}

const Rule::Condition& Stack::TopCondition() const
{
	return m_conditions.back();
}

void Stack::ExpandTop(const Rule& rule)
{
	Item item(rule);

	// Replace the top condition.
	if (!m_conditions.empty()) {
		m_conditions.pop_back();
	}
	item.Unpack(m_conditions);

	m_items.push_back(item);
}

short Stack::ChangeTop(bool recursive)
{
	// If there's no parent the analyze failed.
	if (m_items.empty()) {
		return -1;
	}

	Item& item = m_items.back();

	// Remove the conditions of the current rule.
	const unsigned short validated = item.Pack(m_conditions, recursive ? 1 : 0);

	if (!item.Next()) {
		// The current rule failed for all proposals, asking for an other proposal in the parent rule.
		m_items.pop_back();

		// Change of proposal in the parent rule.
		return ChangeTop(true);
	}

	// Unpack the conditions of the next proposal of the current rule.
	item.Unpack(m_conditions);

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
		// If all the condition of the top rule is validated we pass to the parent rule.
		m_items.pop_back();
		// The parent rule has one condition validated indirectly (the previous rule itself).
		return ValidateItem();
	}

	return false;
}

bool Stack::ValidateTop()
{
	m_conditions.pop_back();

	return ValidateItem();
}

void Stack::Debug()
{
	std::cout << termcolor::bold;

	for (std::deque<Rule::Condition>::const_reverse_iterator it = m_conditions.crbegin(), end = m_conditions.crend(); it != end; ++it) {
		std::cout << it->m_value << " ";
	}

	std::cout << std::endl;

	if (!m_items.empty()) {
		std::cout << m_items.back();
		for (std::deque<Item>::const_reverse_iterator it = ++m_items.crbegin(), end = m_items.crend(); it != end; ++it) {
			std::cout << "<-" << *it;
		}
	}

	std::cout << termcolor::reset << std::endl;
}

