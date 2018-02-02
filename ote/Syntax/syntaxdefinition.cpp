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

const QVector<SyntaxDefinition::OperatorHighlightGroup>&SyntaxDefinition::getOperatorGroups() const
{
	return m_data->operatorGroups;
}

const QVector<SyntaxDefinition::WordHighlightGroup>&SyntaxDefinition::getKeywordGroups() const
{
	return m_data->wordGroups;
}

const QVector<SyntaxDefinition::RegexHighlightGroup>&SyntaxDefinition::getRegexGroups() const
{
	return m_data->regexGroups;
}


} // namespace ote
