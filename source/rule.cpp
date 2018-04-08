#include "rule.h"
#include "grammar.h"
#include "lexeme.h"

const char Rule::Condition::names[Rule::Condition::NUM_LEXEME_TYPE] =  {
		't', // TERMINAL
		'n', // NON_TERMINAL,
		'e' // EMPTY
};

Rule::Condition::Condition(Rule::Condition::Type type, const std::string& value)
	:m_type(type),
	m_value(value)
{
}

Rule::Condition::Condition(const std::string& token)
{
	if (token.front() == '<' && token.back() == '>') {
		m_type = NON_TERMINAL;
		m_value = token.substr(1, token.size() - 2);
	}
	else if (token == "E") {
		m_type = EMPTY;
		m_value = "E";
	}
	else {
		m_type = TERMINAL;
		m_value = token;
	}
}

bool Rule::Condition::Match(const Lexeme& lexeme) const
{
	switch (m_type) {
		case TERMINAL:
		{
			return (m_value == lexeme.GetType());
		}
		default:
		{
			return false;
		}
	}
}

const Rule::Proposal Rule::Proposal::empty;

Rule::Rule::Proposal::Proposal()
{
}

Rule::Proposal::Proposal(const Rule::ConditionList& conditions)
	:m_conditions(conditions)
{
}

const Rule::ConditionList& Rule::Proposal::GetConditions() const
{
	return m_conditions;
}

Rule::Rule(const std::string& name, const StringList& proposalTokens)
{
	m_name = name.substr(1, name.size() - 2);

	StringList::const_iterator proposalIt = proposalTokens.begin();
	const StringList::const_iterator proposalEnd = proposalTokens.end();

	/*StringList::const_iterator semanticIt = semanticTokens.begin();
	const StringList::const_iterator semanticEnd = semanticTokens.end();*/

	while (proposalIt != proposalEnd) {
		ConditionList conditions;
		for (; proposalIt != proposalEnd && *proposalIt != "|"; ++proposalIt) {
			conditions.emplace_back(*proposalIt);
		}

		/*std::string semanticAction;
		for (; semanticIt != semanticEnd && *semanticIt != "|"; ++semanticIt) {
			semanticAction = *semanticIt;
		}*/

		m_proposals.emplace(conditions);

		// Skip the | token.
		if (proposalIt != proposalEnd && *proposalIt == "|") {
			++proposalIt;
		}
		// Skip the | token.
		/*if (semanticIt != semanticEnd && *semanticIt == "|") {
			++semanticIt;
		}*/
	}
}

void Rule::ConstructConditionPrefix(const ProposalSet& proposals, const ConditionList suffix, const Grammar& grammar)
{
	for (const Proposal& proposal : proposals) {
		const ConditionList& conditions = proposal.GetConditions();
		const Condition& cond = conditions.front();
		switch (cond.m_type) {
			case Rule::Condition::TERMINAL:
			case Rule::Condition::EMPTY:
			{
				// Constructing the complete proposal by appending the suffix proposal.
				ConditionList conds = conditions;
				conds.insert(conds.end(), suffix.begin(), suffix.end());

				m_prefixedProposals[cond.m_value].emplace(conds);
				break;
			}
			case Rule::Condition::NON_TERMINAL:
			{
				// Get the rule corresponding to the first non-terminal.
				const Rule& rule = grammar.GetRule(cond.m_value);

				/* Construct the suffix proposal based on the current proposal without its
				 * first condition (the one which will be replaced in next call) and appending
				 * the previous suffix proposal.
				 */
				ConditionList suf(++conditions.begin(), conditions.end());
				suf.insert(suf.end(), suffix.begin(), suffix.end());

				ConstructConditionPrefix(rule.GetProposals(), suf, grammar);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void Rule::ConstructPrefix(const Grammar& grammar)
{
	ConstructConditionPrefix(m_proposals, {}, grammar);

	std::cout << "Prefix <" << m_name << ">" << std::endl;
	for (const auto& pair : m_prefixedProposals) {
		std::cout << "\t" << pair.first << std::endl;
		for (const Proposal& proposal : pair.second) {
			std::cout << "\t\t" << proposal << std::endl;
		}
	}
}

const std::string& Rule::GetName() const
{
	return m_name;
}

const Rule::ProposalSet& Rule::GetProposals() const
{
	return m_proposals;
}

const Rule::ProposalSet& Rule::GetProposals(const Lexeme& prefix) const
{
	const std::map<std::string, ProposalSet>::const_iterator tokenIt = m_prefixedProposals.find(prefix.GetToken());
	if (tokenIt != m_prefixedProposals.end()) {
		return tokenIt->second;
	}

	const std::map<std::string, ProposalSet>::const_iterator typeIt = m_prefixedProposals.find(prefix.GetType());
	if (typeIt != m_prefixedProposals.end()) {
		return typeIt->second;
	}

	const std::map<std::string, ProposalSet>::const_iterator emptyIt = m_prefixedProposals.find("E");
	if (emptyIt != m_prefixedProposals.end()) {
		return emptyIt->second;
	}

	const static ProposalSet empty;
	return empty;
}

void Rule::Print() const
{
	std::cout << "options: " << std::endl;
	for (const Proposal& proposal : m_proposals) {
		std::cout << "\t" << proposal << std::endl;
	}
}
