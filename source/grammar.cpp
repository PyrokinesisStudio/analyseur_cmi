#include <algorithm>
#include <istream>

#include "grammar.h"
#include "tokenizer.h"

Grammar::Grammar(std::istream& stream)
{
    std::string line;
	while (!std::getline(stream, line).eof()) {
		const StringList lexeme = SplitBlank(line);
		// Ignore empty lines.
		if (lexeme.empty()) {
			continue;
		}

		const StringList::const_iterator it = std::find(lexeme.begin(), lexeme.end(), "::=");
		if (it == lexeme.end()) {
			ErrorM("Invalid BNF: " << line);
			continue;
		}

		const StringList leftLexeme(lexeme.begin(), it);
		const StringList rightLexeme(std::next(it), lexeme.end());

		const std::string& ruleName = leftLexeme.front();

		const Rule rule(ruleName, rightLexeme);
		std::cout << "Rule <" << rule.GetName() << ">, ";
		rule.Print();

		// Extract terminals from rules.
		const StringSet terms = rule.GetTerms();
		m_terms.insert(terms.begin(), terms.end());

		m_rules[rule.GetName()] = rule;
    }
}

const Rule& Grammar::GetRule(const std::string& name) const
{
	static const Rule error;
	std::map<std::string, Rule>::const_iterator it = m_rules.find(name);
	if (it != m_rules.cend()) {
		return it->second;
	}
	return error;
}

const StringSet& Grammar::GetTerms() const
{
    return m_terms;
}
