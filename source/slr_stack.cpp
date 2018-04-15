#include "slr_stack.h"

SlrStack::SlrStack(const LexemeList::const_iterator& begin, const LexemeList::const_iterator& end,
		const ItemSet& initialState, const Rule::Condition& endCond)
	:m_initialState(initialState),
	m_endCondition(endCond)
{
	for (LexemeList::const_iterator it = begin; it != end; ++it) {
		const Lexeme& lexeme = *it;
		m_conditions.emplace_back(Rule::Condition::TERMINAL, it->GetType());
	}
}

void SlrStack::Analyze(const Grammar& grammar)
{
	ItemSet state = m_initialState;
	unsigned short pos = 0;

	while (m_conditions.size() > 1 && m_conditions[0] != m_endCondition) {
		std::cout << "=============================" << std::endl;
		std::cout << "Stack : ";
		for (const Rule::Condition& cond : m_conditions) {
			std::cout << cond.m_value << " ";
		}
		std::cout << "(" << pos << ")" << std::endl;

		const Rule::Condition& cond = m_conditions[pos];

		ItemSet items = state.GetTransition(grammar, cond);
		std::cout << "Current state : " << std::endl;
		std::cout << state << std::endl;

		// If there's no transition possible, try a reduction TODO SUIVANT check.
		if (items.Empty()) {
			// TODO test if the reduction is valid.
			const Item reduction = state.GetReduction(grammar);
			std::cout << "Reduction : " << reduction << std::endl;

			const unsigned short lenProposal = reduction.GetProposal().GetConditions().size();
			// Get the iterator from the stack top to the bottom of the proposal.
			const std::deque<Rule::Condition>::const_iterator top = m_conditions.begin() + pos;
			const std::deque<Rule::Condition>::const_iterator bottom = top - lenProposal;

			// Replace the proposal with the parent condition.
			m_conditions.erase(bottom, top);
			m_conditions.insert(m_conditions.begin() + pos - lenProposal, reduction.GetParentCondition());

			// Reset to first state.
			pos = 0;
			state = m_initialState;
		}
		else {
			std::cout << "Transition : " << cond << std::endl;
			std::cout << items << std::endl;
			state = items;
			++pos;
		}
	}
}
