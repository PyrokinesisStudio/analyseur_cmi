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
		/// Current proposal unpacked on stack.
		Rule::ProposalList::const_iterator m_currentProposal;
		Rule::ProposalList::const_iterator m_endProposal;
		/// Number of validated lexemes.
		unsigned short m_validated;

	public:
		Item(const Rule& rule);

		const Rule& GetRule() const;

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

	/// Validate a lexeme on the top rule, could validate parent rules indirectly.
	bool ValidateItem();

public:
	const Rule::Condition& TopCondition() const;
	/// Expand a rule first proposal on the lexeme stack.
	void ExpandTop(const Rule& rule);
	/** Pack and unpack the last expanded rule with one of its next proposal.
	 * Return the number of char to restore.
	 * If all the proposals of the current rule were tested, the rule is pop,
	 * its lexemes packed and the parent rule test an other proposal.
	 * \param reursive When a rule is pop and that the parent rule has
	 * no more proposal, the parent rule will be pop and will try to pack its
	 * lexeme but we didn't restored the last lexeme, the one which was expanded
	 * in the child rule. To avoid restore this lexeme we just notify that
	 * one less lexeme is to restore.
	 */
	short ChangeTop(bool recursive);
	/// Validate the top lexeme, return true if all the rule were validated.
	bool ValidateTop();

	void Debug();
};

inline std::ostream& operator<< (std::ostream& out, const Stack::Item& item)
{
	const unsigned short nbProposals = item.m_rule.GetProposals().size();
	const unsigned short invalidProposals = nbProposals - std::distance(item.m_currentProposal, item.m_endProposal) + 1;

	out << termcolor::bold << item.m_rule.GetName() << termcolor::reset;
	out << "(";
	out << termcolor::red << invalidProposals << termcolor::reset;
	out << "/";
	out << termcolor::yellow << nbProposals << termcolor::reset;
	out << " | ";
	out << termcolor::green << item.m_validated << termcolor::reset << ")";

	return out;
}
