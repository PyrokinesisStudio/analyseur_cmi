#include "slr_stack.h"

SlrStack::SlrStack(const LexemeList::const_iterator& begin, const LexemeList::const_iterator& end, const ItemSet& initialState)
	:m_initialState(initialState)
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

	while (true) {
		std::cout << "stack: ";
		for (const Rule::Condition& cond : m_conditions) {
			std::cout << cond << " ";
		}
		std::cout << "(" << pos << ")" << std::endl;

		const Rule::Condition& cond = m_conditions[pos];

		ItemSet items = state.GetTransition(grammar, cond);
		std::cout << "Current state " << std::endl << items << std::endl;

		// If there's no transition possible, try a reduction TODO SUIVANT check.
		if (items.Empty()) {
			// TODO test if the reduction is valid.
			const Item reduction = state.GetReduction(grammar);
			std::cout << "Reduction : " << reduction << std::endl;
			m_conditions.erase(m_conditions.begin(),
					m_conditions.begin() + reduction.GetProposal().GetConditions().size());
			m_conditions.push_front(reduction.GetParentCondition());

			// Reset to first state.
			pos = 0;
			state = m_initialState;
		}
		else {
			std::cout << "Transition : " << cond << std::endl;
			state = items;
			++pos;
		}
	}
}
