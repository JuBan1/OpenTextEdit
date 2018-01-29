#include "themedata.h"

namespace ote {

ThemeData::ThemeData(QString name)
	: m_name(name)
{

}

QTextCharFormat ThemeData::getFormat(Theme::Colors colorEnum)
{
	return m_colors[colorEnum];
}


QColor ThemeData::getColor(Theme::Colors colorEnum) {
	return m_colors[colorEnum].foreground().color();
}

ThemeData& ThemeData::set(Theme::Colors colorEnum, const QTextCharFormat& format) {
	m_colors[colorEnum] = format;
	return *this;
}

ThemeData& ThemeData::set(Theme::Colors colorEnum, const QColor& color)
{
	m_colors.at(colorEnum).setForeground(color);
	return *this;
}

void ThemeData::override(ThemeData* other)
{
	m_colors = other->m_colors;
}

} // namespace ote
