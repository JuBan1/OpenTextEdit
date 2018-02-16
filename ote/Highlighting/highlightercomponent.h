#ifndef HIGHLIGHTERCOMPONENT_H
#define HIGHLIGHTERCOMPONENT_H

#include <QString>

namespace ote {

class CompositeHighlighter;

class HighlighterComponent
{
public:
    HighlighterComponent(CompositeHighlighter* h);
    virtual ~HighlighterComponent();

    virtual void setup() = 0;
    virtual void highlightBlock(const QString &text) = 0;
    virtual void highlightSection(QStringRef ref) {}

protected:

    CompositeHighlighter* m_highlighter;
};

} // namespace ote

#endif // HIGHLIGHTERCOMPONENT_H
