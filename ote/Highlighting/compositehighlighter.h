#ifndef COMPOSITEHIGHLIGHTER_H
#define COMPOSITEHIGHLIGHTER_H

#include <memory>

#include <QRegularExpression>
#include <QSyntaxHighlighter>

#include "bracketmatchercomponent.h"
#include "syntaxhighlightercomponent.h"

#include "ote/Themes/theme.h"
#include "ote/Syntax/syntaxdefinition.h"

#include <memory>

class QString;
class QTextDocument;

namespace ote {

class CompositeHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	CompositeHighlighter(QTextDocument *parent = nullptr);

	void setCurrentBlockUserData(QTextBlockUserData* data);
	void* currentBlockUserData() const;

	void setCurrentBlockState(int state);
	int previousBlockState() const;

	void setFormat(int start, int count, const QTextCharFormat &format);
	void setFormat(int start, int count, const QColor &color);
	void setFormat(int start, int count, const QFont &font);

	void setSyntaxDefinition(SyntaxDefinition d);
	SyntaxDefinition getSyntaxDefinition() const { return m_definition; }

	void setTheme(Theme t);
	Theme getTheme() const { return m_theme; }

protected:
	void highlightBlock(const QString &text) override;

//private:
public:
	friend class HighlighterComponent;

	Theme m_theme;
	SyntaxDefinition m_definition;

	std::unique_ptr<BracketMatcherComponent> m_bracketMatcher;
	std::unique_ptr<SyntaxHighlighterComponent> m_syntaxHighlighter;
};

} // namespace ote

#endif // COMPOSITEHIGHLIGHTER_H
