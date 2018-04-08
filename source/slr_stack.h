#pragma once

#include <deque>

#include "item.h"

class SlrStack
{
private:
	/// The initial state from the augmented item -<root>.
	const ItemSet& m_initialState;

	/* The stack of condition, each final state reduce terminal condition
	 * to non-terminal condition.
	 */
	std::deque<Rule::Condition> m_conditions;

public:
	SlrStack(const LexemeList::const_iterator& begin, const LexemeList::const_iterator& end, const ItemSet& initialState);

	void Analyze(const Grammar& grammar);
};
