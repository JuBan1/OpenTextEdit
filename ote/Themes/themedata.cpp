#include "themedata.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

#include "themedatabase.h"

namespace ote {

ThemeData::ThemeData()
{

}

ThemeData::ThemeData(QString name)
	: m_name(name)
{

}

QTextCharFormat ThemeData::getFormat(Theme::HighlightElements colorEnum) const
{
	return m_formats[colorEnum];
}


QColor ThemeData::getColor(Theme::HighlightElements colorEnum) const {
	return m_formats[colorEnum].foreground().color();
}

ThemeData& ThemeData::set(Theme::HighlightElements elem, const QTextCharFormat& format) {
	m_formats[elem] = format;
	return *this;
}

ThemeData& ThemeData::set(Theme::HighlightElements elem, const QColor& color)
{
	m_formats.at(elem).setForeground(color);
	return *this;
}

void ThemeData::override(const ThemeData* other)
{
	m_formats = other->m_formats;
}

QTextCharFormat::UnderlineStyle stringToULStyle(QString s) {
	s = s.toLower();
	if(s == "nounderline") return QTextCharFormat::NoUnderline;
	if(s == "singleunderline") return QTextCharFormat::SingleUnderline;
	if(s == "dashunderline") return QTextCharFormat::DashUnderline;
	if(s == "dotline") return QTextCharFormat::DotLine;
	if(s == "dashdotline") return QTextCharFormat::DashDotLine;
	if(s == "dashdotdotline") return QTextCharFormat::DashDotDotLine;
	if(s == "waveunderline") return QTextCharFormat::WaveUnderline;
	if(s == "spellcheckunderline") return QTextCharFormat::SpellCheckUnderline;
	return QTextCharFormat::NoUnderline;
}

static bool readValue(const QJsonObject& obj, const QString& key, QString& ref) {
	const auto& val = obj[key];
	if(val.isUndefined())
		return false;
	if (!val.isString()) {
		qWarning() << "Error loading theme: " << key << "expected string but found" << val;
		return false;
	}
	ref = val.toString();
	return true;
}

static bool readValue(const QJsonObject& obj, const QString& key, bool& ref) {
	const auto& val = obj[key];
	if(val.isUndefined())
		return false;
	if (!val.isBool()) {
		qWarning() << "Error loading theme: " << key << "expected bool but found" << val;
		return false;
	}
	ref = val.toBool();
	return true;
}

static QColor toQColor(QString s) {
	QColor c(s);
	if(!c.isValid()) qWarning() << "Error loading theme: expected color but got " << s;
	return c;
}

static void readTextCharFormat(QTextCharFormat& fmt, const QJsonValue& val) {
	QString line;
	bool value;

	if(val.isString()) {
		fmt.setForeground( toQColor(val.toString()) );
		return;
	}

	if(!val.isObject()) {
		qWarning() << "Error loading theme: expected string or object but got " << val;
		return;
	}

	const auto& obj = val.toObject();

	if (readValue(obj, "foregroundColor", line))
		fmt.setForeground( toQColor(line) );

	if (readValue(obj, "backgroundColor", line))
		fmt.setBackground( toQColor(line) );

	if (readValue(obj, "underlineColor", line))
		fmt.setUnderlineColor( toQColor(line) );

	if (readValue(obj, "underlineStyle", line))
		fmt.setUnderlineStyle( stringToULStyle(line) );

	if (readValue(obj, "italic", value))
		fmt.setFontItalic(value);

	if (readValue(obj, "bold", value))
		fmt.setFontWeight(value ? QFont::Bold : QFont::Normal);
}

bool ThemeData::loadFromFile(QString filePath)
{
	QFile file(filePath);
	file.open(QFile::ReadOnly);

	if (!file.isOpen() || !file.isReadable())
		return false;

	QJsonParseError err;
	auto doc = QJsonDocument::fromJson(file.readAll(), &err);

	if(err.error != QJsonParseError::NoError) {
		qWarning() << "Error loading theme: could not validate Json file: " << err.errorString();
		return false;
	}

	if (doc.isNull()) {
		qWarning() << "Error loading theme: empty or malformed Json";
		return false;
	}

	const auto& docObj = doc.object();

	QString line;
	if (readValue(docObj, "name", line))
		m_name = line;
	else
		return false;

	if (readValue(docObj, "base", line)) {
		if(ThemeDatabase::hasTheme(line)) {
			override(ThemeDatabase::getTheme(line).m_data);
		} else {
			qWarning() << "Error loading theme: base theme " << line << " not found.";
			return false;
		}
	} else
		for(auto& fmt : m_formats)
			fmt = QTextCharFormat();

	for(size_t i=0; i<m_formats.size(); ++i) {
		auto& fmt = m_formats[i];
		auto he = static_cast<Theme::HighlightElements>(i);
		auto heName = Theme::elementToString(he);

		auto obj = doc[heName];

		if(!obj.isUndefined())
			readTextCharFormat(fmt, obj);
	}

	return true;
}

} // namespace ote
