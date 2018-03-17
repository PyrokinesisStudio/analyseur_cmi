#pragma once

#include <deque>
#include <iterator>

#include "rule.h"

class Stack
{
public:
	class Item
	{
		friend std::ostream& operator<< (std::ostream& out, const Item& item);
	private:
		const Rule& m_rule;
		unsigned short m_nbProposals;
		/// Current proposal unpacked on stack.
		Rule::ProposalSet::const_iterator m_currentProposal;
		Rule::ProposalSet::const_iterator m_endProposal;
		/// Number of validated lexemes.
		unsigned short m_validated;

	public:
		Item(const Rule& rule, const Rule::ProposalSet& proposals);

		const Rule& GetRule() const;
		const Rule::Proposal& GetCurrentProposal() const;

		/// Unpack lexemes of the current proposal on stack.
		void Unpack(std::deque<Rule::Condition>& stack);
		/** Remove lexemes of the current proposal on the stack less a delta.
		 * Return the number of lexeme which were already validated. */
		unsigned short Pack(std::deque<Rule::Condition>& stack, unsigned short delta);
		/// Change to the next proposal.
		bool Next();
		/** Notify that a top lexeme was validated.
		 * Return true if all the lexemes were validated. */
		bool Validate();
	};

private:
	std::deque<Rule::Condition> m_conditions;
	std::deque<Item> m_items;

	/// Validate a lexeme on the top item, could validate parent items indirectly.
	bool ValidateItem();

public:
	const Rule::Condition& TopCondition() const;
	/// Expand a item first proposal on the lexeme stack.
	void ExpandTop(const Rule& rule, const Rule::ProposalSet& proposals);
	/** Pack and unpack the last expanded item with one of its next proposal.
	 * Return the number of char to restore.
	 * If all the proposals of the current item were tested, the item is pop,
	 * its lexemes packed and the parent item test an other proposal.
	 * \param recursive When a item is pop and that the parent item has
	 * no more proposal, the parent item will be pop and will try to pack its
	 * lexeme but we didn't restored the last lexeme, the one which was expanded
	 * in the child item. To avoid restore this lexeme we just notify that
	 * one less lexeme is to restore.
	 */
	short ChangeTop(bool recursive);
	/// Validate the top lexeme, return true if all the items were validated.
	bool ValidateTop();

	void Debug();
	/// Used only for debugging purpose.
	const Rule& GetTopRule() const;
	/// Used only for debugging purpose.
	const Rule::Proposal& GetTopCurrentProposal() const;
};

inline std::ostream& operator<< (std::ostream& out, const Stack::Item& item)
{
	out << termcolor::bold << item.m_rule.GetName() << termcolor::reset;
	out << "(";

	const unsigned short nbProposals = item.m_nbProposals;
	if (nbProposals > 1) {
		const unsigned short invalidProposals = nbProposals - std::distance(item.m_currentProposal, item.m_endProposal) + 1;
		out << termcolor::red << invalidProposals << termcolor::reset;
		out << "/";
		out << termcolor::yellow << nbProposals << termcolor::reset;
		out << " | ";
	}

	out << termcolor::green << item.m_validated << termcolor::reset << ")";

	return out;
}
