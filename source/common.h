#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "termcolor.hpp"

#define ErrorM(x) std::cout << termcolor::red << termcolor::bold << x << termcolor::reset << std::endl;
#define FailureM(x) std::cout << termcolor::yellow << x << termcolor::reset << std::endl;
#define SuccessM(x) std::cout << termcolor::green << x << termcolor::reset << std::endl;
#define DebugM(x) std::cout << termcolor::bold << x << termcolor::reset << std::endl;

#define RulePrefixM(rule) std::cout << "<" << termcolor::bold << (rule).GetName() << termcolor::reset << "> ";
#define DebugRuleM(rule, x) RulePrefixM(rule); std::cout << x << std::endl;
#define ErrorRuleM(rule, x) RulePrefixM(rule); ErrorM(x);
#define FailureRuleM(rule, x) RulePrefixM(rule); FailureM(x);
#define SuccessRuleM(rule, x) RulePrefixM(rule); SuccessM(x);

using StringList = std::vector<std::string>;
