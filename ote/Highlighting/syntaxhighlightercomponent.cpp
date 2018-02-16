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

}

void SyntaxHighlighterComponent::highlightBlock(const QString& text)
{

}

void SyntaxHighlighterComponent::highlightSection(QStringRef text)
{
    const auto& t = m_highlighter->getTheme();
    const auto& d = m_highlighter->getSyntaxDefinition();

    /*static int counter = 0;
    counter += text.length();
    qDebug() << counter;*/

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
                m_highlighter->setFormat(text.position() + i, 1, t.getFormat(item.type));
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
}


} // namespace ote
