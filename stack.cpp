#include <iterator>

#include "stack.h"

Stack::Item::Item(const Rule& rule)
	:m_rule(rule),
	m_validated(0)
{
	const std::vector<Rule::LexemeList>& options = rule.GetOptions();
	m_currentOption = options.begin();
	m_endOption = options.end();
}

const Rule& Stack::Item::GetRule() const
{
	return m_rule;
}

void Stack::Item::Unpack(std::deque<Rule::Lexeme>& stack)
{
	const Rule::LexemeList& option = *m_currentOption;
	stack.insert(stack.end(), option.crbegin(), option.crend());
}

unsigned short Stack::Item::Pack(std::deque<Rule::Lexeme>& stack, unsigned short delta)
{
	const Rule::LexemeList& option = *m_currentOption;
	stack.resize(stack.size() - option.size() + m_validated + delta);

	return m_validated;
}

bool Stack::Item::Validate()
{
	++m_validated;
	return (m_validated == m_currentOption->size());
}

bool Stack::Item::Next()
{
	m_validated = 0;
	++m_currentOption;

	if (m_currentOption == m_endOption) {
		return false;
	}

	return true;
}

const Rule::Lexeme& Stack::TopLexeme() const
{
	return m_lexemes.back();
}

void Stack::ExpandTop(const Rule& rule)
{
	Item item(rule);

	// Replace the top lexeme.
	if (!m_lexemes.empty()) {
		m_lexemes.pop_back();
	}
	item.Unpack(m_lexemes);

	m_items.push_back(item);
}

short Stack::ChangeTop(bool recursive)
{
	// If there's no parent the analyze failed.
	if (m_items.empty()) {
		return -1;
	}

	Item& item = m_items.back();

	// Remove the lexemes of the current rule.
	const unsigned short validated = item.Pack(m_lexemes, recursive ? 1 : 0);

	if (!item.Next()) {
		// The current rule failed for all options, asking for an other option in the parent rule.
		m_items.pop_back();

		// Change of option in the parent rule.
		return ChangeTop(true);
	}

	// Unpack the lexemes of the next option of the current rule.
	item.Unpack(m_lexemes);

	return validated;
}

bool Stack::ValidateItem()
{
	if (m_items.empty()) {
		return true;
	}

	Item& item = m_items.back();
	// Notify that a lexeme was validated.
	if (item.Validate()) {
		// If all the lexeme of the top rule is validated we pass to the parent rule.
		m_items.pop_back();
		// The parent rule has one lexeme validated indirectly.
		return ValidateItem();
	}

	return false;
}

bool Stack::ValidateTop()
{
	m_lexemes.pop_back();

	return ValidateItem();
}

void Stack::Debug()
{
	std::cout << termcolor::bold;

	for (std::deque<Rule::Lexeme>::const_reverse_iterator it = m_lexemes.crbegin(), end = m_lexemes.crend(); it != end; ++it) {
		std::cout << it->m_value << " ";
	}

	std::cout << std::endl;

	if (!m_items.empty()) {
		std::cout << m_items.back();
		for (std::deque<Item>::const_reverse_iterator it = ++m_items.crbegin(), end = m_items.crend(); it != end; ++it) {
			std::cout << "<-" << *it;
		}
	}

	std::cout << termcolor::reset << std::endl;
}

