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

const Rule::Proposal Rule::Proposal::empty;

Rule::Rule::Proposal::Proposal()
	:m_semanticAction(0)
{
}

Rule::Proposal::Proposal(const Rule::ConditionList& conditions, const std::string& semanticAction)
	:m_conditions(conditions)
{
	if (!semanticAction.empty()) {
		// Used to parse the name.
		const Condition& tmpCond(semanticAction);
		for (unsigned short i = 0, size = m_conditions.size(); i < size; ++i) {
			if (m_conditions[i] == tmpCond) {
				m_semanticAction = i;
				break;
			}
		}
	}
	else {
		m_semanticAction = 0;
	}
}

Rule::Proposal::Proposal(const Rule::ConditionList& conditions, unsigned short semanticAction)
	:m_conditions(conditions),
	m_semanticAction(semanticAction)
{
}

const Rule::ConditionList& Rule::Proposal::GetConditions() const
{
	return m_conditions;
}

short Rule::Proposal::GetSemanticAction() const
{
	return m_semanticAction;
}

Rule::Rule(const std::string& name, const StringList& proposalTokens, const StringList& semanticTokens)
{
	m_name = name.substr(1, name.size() - 2);

	StringList::const_iterator proposalIt = proposalTokens.begin();
	const StringList::const_iterator proposalEnd = proposalTokens.end();

	StringList::const_iterator semanticIt = semanticTokens.begin();
	const StringList::const_iterator semanticEnd = semanticTokens.end();

	while (proposalIt != proposalEnd) {
		ConditionList conditions;
		for (; proposalIt != proposalEnd && *proposalIt != "|"; ++proposalIt) {
			conditions.emplace_back(*proposalIt);
		}

		std::string semanticAction;
		for (; semanticIt != semanticEnd && *semanticIt != "|"; ++semanticIt) {
			semanticAction = *semanticIt;
		}

		m_proposals.emplace(conditions, semanticAction);

		// Skip the | token.
		if (proposalIt != proposalEnd && *proposalIt == "|") {
			++proposalIt;
		}
		// Skip the | token.
		if (semanticIt != semanticEnd && *semanticIt == "|") {
			++semanticIt;
		}
	}
}

void Rule::ConstructConditionPrefix(const ProposalSet& proposals, const ConditionList suffix,
		unsigned short cumulSemanticAction, const Grammar& grammar)
{
	for (const Proposal& proposal : proposals) {
		const ConditionList& conditions = proposal.GetConditions();
		const Condition& cond = conditions.front();
		const unsigned short semanticAction = proposal.GetSemanticAction();
		switch (cond.m_type) {
			case Rule::Condition::TERMINAL:
			case Rule::Condition::TERMINAL_TYPE:
			case Rule::Condition::EMPTY:
			{
				// Constructing the complete proposal by appending the suffix proposal.
				ConditionList conds = conditions;
				conds.insert(conds.end(), suffix.begin(), suffix.end());

				// TODO
				m_prefixedProposals[cond.m_value].emplace(conds, semanticAction + cumulSemanticAction);
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

				ConstructConditionPrefix(rule.GetProposals(), suf, semanticAction + cumulSemanticAction, grammar);
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
	ConstructConditionPrefix(m_proposals, {}, 0, grammar);

	std::cout << "rule " << m_name << std::endl;
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
		std::cout << "\t";
		const ConditionList& conditions = proposal.GetConditions();
		for (const Condition& cond : conditions) {
			std::cout << cond << " ";
		}

		const short semanticAction = proposal.GetSemanticAction();
		if (semanticAction != -1) {
			std::cout << " || " << conditions[semanticAction] << " (" << semanticAction << ")";
		}

		std::cout << std::endl;
	}
}
