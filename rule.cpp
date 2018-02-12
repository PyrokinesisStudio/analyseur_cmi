#include "rule.h"
#include "grammar.h"

const char Rule::Lexeme::names[3] =  {
		't', // TERMINAL
		'n', // NON_TERMINAL,
		'e' // EMPTY
};

Rule::Lexeme::Lexeme(const std::string& str)
{
	if (str.front() == '<' && str.back() == '>') {
		m_type = NON_TERMINAL;
		m_value = str.substr(1, str.size() - 2);
	}
	else if (str.front() == '"' && str.back() == '"') {
		m_type = TERMINAL;
		m_value = str.substr(1, str.size() - 2);
	}
	else if (str == "E") {
		m_type = EMPTY;
		m_value = "E";
	}
}

Rule::Rule(const std::string& name, const StringList& content)
{
	m_name = name.substr(1, name.size() - 2);
	m_options.emplace_back();
	for (const std::string& str : content) {
		if (str == "|") {
			m_options.emplace_back();
		}
		else {
			m_options.back().emplace_back(str);
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

static void printTokenIt(StringList::const_iterator& it, const StringList& tokens)
{
	for (const std::string& str : tokens) {
		std::cout << str;
	}
	std::cout << std::endl;
	const unsigned short pos = std::distance(tokens.begin(), it);
	for (unsigned short i = 0; i < pos; ++i) {
		std::cout << " ";
	}
	std::cout << "^" << std::endl;
}

bool Rule::Match(const Grammar& grammar, StringList::const_iterator& it, const StringList& tokens) const
{
	StringList::const_iterator previt = it;
	for (const LexemeList& option : m_options) {
		bool fail = false;
		DebugRuleM(*this, "Testing option: " << option);
		for (const Lexeme& lexeme : option) {
			DebugRuleM(*this, "Testing lexeme: " << lexeme);
			switch (lexeme.m_type) {
				case Rule::Lexeme::TERMINAL:
				{
					printTokenIt(it, tokens);
					if (lexeme.m_value == *it) {
						++it;
					}
					else {
						fail = true;
					}
					break;
				}
				case Rule::Lexeme::NON_TERMINAL:
				{
					const Rule& rule = grammar.GetRule(lexeme.m_value);
					if (!rule.Match(grammar, it, tokens)) {
						fail = true;
					}
					break;
				}
				case Rule::Lexeme::EMPTY:
				{
					break;
				}
			}
			if (fail) {
				FailureRuleM(*this, "Failed with option: " << option);
				it = previt;
				break;
			}
			// TODO
			if (it == tokens.end()) {
				break;
			}
		}
		if (!fail) {
			SuccessRuleM(*this, "Succeed with option: " << option);
			return true;
		}
	}

	return false;
}
