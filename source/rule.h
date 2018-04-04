#pragma once

#include <map>
#include <set>
#include <ostream>

#include "lexeme.h"

class Grammar;

class Rule
{
public:
	/**- \brief Condition of a rule option for a lexeme.
	 */
	class Condition
	{
	public:
		enum Type {
			/// A specific terminal such as ponctuation.
			TERMINAL,
			// A group of terminals such as number, identifier… as defined in the lexical analyzer.
			TERMINAL_TYPE,
			// A link to a grammar rule.
			NON_TERMINAL,
			// An optional empty rule, sometimes used to break a recursion.
			EMPTY,
			NUM_LEXEME_TYPE
		} m_type;

		static const char names[NUM_LEXEME_TYPE];

		std::string m_value;

		Condition() = default;
		Condition(const std::string& token);

		bool Match(const Lexeme& lexeme) const;
	};

	using ConditionList = std::vector<Condition>;

	class Proposal
	{
	private:
		ConditionList m_conditions;

	public:
		static const Proposal empty;

		Proposal();
		Proposal(const ConditionList& conditions);

		const ConditionList& GetConditions() const;
	};

	using ProposalSet = std::multiset<Proposal>;

private:
    ProposalSet m_proposals;
	std::map<std::string, ProposalSet> m_prefixedProposals;
	std::string m_name;

	void ConstructConditionPrefix(const ProposalSet& proposals, const ConditionList suffix, const Grammar& grammar);

public:
	Rule() = default;
    Rule(const std::string& name, const StringList& proposalTokens);

	void ConstructPrefix(const Grammar& grammar);

	const std::string& GetName() const;
	const ProposalSet& GetProposals() const;
	const ProposalSet& GetProposals(const Lexeme& prefix) const;

	void Print() const;
};

inline bool operator==(const Rule::Condition& c1, const Rule::Condition& c2)
{
	return (c1.m_value == c2.m_value && c1.m_type == c2.m_type);
}

inline bool operator< (const Rule::Proposal& p1, const Rule::Proposal& p2)
{
	return (p1.GetConditions().size() > p2.GetConditions().size());
}

inline std::ostream& operator<< (std::ostream& out, const Rule::Condition& cond)
{
	out << cond.m_value << termcolor::italic << " (" << Rule::Condition::names[cond.m_type] << ")" << termcolor::italic_off;
	return out;
}

inline std::ostream& operator<< (std::ostream& out, const Rule::ConditionList& list)
{
	if (!list.empty()) {
		out << list.front();
		for (unsigned int i = 1, size = list.size(); i < size; ++i) {
			out << " " << list[i];
		}
	}
	return out;
}

inline std::ostream& operator<< (std::ostream& out, const Rule::Proposal& proposal)
{
	out << proposal.GetConditions();
	return out;
}

