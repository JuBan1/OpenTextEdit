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
    void highlightSection(QStringRef text) override;
};

} // namespace ote

#endif // SYNTAXHIGHLIGHTERCOMPONENT_H
