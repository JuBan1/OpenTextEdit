#ifndef THEMEDATA_H
#define THEMEDATA_H

#include <array>

#include <QColor>

#include "theme.h"

namespace ote {

class ThemeData
{
public:


    QString getName() const { return m_name; }

    QColor getColor(Theme::HighlightElements colorEnum) const;
    QTextCharFormat getFormat(Theme::HighlightElements colorEnum) const;

    ThemeData& set(Theme::HighlightElements elem, const QColor& color);
    ThemeData& set(Theme::HighlightElements elem, const QTextCharFormat& format);

private:
    friend class ThemeDatabase;

    ThemeData();
    ThemeData(QString name);
    ThemeData(const ThemeData&) = default;
    ThemeData& operator=(const ThemeData&) = default;

    void override(const ThemeData* other);
    bool loadFromFile(QString filePath);

    QString m_name;
    std::array<QTextCharFormat,Theme::MAX_ITEMS> m_formats;
};

} // namespace ote

#endif // THEMEDATA_H
