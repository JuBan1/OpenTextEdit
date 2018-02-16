#ifndef THEME_H
#define THEME_H

#include <map>
#include <memory>

#include <QColor>
#include <QTextCharFormat>

namespace ote {

class ThemeData;

class Theme
{
public:

    // If you add/remove any items here be sure to also change the impl of elementToString(...) and
    // stringToElement(...)
    enum HighlightElements
    {
        TextEditText,
        TextEditActiveText,
        TextEditBackground,
        TextEditActiveBackground,

        GutterText,
        GutterActiveText,
        GutterBackground,
        GutterActiveBackground,

        SyntaxComment,
        SyntaxString,
        SyntaxNumber,
        SyntaxConstant,
        SyntaxType,
        SyntaxKeyword,
        SyntaxMatchingBracket,
        SyntaxOperator,
        SyntaxSymbol,
        SyntaxVariable,

        SearchHighlight,

        MAX_ITEMS
    };

    static const char* elementToString(HighlightElements elem);

    static HighlightElements stringToElement(const char* elem);
    static HighlightElements stringToElement(const QString& elem);

    Theme();
    Theme(QString name);

    QString getName() const;
    bool isDefault() const;

    QTextCharFormat getFormat(HighlightElements c) const;
    QColor getColor(HighlightElements tc) const;

private:
    friend class ThemeData;
    friend class ThemeDatabase;

    Theme(ThemeData* data);
    ThemeData* m_data;
};

} // namespace ote

#endif // THEME_H
