#pragma once

#include "rule.h"

class Grammar
{
private:
    std::map<std::string, Rule> m_rules;
    StringSet m_terms;

public:
    Grammar(std::istream& stream);

	const Rule& GetRule(const std::string& name) const;
    const StringSet& GetTerms() const;
};
