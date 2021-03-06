#include "syntaxdefinitiondata.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

#include <QDebug>

namespace ote {


SyntaxDefinitionData::SyntaxDefinitionData()
{

}

SyntaxDefinitionData::SyntaxDefinitionData(QString name)
    : m_name(name)
{

}

void SyntaxDefinitionData::override(const SyntaxDefinitionData* other)
{
    const auto name = m_name;
    *this = *other;
    m_name = name;
}

void SyntaxDefinitionData::addOperatorGroups(const QJsonValue& val)
{
    if(!val.isObject()) {
        qWarning() << "Error loading syntax definition: expected object but found " << val;
        return;
    }
    const auto& obj = val.toObject();

    if(!obj["type"].isString() || !obj["operators"].isString()) {
        qWarning() << "Error loading syntax definition: operator group needs a 'type' and 'operators' member";
        return;
    }

    const auto& type = obj["type"].toString();
    const auto& operators = obj["operators"].toString();

    const auto highlightType = Theme::stringToElement(type);

    if(highlightType == Theme::MAX_ITEMS) {
        qWarning() << "Error loading syntax definition: unknown type " << highlightType;
        return;
    }

    operatorGroups.append( {operators, highlightType} );
}

void SyntaxDefinitionData::addWordGroup(const QJsonValue&  val) {
    if(!val.isObject()) {
        qWarning() << "Error loading syntax definition: expected object but found " << val;
        return;
    }
    const auto& obj = val.toObject();

    if(!obj["type"].isString() || !obj["keywords"].isArray()) {
        qWarning() << "Error loading syntax definition: word group needs a 'type' and 'keywords' member";
        return;
    }

    const auto& type = obj["type"].toString();
    const auto& words = obj["keywords"].toArray();

    const auto highlightType = Theme::stringToElement(type);

    if(highlightType == Theme::MAX_ITEMS) {
        qWarning() << "Error loading syntax definition: unknown 'type'";
        return;
    }

    QStringList list;
    for(const auto& w : words) {
        if(w.isString())
            list << w.toString();
    }

    wordGroups.append( {list, highlightType} );
}

void SyntaxDefinitionData::addRegexGroup(const QJsonValue& val)
{
    if(!val.isObject()) {
        qWarning() << "Error loading syntax definition: expected object but found " << val;
        return;
    }
    const auto& obj = val.toObject();

    if(!obj["type"].isString() || !obj["regex"].isString()) {
        qWarning() << "Error loading syntax definition: regex group needs a 'type' and 'regex' member";
        return;
    }

    const auto& type = obj["type"].toString();
    const auto& regex = obj["regex"].toString();
    const auto captureGroup = obj["captureGroup"].toInt(0);

    const auto highlightType = Theme::stringToElement(type);

    if(highlightType == Theme::MAX_ITEMS) {
        qWarning() << "Error loading syntax definition: unknown 'type'";
        return;
    }

    regexGroups.append( {QRegularExpression(regex), captureGroup, highlightType} );
}

bool SyntaxDefinitionData::loadFromFile(QString filePath)
{
    QFile file(filePath);
    file.open(QFile::ReadOnly);

    if (!file.isOpen() || !file.isReadable())
        return false;

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(file.readAll(), &err);

    if(err.error != QJsonParseError::NoError) {
        qWarning() << "Error loading syntax definition: could not validate Json file: " << err.errorString();
        return false;
    }

    if (doc.isNull()) {
        qWarning() << "Error loading syntax definition: empty or malformed Json";
        return false;
    }

    const auto& docObj = doc.object();

    if(!doc["name"].isString()) {
        qWarning() << "Error loading syntax definition: definition needs a 'name' member";
        return false;
    }

    m_name = doc["name"].toString();

    const auto& extensions = docObj["fileExtensions"].toArray();
    if(!extensions.isEmpty()) {
        for(const auto it : extensions)
            if(it.isString())
                m_fileExtensions << it.toString();
    }

    m_singleLineComment = doc["singleLineComment"].toString();
    m_multiLineCommentStart = doc["multiLineCommentStart"].toString();
    m_multiLineCommentEnd= doc["multiLineCommentEnd"].toString();


    const auto& operators = docObj["operators"].toArray();
    for(const auto& def : operators) {
        addOperatorGroups(def);
    }

    const auto& keywords = docObj["keywords"].toArray();
    for(const auto& def : keywords) {
        if(!def["default"].isUndefined()) {
            keywordDefault = Theme::stringToElement(def["default"].toString());
            if(keywordDefault == Theme::MAX_ITEMS)
                qWarning() << "Error loading syntax definition: unknown 'type' for keyword default";
            continue;
        }

        addWordGroup(def);
    }

    const auto& regexes = docObj["regexRules"].toArray();
    for(const auto& reg : regexes) {
        addRegexGroup(reg);
    }

    return true;
}



} // namespace ote
