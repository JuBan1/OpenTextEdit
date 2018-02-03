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
	bool found;
	while (matchIterator.hasNext()) {
		auto match = matchIterator.next();
		found = false;

		for (const auto& rule : d.getKeywordGroups()) {
			if (rule.words.contains(match.captured())) {
				m_highlighter->setFormat(match.capturedStart(),
										 match.capturedLength(),
										 t.getFormat(rule.type));
				found = true;
			}
			if(found) break;
		}
		if(!found && d.getKeywordDefault() != Theme::MAX_ITEMS) {
			m_highlighter->setFormat(match.capturedStart(),
									 match.capturedLength(),
									 t.getFormat(d.getKeywordDefault()));
		}

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

	// Search for a single-line comment first.
	int slcStart = 0;
	const auto& slc = d.getSingleLineComment();

	if(!slc.isEmpty()) {
		slcStart = text.indexOf(slc);
		if(slcStart >= 0)
			m_highlighter->setFormat(slcStart, text.length() - slcStart, t.getFormat(Theme::SyntaxComment));
		else
			slcStart = text.length();
	}

	int startIndex = 0;

	// Handle multi-line comment highlighting
	m_highlighter->setCurrentBlockState(0);

	const auto& mlcStart = d.getMultiLineCommentStart();
	const auto& mlcEnd = d.getMultiLineCommentEnd();

	if(mlcStart.isEmpty() || mlcEnd.isEmpty())
		return;

	const auto mlcEndLength = mlcEnd.length();


	if (m_highlighter->previousBlockState() != 1)
		startIndex = text.indexOf(mlcStart);

	while (startIndex >= 0 && startIndex<=slcStart) {
		const int endIndex = text.indexOf(mlcEnd, startIndex);
		int commentLength = 0;
		if (endIndex == -1) {
			m_highlighter->setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex
							+ mlcEndLength;
		}

		m_highlighter->setFormat(startIndex, commentLength, t.getFormat(Theme::SyntaxComment));
		startIndex = text.indexOf(mlcStart, startIndex + commentLength);
	}
}


} // namespace ote
