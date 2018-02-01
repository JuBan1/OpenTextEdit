#ifndef THEMEDATABASE_H
#define THEMEDATABASE_H

#include <QString>
#include <map>

#include "theme.h"

namespace ote {

class ThemeData;

class ThemeDatabase
{
public:
	static bool hasTheme(QString name);
	static Theme getTheme(QString name);
	static void addTheme(ThemeData* data);

	static void loadFromFile(QString filePath);
	static void loadFromDir(QString dirPath);

	static std::vector<Theme> getAllThemes();

private:
	ThemeDatabase() = delete;

	static std::map<QString, ThemeData*> createThemeDB();

	static std::map<QString, ThemeData*> s_themes;
};

} // namespace ote

#endif // THEMEDATABASE_H
