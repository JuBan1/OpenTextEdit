#include "theme.h"

#include "themedata.h"
#include "themedatabase.h"

namespace ote {

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

QTextCharFormat Theme::getFormat(Colors c) const
{
	return m_data->getFormat(c);
}

QColor Theme::getColor(Colors c) const {
	return m_data->getColor(c);
}

} // namespace ote
