#ifndef COMPOSITEHIGHLIGHTER_H
#define COMPOSITEHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>

#include "bracketmatchercomponent.h"
#include "syntaxhighlightercomponent.h"

#include "ote/Theming/theme.h"

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

	void setTheme(Theme t);
	Theme getTheme() const { return m_theme; }

protected:
	void highlightBlock(const QString &text) override;

//private:
public:
	friend class HighlighterComponent;

	Theme m_theme;

	BracketMatcherComponent* m_bracketMatcher = new BracketMatcherComponent(this);
	SyntaxHighlighterComponent* m_syntaxHighlighter = new SyntaxHighlighterComponent(this);
};

} // namespace ote

#endif // COMPOSITEHIGHLIGHTER_H
