#pragma once

#include "common.h"

class GraphvizUtility;

class AbstractNode
{
private:
	std::string m_value;
	std::vector<AbstractNode> m_children;

	unsigned int m_id;

	void ExportRecursive(GraphvizUtility& util, unsigned int parentId) const;

public:
	AbstractNode();

	const std::vector<AbstractNode>& GetChildren() const;

	const std::string& GetValue() const;
	void SetValue(const std::string& value);

	void AddNode(const AbstractNode& node);
	void Prune();

	void Print(short level) const;
	void Export(const std::string& filename) const;
};
