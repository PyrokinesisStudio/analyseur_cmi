#pragma once

#include "rule.h"
#include "abstract_tree.h"

class GraphvizUtility;

class DerivationNode
{
private:
    std::vector<DerivationNode> m_children;

    const Rule::Condition& m_cond;
	const Rule::Proposal& m_proposal;
	const Lexeme& m_lexeme;

	unsigned int m_id;

	void ExportRecursive(GraphvizUtility& util, unsigned int parentId) const;

public:

    DerivationNode(const Rule::Condition& cond, const Rule::Proposal& proposal, const Lexeme& lexeme);
    ~DerivationNode() = default;

    void AddChild(const DerivationNode& node);
	void Clear();

	void Print(short level) const;
	void Export(const std::string& filename) const;

	AbstractNode ConstructAbstractTree() const;
};
