#ifndef SYNTAXHIGHLIGHTERCOMPONENT_H
#define SYNTAXHIGHLIGHTERCOMPONENT_H

#include <QTextCharFormat>
#include <QRegularExpression>

#include "highlightercomponent.h"

namespace ote {

class SyntaxHighlighterComponent : public HighlighterComponent
{
public:
	SyntaxHighlighterComponent(CompositeHighlighter* h);

	void setup() override;
	void highlightBlock(const QString& text) override;

private:
	struct HighlightingRule
	{
		QRegularExpression pattern;
		QTextCharFormat format;
		int captureGroup = 0;
	};
	QVector<HighlightingRule> highlightingRules;

	struct WordHighlightingRule {
		QStringList words;
		QTextCharFormat format;
	};
	QVector<WordHighlightingRule> wordRules;

	QRegularExpression commentStartExpression;
	QRegularExpression commentEndExpression;
	QTextCharFormat multiLineCommentFormat;
};

} // namespace ote

#endif // SYNTAXHIGHLIGHTERCOMPONENT_H
