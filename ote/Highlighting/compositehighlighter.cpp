#include "compositehighlighter.h"

#include <QDebug>

namespace ote {

CompositeHighlighter::CompositeHighlighter(QTextDocument* parent)
	: QSyntaxHighlighter (parent),
	  m_syntaxHighlighter(std::make_unique<SyntaxHighlighterComponent>(this)),
	  m_bracketMatcher(std::make_unique<BracketMatcherComponent>(this))
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

void CompositeHighlighter::setSyntaxDefinition(SyntaxDefinition d)
{
	m_definition = d;

	m_syntaxHighlighter->setup();
	m_bracketMatcher->setup();
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

	const auto& t = getTheme();
	const auto& d = getSyntaxDefinition();

	// Search for a single-line comment first.
	int slcStart = 0;
	const auto& slc = d.getSingleLineComment();

	if(!slc.isEmpty()) {
		slcStart = text.indexOf(slc);
		if(slcStart >= 0)
			setFormat(slcStart, text.length() - slcStart, t.getFormat(Theme::SyntaxComment));
		else
			slcStart = text.length();
	}


	const auto& mlcStart = d.getMultiLineCommentStart();
	const auto& mlcEnd = d.getMultiLineCommentEnd();

	const auto mlcEndLength = mlcEnd.length();

	int currPos = 0;
	bool isInComment = (previousBlockState() == 1);
	while(currPos < slcStart) {

		if(isInComment) {
			int idx = text.indexOf(mlcEnd, currPos);
			if(idx==-1)
				idx = slcStart;
			else
				isInComment = false;

			setFormat(currPos, idx+mlcEndLength, t.getFormat(Theme::SyntaxComment));
			currPos = idx+mlcEndLength;
		} else {
			int idx = text.indexOf(mlcStart, currPos);
			if(idx==-1)
				idx = slcStart;
			else
				isInComment = true;

			QStringRef ref(&text, currPos, idx-currPos);
			m_syntaxHighlighter->highlightSection(ref);
			m_bracketMatcher->highlightSection(ref);
			currPos = idx;
		}

	}

	setCurrentBlockState( isInComment );
}

} // namespace ote
