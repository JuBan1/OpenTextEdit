#ifndef SYNTAXHIGHLIGHTERCOMPONENT_H
#define SYNTAXHIGHLIGHTERCOMPONENT_H

#include <QTextCharFormat>
#include <QRegularExpression>

#include "highlightercomponent.h"

#include "ote/Syntax/syntaxdefinition.h"

namespace ote {

class SyntaxHighlighterComponent : public HighlighterComponent
{
public:
	SyntaxHighlighterComponent(CompositeHighlighter* h);

	void setup() override;
	void highlightBlock(const QString& text) override;

private:
	QRegularExpression commentStartExpression;
	QRegularExpression commentEndExpression;
};

} // namespace ote

#endif // SYNTAXHIGHLIGHTERCOMPONENT_H
