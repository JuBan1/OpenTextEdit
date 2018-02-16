#include "syntaxdefinition.h"

#include "syntaxdefinitiondata.h"
#include "syntaxdefinitiondatabase.h"

namespace ote {

SyntaxDefinition::SyntaxDefinition()
    : m_data( SyntaxDefinitionDatabase::getDefinition("").m_data ) {}

SyntaxDefinition::SyntaxDefinition(QString name)
    : m_data( SyntaxDefinitionDatabase::getDefinition(name).m_data ) {}

SyntaxDefinition::SyntaxDefinition(SyntaxDefinitionData* data)
    : m_data(data) {}

const QString& SyntaxDefinition::getName() const
{
    return m_data->getName();
}

bool SyntaxDefinition::isDefault() const
{
    return getName().isEmpty();
}

const QVector<SyntaxDefinition::OperatorHighlightGroup>& SyntaxDefinition::getOperatorGroups() const
{
    return m_data->operatorGroups;
}

const QVector<SyntaxDefinition::WordHighlightGroup>& SyntaxDefinition::getKeywordGroups() const
{
    return m_data->wordGroups;
}

const QVector<SyntaxDefinition::RegexHighlightGroup>& SyntaxDefinition::getRegexGroups() const
{
    return m_data->regexGroups;
}

Theme::HighlightElements SyntaxDefinition::getKeywordDefault() const
{
    return m_data->keywordDefault;
}

const QString&SyntaxDefinition::getSingleLineComment() const
{
    return m_data->m_singleLineComment;
}

const QString&SyntaxDefinition::getMultiLineCommentStart() const
{
    return m_data->m_multiLineCommentStart;
}

const QString&SyntaxDefinition::getMultiLineCommentEnd() const
{
    return m_data->m_multiLineCommentEnd;
}


} // namespace ote
