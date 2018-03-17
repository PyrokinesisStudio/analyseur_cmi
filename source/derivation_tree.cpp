#include "graphviz_utility.h"
#include "derivation_tree.h"

void DerivationNode::ExportRecursive(GraphvizUtility& util, unsigned int parentId) const
{

	util.AddNode(m_id, m_cond.m_value);
	if (parentId != 0) {
		std::string label;
		if (m_lexeme != Lexeme::empty) {
			label = m_lexeme.GetToken();
		}
		util.AddEdge(parentId, m_id, label, "");
	}

	for (const DerivationNode& child : m_children) {
		child.ExportRecursive(util, m_id);
	}
}

static unsigned int idCount = 1;

DerivationNode::DerivationNode(const Rule::Condition& cond, const Rule::Proposal& proposal, const Lexeme& lexeme)
    :m_cond(cond),
    m_proposal(proposal),
    m_lexeme(lexeme),
    m_id(idCount++)
{
}

void DerivationNode::AddChild(const DerivationNode& node)
{
	m_children.push_back(node);
}

void DerivationNode::Clear()
{
	m_children.clear();
}

void DerivationNode::Print(short level) const
{
	for (unsigned short i = 0; i < (level - 1); ++i) {
		std::cout << "|   ";
	}
	if (level > 0) {
		std::cout << "|___";
	}

	std::cout << m_cond << " " <<
		termcolor::dark << termcolor::grey << termcolor::bold << termcolor::on_white << m_lexeme.GetToken() <<
		termcolor::reset << std::endl;

	for (const DerivationNode& child : m_children) {
		child.Print(level + 1);
	}
}

void DerivationNode::Export(const std::string& filename) const
{
	GraphvizUtility util;

	ExportRecursive(util, 0);

	util.Export(filename);
}

AbstractNode DerivationNode::ConstructAbstractTree() const
{
	AbstractNode node;
	for (const DerivationNode& child : m_children) {
		node.AddNode(child.ConstructAbstractTree());
	}

	if (m_children.empty()) {
		node.SetValue(m_lexeme.GetToken());
	}
	else {
		const short semanticAction = m_proposal.GetSemanticAction();
		node.SetValue(node.GetChildren()[semanticAction].GetValue());
	}

	return node;
}



