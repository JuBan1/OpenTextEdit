#include "themedatabase.h"

#include <QDir>

#include "themedata.h"

namespace ote {

std::map<QString, ThemeData*> ThemeDatabase::createThemeDB() {
	std::map<QString, ThemeData*> map;

	ThemeData* defaultTheme = new ThemeData("");

	for(int i=0; i<Theme::MAX_ITEMS; ++i)
		defaultTheme->set(static_cast<Theme::HighlightElements>(i), Qt::black);

	map[defaultTheme->getName()] = defaultTheme;

	return map;
}

std::map<QString, ThemeData*> ThemeDatabase::s_themes = createThemeDB();

bool ThemeDatabase::hasTheme(QString name)
{
	return s_themes.find(name) != s_themes.end();
}

Theme ThemeDatabase::getTheme(QString name)
{
	auto it = s_themes.find(name);
	if (it != s_themes.end())
		return Theme(it->second);

	return Theme(s_themes.at(""));
}

void ThemeDatabase::addTheme(ThemeData* data)
{
	auto it = s_themes.find(data->getName());
	if (it == s_themes.end())
		s_themes[data->getName()] = data;
	else {
		it->second->override(data);
		delete data;
	}
}

void ThemeDatabase::loadFromFile(QString filePath)
{
	ThemeData* d = new ThemeData();
	if (d->loadFromFile(filePath))
		addTheme(d);
	else
		delete d;

}

void ThemeDatabase::loadFromDir(QString dirPath)
{
	QDir dir(dirPath);

	if(!dir.exists())
		return;

	for(const auto& filePath : dir.entryInfoList(QStringList() << "*.json"))
		loadFromFile(filePath.absoluteFilePath());
}

std::vector<Theme> ThemeDatabase::getAllThemes()
{
	std::vector<Theme> themes;

	for(const auto& item : s_themes)
		themes.push_back( Theme(item.second) );

	return themes;
}

} // namespace ote
