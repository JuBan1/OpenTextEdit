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
	auto d = m_highlighter->getSyntaxDefinition();
	Theme t = m_highlighter->getTheme();

	highlightingRules.clear();


	for(const auto& l : d.getKeywordGroups()){
		wordRules.append( {l.words, t.getFormat(l.type)} );
	}

	for(const auto& l : d.getRegexGroups()){
		highlightingRules.append( {l.pattern, t.getFormat(l.type), l.captureGroup} );
	}

	// Single-line comments
	/*rule.pattern = QRegularExpression("//[^\n]*");
	rule.format = t.getFormat(Theme::SyntaxComment);
	highlightingRules.append(rule);*/

	// Multi-line comments
	multiLineCommentFormat = t.getFormat(Theme::SyntaxComment);
	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
}

void SyntaxHighlighterComponent::highlightBlock(const QString& text)
{
	// highlight keywords
	QRegularExpressionMatchIterator matchIterator = QRegularExpression("\\b\\w+\\b").globalMatch(text);
	while (matchIterator.hasNext()) {
		auto match = matchIterator.next();

		for (const auto& rule : wordRules)
			if (rule.words.contains(match.captured()))
				m_highlighter->setFormat(match.capturedStart(),
										 match.capturedLength(),
										 rule.format);
	}

	// Other generic highlighting rules
	for (const HighlightingRule& rule : highlightingRules) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			m_highlighter->setFormat(match.capturedStart(rule.captureGroup),
									 match.capturedLength(rule.captureGroup),
									 rule.format);
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
		m_highlighter->setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
	}
}


} // namespace ote
