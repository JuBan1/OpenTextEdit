#ifndef SYNTAXDEFINITIONDATABASE_H
#define SYNTAXDEFINITIONDATABASE_H

#include <QString>
#include <map>

#include "syntaxdefinition.h"

namespace ote {

class SyntaxDefinitionData;

class SyntaxDefinitionDatabase
{
public:
	static bool hasDefinition(QString name);
	static SyntaxDefinition getDefinition(QString name);
	static void addDefinition(SyntaxDefinitionData* data);

	static void loadFromFile(QString filePath);
	static void loadFromDir(QString dirPath);

	static std::vector<SyntaxDefinition> getAllDefinitions();

private:
	SyntaxDefinitionDatabase() = delete;

	static std::map<QString, SyntaxDefinitionData*> createDefinitionDB();

	static std::map<QString, SyntaxDefinitionData*> s_definitions;
};

} // namespace ote

#endif // SYNTAXDEFINITIONDATABASE_H
