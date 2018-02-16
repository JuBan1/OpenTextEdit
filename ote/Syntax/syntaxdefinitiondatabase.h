#ifndef SYNTAXDEFINITIONDATABASE_H
#define SYNTAXDEFINITIONDATABASE_H

#include <QString>
#include <map>
#include <memory>

#include "syntaxdefinition.h"

namespace ote {

class SyntaxDefinitionData;

class SyntaxDefinitionDatabase
{
public:
    static bool hasDefinition(QString name);
    static SyntaxDefinition getDefinition(QString name);
    static void addDefinition(std::unique_ptr<SyntaxDefinitionData> data);

    static void loadFromFile(QString filePath);
    static void loadFromDir(QString dirPath);

    static std::vector<SyntaxDefinition> getAllDefinitions();

    static SyntaxDefinition findDefinitionForFileExtension(QString ext);

private:
    SyntaxDefinitionDatabase() = delete;

    static std::map<QString, std::unique_ptr<SyntaxDefinitionData>> createDefinitionDB();

    static std::map<QString, std::unique_ptr<SyntaxDefinitionData>> s_definitions;
};

} // namespace ote

#endif // SYNTAXDEFINITIONDATABASE_H
