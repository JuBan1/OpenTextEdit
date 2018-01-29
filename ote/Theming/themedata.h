#ifndef THEMEDATA_H
#define THEMEDATA_H

#include <array>

#include <QColor>

#include "theme.h"

namespace ote {

class ThemeData
{
public:
	ThemeData(QString name);

	QString getName() const { return m_name; }

	QColor getColor(Theme::Colors colorEnum);
	QTextCharFormat getFormat(Theme::Colors colorEnum);

	ThemeData& set(Theme::Colors colorEnum, const QColor& color);
	ThemeData& set(Theme::Colors colorEnum, const QTextCharFormat& format);

	void override(ThemeData* other);

private:
	Q_DISABLE_COPY(ThemeData)

	QString m_name;
	std::array<QTextCharFormat,Theme::MAX_ITEMS> m_colors;
};

} // namespace ote

#endif // THEMEDATA_H
