#ifndef SYNTAXDEFINITIONDATA_H
#define SYNTAXDEFINITIONDATA_H

#include <QString>

#include "syntaxdefinition.h"

namespace ote {

class SyntaxDefinitionData
{
public:

	const QString& getName() const { return m_name; }

private:
	friend class SyntaxDefinition;
	friend class SyntaxDefinitionDatabase;

	SyntaxDefinitionData();
	SyntaxDefinitionData(QString name);
	SyntaxDefinitionData(const SyntaxDefinitionData&) = default;
	SyntaxDefinitionData& operator=(const SyntaxDefinitionData&) = default;

	void override(const SyntaxDefinitionData* other);
	bool loadFromFile(QString filePath);

	void addOperatorGroups(const QJsonValue& val);
	void addWordGroup(const QJsonValue& val);
	void addRegexGroup(const QJsonValue& val);

	QString m_name;
	QStringList m_fileExtensions;
	QString m_singleLineComment;
	QString m_multiLineCommentStart;
	QString m_multiLineCommentEnd;
	QVector<SyntaxDefinition::OperatorHighlightGroup> operatorGroups;
	QVector<SyntaxDefinition::WordHighlightGroup> wordGroups;
	QVector<SyntaxDefinition::RegexHighlightGroup> regexGroups;
	Theme::HighlightElements keywordDefault = Theme::MAX_ITEMS;
};

} // namespace ote

#endif // SYNTAXDEFINITIONDATA_H
