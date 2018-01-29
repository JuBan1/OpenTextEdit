#ifndef BRACKETMATCHERCOMPONENT_H
#define BRACKETMATCHERCOMPONENT_H

#include <QVector>
#include <QTextBlockUserData>

#include "highlightercomponent.h"

namespace ote {

struct BracketInfo
{
	char character;
	int position;
};

class TextBlockData : public QTextBlockUserData
{
public:
	QVector<BracketInfo*> brackets;
};

class BracketMatcherComponent : public HighlighterComponent
{
public:
	BracketMatcherComponent(CompositeHighlighter* h);

	void setup() override;
	void highlightBlock(const QString& text) override;

	QPair<int, int> findMatchingBracketPosition(QTextCursor cursor) const;
private:
	int findRightBracket(char orig, char other, QTextBlock currentBlock, int pos, int depth) const;
	int findLeftBracket(char orig, char other, QTextBlock currentBlock, int pos, int depth) const;
};

} // namespace ote

#endif // BRACKETMATCHERCOMPONENT_H
