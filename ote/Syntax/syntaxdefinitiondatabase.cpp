#include "syntaxdefinitiondatabase.h"

#include <QDir>

#include "syntaxdefinitiondata.h"

namespace ote {

std::map<QString, std::unique_ptr<SyntaxDefinitionData>> SyntaxDefinitionDatabase::createDefinitionDB() {
	std::map<QString, std::unique_ptr<SyntaxDefinitionData>> map;

	std::unique_ptr<SyntaxDefinitionData> defaultDefinition(new SyntaxDefinitionData(""));

	//for(int i=0; i<Definition::MAX_ITEMS; ++i)
	//	defaultDefinition->set(static_cast<SyntaxDefinition::HighlightElements>(i), Qt::black);

	map[defaultDefinition->getName()] = std::move(defaultDefinition);

	return map;
}

std::map<QString, std::unique_ptr<SyntaxDefinitionData>> SyntaxDefinitionDatabase::s_definitions = createDefinitionDB();

bool SyntaxDefinitionDatabase::hasDefinition(QString name)
{
	return s_definitions.find(name) != s_definitions.end();
}

SyntaxDefinition SyntaxDefinitionDatabase::getDefinition(QString name)
{
	auto it = s_definitions.find(name);
	if (it != s_definitions.end())
		return SyntaxDefinition(it->second.get());

	return SyntaxDefinition(s_definitions.at("").get());
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

} // namespace ote
