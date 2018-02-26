#include <istream>

#include "lexical.h"
#include "tokenizer.h"

static const std::array<std::string, Lexeme::NUM_LEXEME_TYPE> regexNameTable = {
	"<ponctuation>", // PONCTUATION
	"<integer>", // INTEGER
	"<float>", // FLOAT
	"<keyword>", // KEYWORD
	"<identifier>", // IDENTIFIER
	"<operator>", // OPERATOR
};

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
			bool sucess = false;
			for (unsigned short i = 0; i < Lexeme::NUM_LEXEME_TYPE; ++i) {
				if (regexNameTable[i] == regexName) {
					m_regexes[i] = std::regex(rightTokens.front());
					sucess = true;
					std::cout << "Regex " << regexName << ": " << rightTokens.front() << std::endl;;
					break;
				}
			}

			if (!sucess) {
				ErrorM("Invalid regex name: " << line)
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
		for (unsigned short i = 0; i < Lexeme::NUM_LEXEME_TYPE; ++i) {
			if (std::regex_search(str, m_regexes[i])) {
				list.emplace_back(str, (Lexeme::Type)i);
				valid = true;
				break;
			}
		}
		if (!valid) {
			ErrorM("Invalid lexeme: " << str);
		}
	}

	return list;
}
