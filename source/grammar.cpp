#include <algorithm>
#include <istream>

#include "grammar.h"
#include "tokenizer.h"

#include <assert.h>

Grammar::Grammar(std::istream& stream)
{
    std::string line;
	while (!std::getline(stream, line).eof()) {
		const StringList tokens = SplitBlank(line);
		// Ignore empty lines.
		if (tokens.empty()) {
			continue;
		}

		const StringList::const_iterator end = tokens.end();
		const StringList::const_iterator proposalIt = std::find(tokens.begin(), tokens.end(), "::=");
// 		const StringList::const_iterator semanticIt = std::find(tokens.begin(), tokens.end(), ";;");
		if (proposalIt == end) {
			ErrorM("Invalid BNF: " << line);
			continue;
		}

		const StringList leftTokens(tokens.begin(), proposalIt);
		const StringList proposalTokens(std::next(proposalIt), end);
// 		const StringList semanticTokens((semanticIt == end) ? end : std::next(semanticIt), end);

		const std::string& ruleName = leftTokens.front();

		const Rule rule(ruleName, proposalTokens);
		std::cout << "Rule <" << rule.GetName() << ">, ";
		rule.Print();

		m_rules[rule.GetName()] = rule;
	}

	// Generate prefix for all rules.
	for (auto& pair : m_rules) {
		Rule& rule = pair.second;
		rule.ConstructPrefix(*this);
	}
}

const Rule& Grammar::GetRule(const std::string& name) const
{
	std::map<std::string, Rule>::const_iterator it = m_rules.find(name);

	assert(it != m_rules.cend() && "In-existing rule for name");
	
	return it->second;
}
