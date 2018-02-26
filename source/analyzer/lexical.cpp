#include "lexical.h"
#include "tokenizer.h"

LexicalAnalyzer::LexicalAnalyzer(std::istream& stream)
{
	
}

LexicalAnalyzer::TokenList LexicalAnalyzer::Process(const std::string& content)
{
	const StringList lexemes = SplitBlankAndSeparator(content, m_separators);

	TokenList list;
	for (const std::string& str : lexemes) {
		bool valid = false;
		for (unsigned short i = 0; i < NUM_TOKEN; ++i) {
			if (std::regex_search(str, m_regexes[i])) {
				list.push_back({str, (TokenType)i});
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
