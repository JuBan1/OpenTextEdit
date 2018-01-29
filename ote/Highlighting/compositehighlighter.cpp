#include "compositehighlighter.h"

namespace ote {


CompositeHighlighter::CompositeHighlighter(QTextDocument* parent)
	: QSyntaxHighlighter (parent)
{

}

void CompositeHighlighter::setCurrentBlockUserData(QTextBlockUserData* data)
{
	QSyntaxHighlighter::setCurrentBlockUserData(data);
}

void*CompositeHighlighter::currentBlockUserData() const
{
	return QSyntaxHighlighter::currentBlockUserData();
}

void CompositeHighlighter::setCurrentBlockState(int state)
{
	QSyntaxHighlighter::setCurrentBlockState(state);
}

int CompositeHighlighter::previousBlockState() const
{
	return QSyntaxHighlighter::previousBlockState();
}

void CompositeHighlighter::setFormat(int start, int count, const QTextCharFormat& format)
{
	QSyntaxHighlighter::setFormat(start, count, format);
}

void CompositeHighlighter::setFormat(int start, int count, const QColor& color)
{
	QSyntaxHighlighter::setFormat(start, count, color);
}

void CompositeHighlighter::setFormat(int start, int count, const QFont& font)
{
	QSyntaxHighlighter::setFormat(start, count, font);
}

void CompositeHighlighter::setTheme(Theme t)
{
	m_theme = t;

	m_syntaxHighlighter->setup();
	m_bracketMatcher->setup();
}

void CompositeHighlighter::highlightBlock(const QString& text)
{
	m_syntaxHighlighter->highlightBlock(text);
	m_bracketMatcher->highlightBlock(text);
}


} // namespace ote
