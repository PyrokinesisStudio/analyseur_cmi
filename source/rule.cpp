#include "rule.h"
#include "grammar.h"

const char Rule::Lexeme::names[3] =  {
		't', // TERMINAL
		'n', // NON_TERMINAL,
		'e' // EMPTY
};

Rule::Lexeme::Lexeme(const std::string& token)
{
	if (token.front() == '<' && token.back() == '>') {
		m_type = NON_TERMINAL;
		m_value = token.substr(1, token.size() - 2);
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

Rule::Rule(const std::string& name, const StringList& tokens)
{
	m_name = name.substr(1, name.size() - 2);
	m_options.emplace_back();
	for (const std::string& token : tokens) {
		if (token == "|") {
			m_options.emplace_back();
		}
		else {
			m_options.back().emplace_back(token);
		}
	}
}

const std::string& Rule::GetName() const
{
	return m_name;
}

StringSet Rule::GetTerms() const
{
    StringSet terms;
	for (const LexemeList& lexemeList : m_options) {
		for (const Lexeme& lexeme : lexemeList) {
            if (lexeme.m_type == Rule::Lexeme::TERMINAL) {
                terms.insert(lexeme.m_value);
            }
        }
    }

    return terms;
}

const std::vector<Rule::LexemeList>& Rule::GetOptions() const
{
	return m_options;
}

void Rule::Print() const
{
	std::cout << "options: " << std::endl;
	for (const LexemeList& lexemeList : m_options) {
		std::cout << "\t";
		for (const Lexeme& lexeme : lexemeList) {
			std::cout << lexeme.m_value << " (" << Rule::Lexeme::names[lexeme.m_type] << ") ";
		}
		std::cout << std::endl;
	}
}
