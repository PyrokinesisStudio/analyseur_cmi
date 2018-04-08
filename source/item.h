#pragma once

#include "rule.h"

class Item
{
private:
	/// The parent condition of the proposal.
	const Rule::Condition& m_parentCondition;
	/// The proposal of the production.
	const Rule::Proposal& m_proposal;
	/// The theoretical analyze advancement position.
	unsigned short m_pos;

public:
	static const Item empty;

	Item(const Rule::Condition& parentCond, const Rule::Proposal& proposal, unsigned short pos);

	const Rule::Condition& GetParentCondition() const;
	const Rule::Proposal& GetProposal() const;
	unsigned short GetPosition() const;
};

class ItemSet
{
friend std::ostream& operator<< (std::ostream& out, const ItemSet& items);

public:
	/*struct Cache
	{
		ItemSet m_closure;
		std::map<Rule::Condition, ItemSet> m_transitions;
	};

	class GlobalCache
	{
	private:
		std::map<ItemSet, Cache *> m_itemToCache;

	public:
		GlobalCache() = default;
		~GlobalCache();

		Cache *GetCache(const ItemSet& items);
	};*/

private:
	std::multiset<Item> m_items;
	/// Cache for transition and closure sets.
// 	Cache *m_cache;

	void GetClosureRecursive(const Grammar& grammar, std::multiset<Item>& items) const;

public:
	ItemSet();
	ItemSet(const std::multiset<Item>& items);

	bool Empty() const;

	ItemSet GetClosure(const Grammar& grammar) const;
	Item GetReduction(const Grammar& grammar) const;
	ItemSet GetTransition(const Grammar& grammar, const Rule::Condition& condTrans) const;
};

inline bool operator< (const Item& i1, const Item& i2)
{
	if (i1.GetPosition() != i2.GetPosition()) {
		return (i1.GetPosition() < i2.GetPosition());
	}

	const Rule::ConditionList& conditions1 = i1.GetProposal().GetConditions();
	const Rule::ConditionList& conditions2 = i2.GetProposal().GetConditions();

	if (conditions1.size() != conditions2.size()) {
		return (conditions1.size() < conditions2.size());
	}

	for (unsigned short i = 0, size = conditions1.size(); i < size; ++i) {
		if (conditions1[i] != conditions2[i]) {
			return conditions1[i] < conditions2[i];
		}
	}

	return false;
}

inline std::ostream& operator<< (std::ostream& out, const Item& item)
{
	const Rule::ConditionList& conditions = item.GetProposal().GetConditions();
	const unsigned pos = item.GetPosition();
	for (unsigned short i = 0, size = conditions.size(); i < size; ++i) {
		if (i == pos) {
			out << "-";
		}
		else if (i < (size) && i > 0) {
			out << " ";
		}

		out << conditions[i];
	}

	return out;
}

inline std::ostream& operator<< (std::ostream& out, const ItemSet& items)
{
	for (const Item& item : items.m_items) {
		std::cout << item << std::endl;
	}

	return out;
}

inline bool operator== (const Item& i1, const Item& i2)
{
	return (i1.GetPosition() == i2.GetPosition() && i1.GetProposal() == i2.GetProposal());
}
