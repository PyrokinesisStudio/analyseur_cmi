#include "tokenizer.h"

StringList SplitBlank(const std::string& content)
{
	static const std::string white = " \t\n\r";

	StringList list;

	// Last iterator of non-blank char.
	const std::string::const_iterator end = content.end();
	std::string::const_iterator lastIt = end;

	for (std::string::const_iterator it = content.begin(); it != end; ++it) {
		const char c = *it;
		if (white.find(c) != std::string::npos) {
			// Found a blank char contrary to the previous char.
			if (lastIt != end) {
				list.emplace_back(lastIt, it);
				lastIt = end;
			}
		}
		else {
			// The previous char was a blank.
			if (lastIt == end) {
				lastIt = it;
			}
		}
	}
	// In case the string doesn't end by a blank.
	if (lastIt != end) {
		list.emplace_back(lastIt, end);
	}

	return list;
}

StringList SplitBlankAndSeparator(const std::string& content, const std::string& sep)
{
    const StringList split = SplitBlank(content);
	StringList list;

	for (const std::string& str : split) {
		unsigned short lastId = 0;
		for (unsigned short i = 0, size = str.size(); i < size; ++i) {
			const char c = str[i];
			if (sep.find(c) != std::string::npos) {
				// Two consecutive separators ?
				if (lastId != i) {
					list.push_back(str.substr(lastId, i - lastId));
				}
				list.emplace_back(1, c);
				// + 1 to exclude the separator.
				lastId = i + 1;
			}
		}
		// Nothing changed.
		if (lastId == 0) {
			list.push_back(str);
		}
	}

	return list;
}
