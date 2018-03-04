#include <algorithm>
#include <istream>

#include "grammar.h"
#include "tokenizer.h"

Grammar::Grammar(std::istream& stream)
{
    std::string line;
	while (!std::getline(stream, line).eof()) {
		const StringList tokens = SplitBlank(line);
		// Ignore empty lines.
		if (tokens.empty()) {
			continue;
		}

		const StringList::const_iterator it = std::find(tokens.begin(), tokens.end(), "::=");
		if (it == tokens.end()) {
			ErrorM("Invalid BNF: " << line);
			continue;
		}

		const StringList leftTokens(tokens.begin(), it);
		const StringList rightTokens(std::next(it), tokens.end());

		const std::string& ruleName = leftTokens.front();

		const Rule rule(ruleName, rightTokens);
		std::cout << "Rule <" << rule.GetName() << ">, ";
		rule.Print();

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
