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
	const QString& getMimeType() const;

	const QVector<WordHighlightGroup>& getKeywordGroups() const;
	const QVector<RegexHighlightGroup>& getRegexGroups() const;

private:
	friend class SyntaxDefinitionData;
	friend class SyntaxDefinitionDatabase;

	SyntaxDefinition(SyntaxDefinitionData* data);

	SyntaxDefinitionData* m_data;
};

} // namespace ote

#endif // SYNTAXDEFINITION_H
