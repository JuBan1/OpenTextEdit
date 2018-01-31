#ifndef SYNTAXDEFINITIONDATA_H
#define SYNTAXDEFINITIONDATA_H

#include <QString>

#include "syntaxdefinition.h"

namespace ote {

class SyntaxDefinitionData
{
public:

	const QString& getName() const { return m_name; }
	const QString& getMimeType() const { return m_mimeType; }

private:
	Q_DISABLE_COPY(SyntaxDefinitionData)
	friend class SyntaxDefinition;
	friend class SyntaxDefinitionDatabase;

	SyntaxDefinitionData();
	SyntaxDefinitionData(QString name);

	void override(const SyntaxDefinitionData* other);
	bool loadFromFile(QString filePath);

	void addWordGroup(const QJsonValue& val);
	void addRegexGroup(const QJsonValue& val);

	QString m_name;
	QString m_mimeType;
	QVector<SyntaxDefinition::WordHighlightGroup> wordGroups;
	QVector<SyntaxDefinition::RegexHighlightGroup> regexGroups;
};

} // namespace ote

#endif // SYNTAXDEFINITIONDATA_H