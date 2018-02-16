#ifndef SYNTAXDEFINITION_H
#define SYNTAXDEFINITION_H

#include <QTextCharFormat>
#include <QRegularExpression>

#include "ote/Themes/theme.h"

namespace ote {

class SyntaxDefinitionData;

class SyntaxDefinition
{
public:
    SyntaxDefinition();
    SyntaxDefinition(QString name);

    struct OperatorHighlightGroup {
        QString operators;
        Theme::HighlightElements type;
    };

    struct WordHighlightGroup {
        QStringList words;
        Theme::HighlightElements type;
    };

    struct RegexHighlightGroup {
        QRegularExpression pattern;
        int captureGroup = 0;
        Theme::HighlightElements type;
    };

    const QString& getName() const;
    bool isDefault() const;

    const QVector<OperatorHighlightGroup>& getOperatorGroups() const;
    const QVector<WordHighlightGroup>& getKeywordGroups() const;
    const QVector<RegexHighlightGroup>& getRegexGroups() const;

    Theme::HighlightElements getKeywordDefault() const;

    const QString& getSingleLineComment() const;
    const QString& getMultiLineCommentStart() const;
    const QString& getMultiLineCommentEnd() const;

private:
    friend class SyntaxDefinitionData;
    friend class SyntaxDefinitionDatabase;

    SyntaxDefinition(SyntaxDefinitionData* data);

    SyntaxDefinitionData* m_data;
};

} // namespace ote

#endif // SYNTAXDEFINITION_H
