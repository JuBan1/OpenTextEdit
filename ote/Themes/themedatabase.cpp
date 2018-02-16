#include "themedatabase.h"

#include <QDir>

#include "themedata.h"

namespace ote {

std::map<QString, std::unique_ptr<ThemeData>> ThemeDatabase::createThemeDB() {
    std::map<QString, std::unique_ptr<ThemeData>> map;

    auto darkTheme = std::unique_ptr<ThemeData>(new ThemeData(""));

    darkTheme->set(Theme::TextEditText, "#eff0f1");
    darkTheme->set(Theme::TextEditActiveText, QColor(45,92,118));
    darkTheme->set(Theme::TextEditBackground, QColor(35,38,41));
    darkTheme->set(Theme::TextEditActiveBackground, QColor(49,54,59,64));

    darkTheme->set(Theme::GutterText, QColor(122,124,125));
    darkTheme->set(Theme::GutterActiveText, QColor(165,166,168));
    darkTheme->set(Theme::GutterBackground, "#31363b");
    darkTheme->set(Theme::GutterActiveBackground, QColor(49,54,59));

    darkTheme->set(Theme::SyntaxComment, "#606060");
    darkTheme->set(Theme::SyntaxString, "#f44f4f");
    darkTheme->set(Theme::SyntaxNumber, "#f67400");
    darkTheme->set(Theme::SyntaxConstant, "#27aeae");
    darkTheme->set(Theme::SyntaxType, "#fdbc4b");
    darkTheme->set(Theme::SyntaxKeyword, "#fdbc4b");

    QTextCharFormat f2;
    f2.setBackground(QColor("#19ffffff"));
    f2.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    darkTheme->set(Theme::SyntaxMatchingBracket, QColor(142,68,173));
    darkTheme->set(Theme::SyntaxMatchingBracket, f2);

    darkTheme->set(Theme::SyntaxOperator, "#cfcfc2");
    darkTheme->set(Theme::SyntaxSymbol, "#cfcfc2");
    darkTheme->set(Theme::SyntaxVariable, "#eff0f1");

    darkTheme->set(Theme::SearchHighlight, QColor(45,92,118,255));


    addTheme(std::move(darkTheme));

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
