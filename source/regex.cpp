#include "regex.h"

static Automaton numberAuto({{0, "$0", 1}, {1, "$0", 1}});
static Automaton nameAuto({{0, "$A", 1}, {1, "$A", 1}, {1, "$0", 1}});

Automaton::Automaton(const Automaton::AutomatonTransitionList& transitions)
{
	for (const AutomatonTransition& transition : transitions) {
		m_table[transition.m_begin].push_back(transition);
	}
}

bool Automaton::Process(const std::string& content)
{
	StringList list;

	short state = 0;

	for (const char& c : content) {
		const std::unordered_map<unsigned short, AutomatonTransitionList>::const_iterator it = m_table.find(state);
		for (const AutomatonTransition& transition : it->second) {
			/*if (transition.m_tag == c) {
				state = transition.m_end;
			}*/
		}
	}
}


