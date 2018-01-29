#include "syntaxhighlightercomponent.h"

#include <QDebug>

#include "compositehighlighter.h"

#include "ote/Theming/theme.h"

namespace ote {

SyntaxHighlighterComponent::SyntaxHighlighterComponent(CompositeHighlighter* h)
	: HighlighterComponent(h)
{

}

void SyntaxHighlighterComponent::setup()
{
	Theme t = m_highlighter->getTheme();

	HighlightingRule rule;
	highlightingRules.clear();

	QStringList keywordPatterns;
	keywordPatterns << "class" << "const" << "enum" << "explicit" << "friend" << "inline"
					<< "namespace" << "operator" << "private" << "protected" << "public"
					<< "signals" << "slots" << "static" << "struct" << "template"
					<< "typedef" << "typename" << "union" << "virtual" << "volatile"
					<< "return" << "case" << "switch" << "while";

	wordRules.append( {keywordPatterns, t.getFormat(Theme::SyntaxKeyword)} );

	QStringList typePatterns;
	typePatterns << "char" << "double" << "int" << "long" << "short" << "signed"
				 << "unsigned" << "void" << "bool";

	wordRules.append( {typePatterns, t.getFormat(Theme::SyntaxType)} );


	QStringList defPatterns;
	defPatterns << "false" << "true" << "null";
	wordRules.append( {defPatterns, t.getFormat(Theme::SyntaxConstant)} );


	// Misc symbols
	rule.pattern = QRegularExpression("\\(|\\)|\\{|\\}|\\[|\\]|,|;|=|\\+|\\-|\\.|\\:");
	rule.format = t.getFormat(Theme::SyntaxSymbol);
	highlightingRules.append(rule);

	// Operators
	rule.pattern = QRegularExpression("\\&|\\=|\\!|<|>|\\*|-|\\+|\\.\\?|\\%");
	rule.format = t.getFormat(Theme::SyntaxOperator);
	highlightingRules.append(rule);

	// Numbers
	rule.pattern = QRegularExpression("\\W(\\d+\\w*)");
	rule.format = t.getFormat(Theme::SyntaxNumber);
	rule.captureGroup = 1;
	highlightingRules.append(rule);

	// Double-quote string
	rule.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
	rule.format = t.getFormat(Theme::SyntaxString);
	rule.captureGroup = 0;
	highlightingRules.append(rule);

	// Single-quote string
	rule.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)*'");
	rule.format = t.getFormat((Theme::SyntaxString));
	rule.captureGroup = 0;
	highlightingRules.append(rule);

	// Single-line comments
	rule.pattern = QRegularExpression("//[^\n]*");
	rule.format = t.getFormat(Theme::SyntaxComment);
	highlightingRules.append(rule);

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
