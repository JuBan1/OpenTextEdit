#include "themedatabase.h"

#include <QDir>

#include "themedata.h"

namespace ote {

std::map<QString, std::unique_ptr<ThemeData>> ThemeDatabase::createThemeDB() {
	std::map<QString, std::unique_ptr<ThemeData>> map;

	std::unique_ptr<ThemeData> defaultTheme(new ThemeData(""));

	for(int i=0; i<Theme::MAX_ITEMS; ++i)
		defaultTheme->set(static_cast<Theme::HighlightElements>(i), Qt::black);

	map[defaultTheme->getName()] = std::move(defaultTheme);

	return map;
}

std::map<QString, std::unique_ptr<ThemeData>> ThemeDatabase::s_themes = createThemeDB();

bool ThemeDatabase::hasTheme(QString name)
{
	return s_themes.find(name) != s_themes.end();
}

Theme ThemeDatabase::getTheme(QString name)
{
	auto it = s_themes.find(name);
	if (it != s_themes.end())
		return Theme(it->second.get());

	return Theme(s_themes.at("").get());
}

void ThemeDatabase::addTheme(std::unique_ptr<ThemeData> data)
{
	auto it = s_themes.find(data->getName());
	if (it == s_themes.end())
		s_themes[data->getName()] = std::move(data);
	else {
		it->second->override(data.get());
	}
}

void ThemeDatabase::loadFromFile(QString filePath)
{
	auto d = std::unique_ptr<ThemeData>(new ThemeData());

	if (d->loadFromFile(filePath))
		addTheme(std::move(d));
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
		themes.push_back( Theme(item.second.get()) );

	return themes;
}

} // namespace ote
