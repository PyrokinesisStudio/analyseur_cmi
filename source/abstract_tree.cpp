#include "graphviz_utility.h"

#include "abstract_tree.h"

void AbstractNode::ExportRecursive(GraphvizUtility& util, unsigned int parentId) const
{
	util.AddNode(m_id, m_value);
	if (parentId != 0) {
		util.AddEdge(parentId, m_id, "", "");
	}

	for (const AbstractNode& child : m_children) {
		child.ExportRecursive(util, m_id);
	}
}

static unsigned int idCount = 1;

AbstractNode::AbstractNode()
	:m_id(idCount++)
{
}

const std::vector<AbstractNode>& AbstractNode::GetChildren() const
{
	return m_children;
}

const std::string& AbstractNode::GetValue() const
{
	return m_value;
}

void AbstractNode::SetValue(const std::string& value)
{
	m_value = value;
}

void AbstractNode::AddNode(const AbstractNode& node)
{
	m_children.push_back(node);
}

void AbstractNode::Prune()
{
	for (AbstractNode& child : m_children) {
		child.Prune();
	}

	unsigned short size = m_children.size();
	for (unsigned short i = 0; i < size;) {
		AbstractNode& child = m_children[i];
		if (child.GetValue() == m_value) {
			const std::vector<AbstractNode>& subchildren = child.GetChildren();
			m_children.insert(m_children.end(), subchildren.begin(), subchildren.end());
			m_children.erase(m_children.begin() + i);
			--size;
		}
		else {
			++i;
		}
	}
}

void AbstractNode::Print(short level) const
{
	for (unsigned short i = 0; i < (level - 1); ++i) {
		std::cout << "|   ";
	}
	if (level > 0) {
		std::cout << "|___";
	}

	std::cout << m_value << std::endl;

	for (const AbstractNode& child : m_children) {
		child.Print(level + 1);
	}
}

void AbstractNode::Export(const std::string& filename) const
{
	GraphvizUtility util;

	ExportRecursive(util, 0);

	util.Export(filename);
}


