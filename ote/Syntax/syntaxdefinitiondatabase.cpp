#include "syntaxdefinitiondatabase.h"

#include <QDebug>
#include <QDir>

#include "syntaxdefinitiondata.h"

namespace ote {

std::map<QString, std::unique_ptr<SyntaxDefinitionData>> SyntaxDefinitionDatabase::createDefinitionDB() {
    std::map<QString, std::unique_ptr<SyntaxDefinitionData>> map;

    std::unique_ptr<SyntaxDefinitionData> defaultDefinition(new SyntaxDefinitionData(s_defaultDefinitionName));

    //for(int i=0; i<Definition::MAX_ITEMS; ++i)
    //	defaultDefinition->set(static_cast<SyntaxDefinition::HighlightElements>(i), Qt::black);

    map[defaultDefinition->getName()] = std::move(defaultDefinition);

    return map;
}

QString SyntaxDefinitionDatabase::s_defaultDefinitionName = "";

std::map<QString, std::unique_ptr<SyntaxDefinitionData>> SyntaxDefinitionDatabase::s_definitions =
        createDefinitionDB();


bool SyntaxDefinitionDatabase::hasDefinition(QString name)
{
    return s_definitions.find(name) != s_definitions.end();
}

SyntaxDefinition SyntaxDefinitionDatabase::getDefinition(QString name)
{
    auto it = s_definitions.find(name);
    if (it != s_definitions.end())
        return SyntaxDefinition(it->second.get());

    return getDefaultDefinition();
}

void SyntaxDefinitionDatabase::addDefinition(std::unique_ptr<SyntaxDefinitionData> data)
{
    auto it = s_definitions.find(data->getName());
    if (it == s_definitions.end())
        s_definitions[data->getName()] = std::move(data);
    else {
        it->second->override(data.get());
    }
}

void SyntaxDefinitionDatabase::loadFromFile(QString filePath)
{
    auto d = std::unique_ptr<SyntaxDefinitionData>(new SyntaxDefinitionData());

    if (d->loadFromFile(filePath))
        addDefinition(std::move(d));
}

void SyntaxDefinitionDatabase::loadFromDir(QString dirPath)
{
    QDir dir(dirPath);

    if(!dir.exists())
        return;

    for(const auto& filePath : dir.entryInfoList(QStringList() << "*.json"))
        loadFromFile(filePath.absoluteFilePath());
}

std::vector<SyntaxDefinition> SyntaxDefinitionDatabase::getAllDefinitions()
{
    std::vector<SyntaxDefinition> definitions;

    for(const auto& item : s_definitions)
        definitions.push_back( SyntaxDefinition(item.second.get()) );

    return definitions;
}

SyntaxDefinition SyntaxDefinitionDatabase::getDefaultDefinition()
{
    return getDefinition(getDefaultDefinitionName());
}

QString SyntaxDefinitionDatabase::getDefaultDefinitionName()
{
    return s_defaultDefinitionName;
}

void SyntaxDefinitionDatabase::setDefaultDefinitionName(const QString& newName)
{
    auto it = s_definitions.find(s_defaultDefinitionName);
    auto data = std::move(it->second);
    s_definitions.erase(it);

    std::unique_ptr<SyntaxDefinitionData> newDef(new SyntaxDefinitionData(newName));
    newDef->override(data.get());
    s_definitions[newName] = std::move(newDef);
    s_defaultDefinitionName = newName;
}

SyntaxDefinition SyntaxDefinitionDatabase::findDefinitionForFileExtension(QString ext)
{
    if(ext.isEmpty())
        return getDefaultDefinition();

    ext = ext.mid( ext.lastIndexOf('.'), -1 );
    if(ext.startsWith('.'))
        ext = ext.remove(0,1);

    for(const auto& sd : s_definitions) {
        if(sd.second->m_fileExtensions.contains(ext))
            return SyntaxDefinition(sd.second.get());
    }

    return getDefaultDefinition();
}

} // namespace ote
