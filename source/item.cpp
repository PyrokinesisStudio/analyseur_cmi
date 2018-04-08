#include "item.h"
#include "grammar.h"

const Item Item::empty(Rule::Condition(), Rule::Proposal::empty, 0);

/*static ItemSet::GlobalCache globalCache;

ItemSet::GlobalCache::~GlobalCache()
{
	for (auto& pair : m_itemToCache) {
		delete pair.second;
	}
}

ItemSet::Cache *ItemSet::GlobalCache::GetCache(const ItemSet& items)
{
	const std::map<ItemSet, Cache *>::const_iterator it = m_itemToCache.find(items);
	if (it != m_itemToCache.end()) {
		return it->second;
	}

	Cache *cache = new Cache();
	m_itemToCache.insert({items, cache});

	return cache;
}*/

Item::Item(const Rule::Condition& parentCond, const Rule::Proposal& proposal, unsigned short pos)
	:m_parentCondition(parentCond),
	m_proposal(proposal),
	m_pos(pos)
{
}

const Rule::Condition& Item::GetParentCondition() const
{
	return m_parentCondition;
}

const Rule::Proposal& Item::GetProposal() const
{
	return m_proposal;
}

unsigned short Item::GetPosition() const
{
	return m_pos;
}

ItemSet::ItemSet()
{
// 	m_cache = globalCache.GetCache(*this);
}

ItemSet::ItemSet(const std::multiset<Item>& items)
	:m_items(items)
{
// 	m_cache = globalCache.GetCache(*this);
}

bool ItemSet::Empty() const
{
	return m_items.empty();
}

void ItemSet::GetClosureRecursive(const Grammar& grammar, std::multiset<Item>& items) const
{
	for (const Item& item : m_items) {
		const Rule::Condition& cond = item.GetProposal().GetConditions()[item.GetPosition()];

		// Item is already in the list and so its closure too.
		if (items.find(item) != items.end()) {
			return;
		}
		items.insert(item);


		if (cond.m_type == Rule::Condition::NON_TERMINAL) {
			const Rule& rule = grammar.GetRule(cond.m_value);

			for (const Rule::Proposal& proposal : rule.GetProposals()) {
				const Item subItem(cond, proposal, 0);
				const ItemSet subSet({subItem});
				subSet.GetClosureRecursive(grammar, items);
			}
		}
	}
}

Item ItemSet::GetReduction(const Grammar& grammar) const
{
	const ItemSet items = GetClosure(grammar);

	for (const Item& item : items.m_items) {
		if (item.GetPosition() == item.GetProposal().GetConditions().size()) {
			return item;
		}
	}

	return Item::empty;
}

ItemSet ItemSet::GetTransition(const Grammar& grammar, const Rule::Condition& condTrans) const
{
	std::multiset<Item> items;
	const ItemSet closure = GetClosure(grammar);
	for (const Item& item : closure.m_items) {
		const Rule::Proposal& proposal = item.GetProposal();
		const Rule::ConditionList& conditions = proposal.GetConditions();

		const unsigned short pos = item.GetPosition();

		// Do nothing if the position is at the end of the proposal.
		if (pos >= conditions.size()) {
			continue;
		}

		const Rule::Condition& cond = conditions[pos];

		if (cond == condTrans) {
			// Insert item with the position after the tag.
			items.insert(Item(item.GetParentCondition(), proposal, pos + 1));
		}
	}

	return ItemSet(items).GetClosure(grammar);
}

ItemSet ItemSet::GetClosure(const Grammar& grammar) const
{
	std::multiset<Item> items;
	GetClosureRecursive(grammar, items);

	return ItemSet(items);
}
