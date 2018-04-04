#pragma once

#include <deque>
#include <iterator>

#include "rule.h"
#include "derivation_tree.h"

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

		/** Number of validated conditions, used to know which condition to test next
		 * and the number of lexemes to restore on failure.
		 */
		unsigned short m_validated;

		/// The non-terminal condition which produce this rule item.
		const Rule::Condition& m_ruleCond;

		/* This node is added to the parent only when this item is validated,
		 * if the item is invalid the node is fully reconstructed.
		 * NOTE: this node is never a leaf, the leaf are created and passed to AddNode(…).
		 */
		DerivationNode m_node;

	public:
		Item(const Rule& rule, const Rule::Condition& ruleCond, const Rule::ProposalSet& proposals);

		const Rule& GetRule() const;
		const Rule::Proposal& GetCurrentProposal() const;

		// Return the top condition not yet validated.
		const Rule::Condition& GetTopCondition() const;

		unsigned short GetValidated() const;

		/// Change to the next proposal.
		bool Next();

		/** Notify that a top lexeme was validated.
		 * Return true if all the lexemes were validated. */
		bool Validate();

		const DerivationNode& GetNode() const;
		void AddNode(const DerivationNode& node);
	};

private:
	std::vector<Item> m_items;
	DerivationNode& m_derivationRoot;

	/// Validate a lexeme on the top item, could validate parent items indirectly.
	bool ValidateItem();

public:
	Stack(DerivationNode& derivationRoot);

	const Rule::Condition& TopCondition() const;

	/// Expand a item first proposal on the lexeme stack.
	void ExpandTop(const Rule& rule, const Rule::Condition& ruleCond, const Rule::ProposalSet& proposals);

	/** Change the proposal of the top item and return the number of lexemes validated for restoration,
	 * -1 if there's no more proposal available.
	 */
	short ChangeTop();

	/// Validate the top lexeme, return true if all the items were validated.
	bool ValidateTop(const Lexeme& lexeme);

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

	out << termcolor::green << item.m_validated << "|" << item.m_currentProposal->GetConditions().size() << termcolor::reset << ")";

	return out;
}
