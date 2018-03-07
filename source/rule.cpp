#include "rule.h"
#include "grammar.h"
#include "lexeme.h"

const char Rule::Condition::names[Rule::Condition::NUM_LEXEME_TYPE] =  {
		't', // TERMINAL
		'g', // TERMINAL_TYPE
		'n', // NON_TERMINAL,
		'e' // EMPTY
};

Rule::Condition::Condition(const std::string& token)
{
	if (token.front() == '<' && token.back() == '>') {
		if (token[1] == 't' && token[2] == '_') {
			m_type = TERMINAL_TYPE;
			m_value = token.substr(3, token.size() - 4);
		}
		else {
			m_type = NON_TERMINAL;
			m_value = token.substr(1, token.size() - 2);
		}
	}
	else if (token.front() == '"' && token.back() == '"') {
		m_type = TERMINAL;
		m_value = token.substr(1, token.size() - 2);
	}
	else if (token == "E") {
		m_type = EMPTY;
		m_value = "E";
	}
}

bool Rule::Condition::Match(const Lexeme& lexeme) const
{
	switch (m_type) {
		case TERMINAL:
		{
			return (m_value == lexeme.GetToken());
		}
		case TERMINAL_TYPE:
		{
			return (m_value == lexeme.GetType());
		}
		default:
		{
			return false;
		}
	}
}

Rule::Rule(const std::string& name, const StringList& tokens)
{
	m_name = name.substr(1, name.size() - 2);
	ConditionList proposal;
	for (const std::string& token : tokens) {
		if (token == "|") {
			m_proposals.insert(proposal);
			proposal = ConditionList();
		}
		else {
			proposal.emplace_back(token);
		}
	}
	m_proposals.insert(proposal);
}

void Rule::ConstructConditionPrefix(const ProposalSet& proposals, const ConditionList suffix, const Grammar& grammar)
{
	for (const ConditionList& proposal : proposals) {
		const Condition& cond = proposal.front();
		switch (cond.m_type) {
			case Rule::Condition::TERMINAL:
			case Rule::Condition::TERMINAL_TYPE:
			case Rule::Condition::EMPTY:
			{
				// Constructing the complete proposal by appending the suffix proposal.
				ConditionList prop = proposal;
				prop.insert(prop.end(), suffix.begin(), suffix.end());

				m_prefixedProposals[cond.m_value].insert(prop);
				break;
			}
			case Rule::Condition::NON_TERMINAL:
			{
				// Get the rule corresponding to the first non-terminal.
				const Rule& rule = grammar.GetRule(cond.m_value);

				/* Construct the suffix proposal based on the current prposal without its
				 * first condition (the one which will be replaced in next call) and appending
				 * the previous suffix proposal.
				 */
				ConditionList suf(++proposal.begin(), proposal.end());
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

	std::cout << "rule " << m_name << std::endl;
	for (const auto& pair : m_prefixedProposals) {
		std::cout << "\t" << pair.first << std::endl;
		for (const ConditionList& proposal : pair.second) {
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
	for (const ConditionList& proposal : m_proposals) {
		std::cout << "\t";
		for (const Condition& cond : proposal) {
			std::cout << cond.m_value << "(" << Rule::Condition::names[cond.m_type] << ") ";
		}
		std::cout << std::endl;
	}
}
