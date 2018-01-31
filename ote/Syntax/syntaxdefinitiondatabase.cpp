#include "syntaxdefinitiondatabase.h"

#include "syntaxdefinitiondata.h"

namespace ote {

std::map<QString, SyntaxDefinitionData*> SyntaxDefinitionDatabase::createDefinitionDB() {
	std::map<QString, SyntaxDefinitionData*> map;

	SyntaxDefinitionData* defaultDefinition = new SyntaxDefinitionData("");

	//for(int i=0; i<Definition::MAX_ITEMS; ++i)
	//	defaultDefinition->set(static_cast<SyntaxDefinition::HighlightElements>(i), Qt::black);

	map[defaultDefinition->getName()] = defaultDefinition;

	return map;
}

std::map<QString, SyntaxDefinitionData*> SyntaxDefinitionDatabase::s_definitions = createDefinitionDB();

bool SyntaxDefinitionDatabase::hasDefinition(QString name)
{
	return s_definitions.find(name) != s_definitions.end();
}

SyntaxDefinition SyntaxDefinitionDatabase::getDefinition(QString name)
{
	auto it = s_definitions.find(name);
	if (it != s_definitions.end())
		return SyntaxDefinition(it->second);

	return SyntaxDefinition(s_definitions.at(""));
}

void SyntaxDefinitionDatabase::addDefinition(SyntaxDefinitionData* data)
{
	auto it = s_definitions.find(data->getName());
	if (it == s_definitions.end())
		s_definitions[data->getName()] = data;
	else {
		it->second->override(data);
		delete data;
	}
}

void SyntaxDefinitionDatabase::loadFromFile(QString filePath)
{
	SyntaxDefinitionData* d = new SyntaxDefinitionData();
	if (d->loadFromFile(filePath))
		addDefinition(d);
	else
		delete d;

}

std::vector<SyntaxDefinition> SyntaxDefinitionDatabase::getAllDefinitions()
{
	std::vector<SyntaxDefinition> definitions;

	for(const auto& item : s_definitions)
		definitions.push_back( SyntaxDefinition(item.second) );

	return definitions;
}

} // namespace ote
