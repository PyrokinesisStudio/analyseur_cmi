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
	:m_terminalType(Lexeme::NONE)
{
	if (token.front() == '<' && token.back() == '>') {
		if (token[1] == 't' && token[2] == '_') {
			m_type = TERMINAL_TYPE;
			m_value = token.substr(3, token.size() - 4);

			// Searching terminal type.
			bool found = false;
			for (unsigned short i = 0, size = Lexeme::typeNameTable.size(); i < size; ++i) {
				if (Lexeme::typeNameTable[i] == m_value) {
					m_terminalType = (Lexeme::Type)i;
					found = true;
					break;
				}
			}

			if (!found) {
				ErrorM("Terminal type name \"" << m_value << "\" doesn't exist.");
			}
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
			return (m_terminalType == lexeme.GetType());
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
	m_proposals.emplace_back();
	for (const std::string& token : tokens) {
		if (token == "|") {
			m_proposals.emplace_back();
		}
		else {
			m_proposals.back().emplace_back(token);
		}
	}
}

const std::string& Rule::GetName() const
{
	return m_name;
}

const Rule::ProposalList& Rule::GetProposals() const
{
	return m_proposals;
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
