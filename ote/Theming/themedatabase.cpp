#include "themedatabase.h"

#include "themedata.h"

namespace ote {

std::map<QString, ThemeData*> createThemeDB() {
	std::map<QString, ThemeData*> map;

	ThemeData* defaultTheme = new ThemeData("");

	for(int i=0; i<Theme::MAX_ITEMS; ++i)
		defaultTheme->set(static_cast<Theme::Colors>(i), Qt::black);

	map[defaultTheme->getName()] = defaultTheme;

	return map;
}

std::map<QString, ThemeData*> ThemeDatabase::s_themes = createThemeDB();

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

} // namespace ote
