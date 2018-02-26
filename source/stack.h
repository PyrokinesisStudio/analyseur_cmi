#pragma once

#include <deque>
#include <iterator>

#include "rule.h"

class Stack
{
public:
	class Item
	{
		friend std::ostream& operator<< (std::ostream& out, const Item& item);
	private:
		const Rule& m_rule;
		/// Current option unpacked on stack.
		std::vector<Rule::LexemeList>::const_iterator m_currentOption;
		std::vector<Rule::LexemeList>::const_iterator m_endOption;
		/// Number of validated lexemes.
		unsigned short m_validated;

	public:
		Item(const Rule& rule);

		const Rule& GetRule() const;

		/// Unpack lexemes of the current option on stack.
		void Unpack(std::deque<Rule::Lexeme>& stack);
		/** Remove lexemes of the current option on the stack less a delta.
		 * Return the number of lexeme which were already validated. */
		unsigned short Pack(std::deque<Rule::Lexeme>& stack, unsigned short delta);
		/// Change to the next option.
		bool Next();
		/** Notify that a top lexeme was validated.
		 * Return true if all the lexemes were validated. */
		bool Validate();
	};

private:
	std::deque<Rule::Lexeme> m_lexemes;
	std::deque<Item> m_items;

	/// Validate a lexeme on the top rule, could validate parent rules indirectly.
	bool ValidateItem();

public:
	const Rule::Lexeme& TopLexeme() const;
	/// Expand a rule first option on the lexeme stack.
	void ExpandTop(const Rule& rule);
	/** Pack and unpack the last expanded rule with one of its next option.
	 * Return the number of char to restore.
	 * If all the options of the current rule were tested, the rule is pop,
	 * its lexemes packed and the parent rule test an other option.
	 * \param reursive When a rule is pop and that the parent rule has
	 * no more option, the parent rule will be pop and will try to pack its
	 * lexeme but we didn't restored the last lexeme, the one which was expanded
	 * in the child rule. To avoid restore this lexeme we just notify that
	 * one less lexeme is to restore.
	 */
	short ChangeTop(bool recursive);
	/// Validate the top lexeme, return true if all the rule were validated.
	bool ValidateTop();

	void Debug();
};

inline std::ostream& operator<< (std::ostream& out, const Stack::Item& item)
{
	const unsigned short nbOptions = item.m_rule.GetOptions().size();
	const unsigned short usedOptions = nbOptions - std::distance(item.m_currentOption, item.m_endOption) + 1;

	out << termcolor::bold << item.m_rule.GetName() << termcolor::reset;
	out << "(";
	out << termcolor::red << usedOptions << termcolor::reset;
	out << "/";
	out << termcolor::yellow << nbOptions << termcolor::reset;
	out << " | ";
	out << termcolor::green << item.m_validated << termcolor::reset << ")";

	return out;
}
