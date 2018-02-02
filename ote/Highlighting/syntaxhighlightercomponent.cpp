#include "syntaxhighlightercomponent.h"

#include <QDebug>

#include "compositehighlighter.h"

#include "ote/Themes/theme.h"

namespace ote {

SyntaxHighlighterComponent::SyntaxHighlighterComponent(CompositeHighlighter* h)
	: HighlighterComponent(h)
{

}

void SyntaxHighlighterComponent::setup()
{
	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
}

void SyntaxHighlighterComponent::highlightBlock(const QString& text)
{
	auto t = m_highlighter->getTheme();
	auto d = m_highlighter->getSyntaxDefinition();

	// highlight keywords
	QRegularExpressionMatchIterator matchIterator = QRegularExpression("\\b\\w+\\b").globalMatch(text);
	while (matchIterator.hasNext()) {
		auto match = matchIterator.next();

		for (const auto& rule : d.getKeywordGroups())
			if (rule.words.contains(match.captured()))
				m_highlighter->setFormat(match.capturedStart(),
										 match.capturedLength(),
										 t.getFormat(rule.type));
	}

	// highlight operators
	for(const auto& item : d.getOperatorGroups()) {
		for (int i=0; i<text.length(); ++i) {
			if(item.operators.contains(text[i]))
			   m_highlighter->setFormat(i, 1, t.getFormat(item.type));
		}
	}

	// Other generic highlighting rules
	for (const auto& rule : d.getRegexGroups()) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			m_highlighter->setFormat(match.capturedStart(rule.captureGroup),
									 match.capturedLength(rule.captureGroup),
									 t.getFormat(rule.type));
		}
	}

	// Handle multi-line comment highlighting
	m_highlighter->setCurrentBlockState(0);

	int startIndex = 0;
	if (m_highlighter->previousBlockState() != 1)
		startIndex = text.indexOf(commentStartExpression);

	while (startIndex >= 0) {
		QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
		int endIndex = match.capturedStart();
		int commentLength = 0;
		if (endIndex == -1) {
			m_highlighter->setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex
							+ match.capturedLength();
		}
		m_highlighter->setFormat(startIndex, commentLength, t.getFormat(Theme::SyntaxComment));
		startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
	}
}


} // namespace ote
