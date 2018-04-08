#include <istream>

#include "lexical.h"
#include "tokenizer.h"

LexicalAnalyzer::LexicalAnalyzer(std::istream& stream)
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
			ErrorM("Invalid regex: " << line);
			continue;
		}

		const StringList leftTokens(tokens.begin(), it);
		const StringList rightTokens(std::next(it), tokens.end());

		const std::string& regexName = leftTokens.front();

		if (regexName == "<separator>") {
			m_separators = rightTokens.front();
			std::cout << "Separators: " << m_separators << std::endl;
		}
		else {
			if (m_regexes.find(regexName) == m_regexes.end()) {
				m_regexes[regexName] = "^" + rightTokens.front() + "$";
				std::cout << "Regex " << regexName << ": " << rightTokens.front() << std::endl;;
			}
		}
    }
}

LexemeList LexicalAnalyzer::Process(const std::string& content)
{
	const StringList tokens = SplitBlankAndSeparator(content, m_separators);

	LexemeList list;
	for (const std::string& str : tokens) {
		bool valid = false;
		for (const auto& pair : m_regexes) {
			if (std::regex_search(str, pair.second)) {
				const std::string& type = pair.first;
				list.emplace_back(str, type.substr(1, type.size() - 2));
				valid = true;
				break;
			}
		}
		if (!valid) {
			ErrorM("Invalid lexeme: " << str);
		}
	}

	// Always notice the end of the input by the empty lexeme.
	list.push_back(Lexeme::empty);

	return list;
}
