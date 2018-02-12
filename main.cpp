#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "analyzer.h"
#include "grammar.h"
#include "tokenizer.h"

int main(int argc, char **argv)
{
    if (argc < 3) {
        ErrorM("No enough arguments provided");
        return -1;
    }

    std::ifstream fileSrc;
    fileSrc.open(argv[1]);
    std::ifstream fileBnf;
    fileBnf.open(argv[2]);

    if (!fileSrc.is_open()) {
        ErrorM("Unable to open source file");
        return -2;
    }
    if (!fileBnf.is_open()) {
        ErrorM("Unable to open BNF file");
        return -3;
    }

    Analyzer::AnalyzeMode mode = Analyzer::LL_NAIVE;
	if (argc > 3) {
		const std::string modeStr = argv[3];
		static const std::map<std::string, Analyzer::AnalyzeMode> convertTable = {
			{"naive", Analyzer::LL_NAIVE},
			{"stack", Analyzer::LL_STACK}
		};

		const std::map<std::string, Analyzer::AnalyzeMode>::const_iterator it = convertTable.find(modeStr);
		if (it == convertTable.end()) {
			ErrorM("Invalid analyze mode");
			return -4;
		}

		mode = it->second;
	}

    Grammar grammar(fileBnf);

    const std::string content((std::istreambuf_iterator<char>(fileSrc)), std::istreambuf_iterator<char>());
    const StringList tokens = Tokenize(content, grammar.GetTerms());

	DebugM("Tokens:");
    for (const std::string& str : tokens) {
        std::cout << str << " ";
    }
    std::cout << std::endl;

	Analyzer analyzer(grammar, tokens);
	analyzer.Analyze(mode);

    fileSrc.close();
    fileBnf.close();
}
