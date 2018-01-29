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

	enum Colors
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

		SearchHighlight,

		MAX_ITEMS
	};

	Theme();
	Theme(QString name);
	Theme(ThemeData* data);

	QString getName() const;

	QTextCharFormat getFormat(Colors c) const;
	QColor getColor(Colors tc) const;

private:
	ThemeData* m_data;
};

} // namespace ote

#endif // THEME_H
