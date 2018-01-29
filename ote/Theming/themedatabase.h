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
	static Theme getTheme(QString name);
	static void addTheme(ThemeData* data);

private:
	ThemeDatabase();

	static std::map<QString, ThemeData*> s_themes;
};

} // namespace ote

#endif // THEMEDATABASE_H
