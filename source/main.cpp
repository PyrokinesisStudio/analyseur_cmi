#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "syntax.h"
#include "lexical.h"
#include "grammar.h"

int main(int argc, char **argv)
{
    if (argc < 4) {
        ErrorM("No enough arguments provided");
        return -1;
    }

    std::ifstream fileSrc;
    fileSrc.open(argv[1]);
    std::ifstream fileBnf;
    fileBnf.open(argv[2]);
	std::ifstream fileRegex;
	fileRegex.open(argv[3]);

    if (!fileSrc.is_open()) {
        ErrorM("Unable to open source file");
        return -2;
    }
    if (!fileBnf.is_open()) {
        ErrorM("Unable to open BNF file");
        return -3;
    }
    if (!fileRegex.is_open()) {
        ErrorM("Unable to open regex file");
        return -4;
    }

    SyntaxAnalyzer::AnalyzeMode mode = SyntaxAnalyzer::LL_NAIVE;
	if (argc > 4) {
		const std::string modeStr = argv[4];
		static const std::map<std::string, SyntaxAnalyzer::AnalyzeMode> convertTable = {
			{"naive", SyntaxAnalyzer::LL_NAIVE},
			{"stack", SyntaxAnalyzer::LL_STACK}
		};

		const std::map<std::string, SyntaxAnalyzer::AnalyzeMode>::const_iterator it = convertTable.find(modeStr);
		if (it == convertTable.end()) {
			ErrorM("Invalid analyze mode");
			return -5;
		}

		mode = it->second;
	}

    Grammar grammar(fileBnf);
	LexicalAnalyzer lexicalAnalyzer(fileRegex);

    const std::string content((std::istreambuf_iterator<char>(fileSrc)), std::istreambuf_iterator<char>());
	const LexemeList lexemes = lexicalAnalyzer.Process(content);

	for (const Lexeme& lexeme : lexemes) {
		std::cout << lexeme.GetToken() << " (" << lexeme.GetType() << ")" << std::endl;
	}

// 	SyntaxAnalyzer analyzer(grammar, tokens);
// 	analyzer.Process(mode);

    fileSrc.close();
    fileBnf.close();
}
