#pragma once

#include <unordered_map>

#include "common.h"

class Automaton
{
public:
	class AutomatonTransition
	{
	public:
		/// The initial state.
		unsigned short m_begin;
		/// The transition char.
		std::string m_tag;
		/// The final state.
		unsigned short m_end;
	};

	using AutomatonTransitionList = std::vector<AutomatonTransition>;

private:
	/// Associate each state to a list of transition.
	std::unordered_map<unsigned short, AutomatonTransitionList> m_table;

public:
	Automaton(const AutomatonTransitionList& transitions);

	bool Process(const std::string& content);
};
