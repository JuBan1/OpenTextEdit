#include "theme.h"

#include "themedata.h"
#include "themedatabase.h"

namespace ote {

#define CASE(item) case item: return #item;
#define IF(item) if(strcmp(elem, #item)==0) return item;

const char* Theme::elementToString(Theme::HighlightElements elem)
{
    switch(elem) {
    CASE(TextEditText)
    CASE(TextEditActiveText)
    CASE(TextEditBackground)
    CASE(TextEditActiveBackground)
    CASE(GutterText)
    CASE(GutterActiveText)
    CASE(GutterBackground)
    CASE(GutterActiveBackground)
    CASE(SyntaxComment)
    CASE(SyntaxString)
    CASE(SyntaxNumber)
    CASE(SyntaxConstant)
    CASE(SyntaxType)
    CASE(SyntaxKeyword)
    CASE(SyntaxMatchingBracket)
    CASE(SyntaxOperator)
    CASE(SyntaxSymbol)
    CASE(SyntaxVariable)
    CASE(SearchHighlight)
    CASE(MAX_ITEMS)
    }
}

Theme::HighlightElements Theme::stringToElement(const char* elem)
{
    IF(TextEditText)
    IF(TextEditActiveText)
    IF(TextEditBackground)
    IF(TextEditActiveBackground)
    IF(GutterText)
    IF(GutterActiveText)
    IF(GutterBackground)
    IF(GutterActiveBackground)
    IF(SyntaxComment)
    IF(SyntaxString)
    IF(SyntaxNumber)
    IF(SyntaxConstant)
    IF(SyntaxType)
    IF(SyntaxKeyword)
    IF(SyntaxMatchingBracket)
    IF(SyntaxOperator)
    IF(SyntaxSymbol)
    IF(SyntaxVariable)
    IF(SearchHighlight)
    IF(MAX_ITEMS)

    return MAX_ITEMS;
}

Theme::HighlightElements Theme::stringToElement(const QString& elem)
{
    return stringToElement(static_cast<const char*>(elem.toLatin1()));
}

Theme::Theme()
    : m_data( ThemeDatabase::getTheme("").m_data ) {}

Theme::Theme(QString name)
    : m_data( ThemeDatabase::getTheme(name).m_data ) {}

Theme::Theme(ThemeData* data)
    : m_data(data) {}

QString Theme::getName() const
{
    return m_data->getName();
}

bool Theme::isDefault() const
{
    return getName().isEmpty();
}

QTextCharFormat Theme::getFormat(HighlightElements c) const
{
    return m_data->getFormat(c);
}

QColor Theme::getColor(HighlightElements c) const {
    return m_data->getColor(c);
}

} // namespace ote
