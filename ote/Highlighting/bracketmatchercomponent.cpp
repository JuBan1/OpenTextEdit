#include "bracketmatchercomponent.h"

#include "compositehighlighter.h"

#include <array>

namespace ote {

BracketMatcherComponent::BracketMatcherComponent(CompositeHighlighter* h)
    : HighlighterComponent(h)
{

}

void BracketMatcherComponent::setup()
{

}

bool isLeftBracket(char c) {
    return c=='(' || c=='{' || c=='[';
}

bool isRightBracket(char c) {
    return c==')' || c=='}' || c==']';
}

char opposingBracket(char c) {
    switch(c) {
    case '(': return ')';
    case '{': return '}';
    case '[': return ']';
    case ')': return '(';
    case '}': return '{';
    case ']': return '[';
    default: return '\0';
    }
}

void BracketMatcherComponent::highlightBlock(const QString& text)
{
    TextBlockData *data = static_cast<TextBlockData*>(m_highlighter->currentBlockUserData());

    if(!data)
        data = new TextBlockData;
    else
        data->brackets.clear();

    m_highlighter->setCurrentBlockUserData(data);
}

void BracketMatcherComponent::highlightSection(QStringRef text)
{
    TextBlockData *data = new TextBlockData;
    const int textSize = text.size();

    for (int i=0; i<textSize; ++i) {
        const char c = text.at(i).toLatin1();
        if(isLeftBracket(c) || isRightBracket(c)) {
            BracketInfo* info = new BracketInfo;
            info->character = c;
            info->position = text.position() + i;
            data->brackets.push_back(info);
        }
    }

    m_highlighter->setCurrentBlockUserData(data);
}

int findLeftBracket(char orig, char other, QTextBlock currentBlock, int pos, int depth)
{
    TextBlockData *data = static_cast<TextBlockData *>(currentBlock.userData());
    QVector<BracketInfo*> infos = data->brackets;

    auto it = infos.rbegin();
    while(it != infos.rend() && pos-->0)
        it++;

    for (; it != infos.rend(); ++it) {
        BracketInfo* pi = *it;

        if(pi->character == orig)
            depth++;
        else if(pi->character == other) {
            if(depth == 0)
                return currentBlock.position() + pi->position;
            depth--;
        }
    }

    currentBlock = currentBlock.previous();
    if(currentBlock.isValid())
        return findLeftBracket(orig, other, currentBlock, 0, depth);
    else
        return -1;
}

int findRightBracket(char orig, char other, QTextBlock currentBlock, int pos, int depth)
{
    TextBlockData *data = static_cast<TextBlockData *>(currentBlock.userData());
    QVector<BracketInfo*> infos = data->brackets;

    for (auto it = infos.begin()+pos; it != infos.end(); ++it) {
        BracketInfo* pi = *it;

        if(pi->character == orig)
            depth++;
        else if(pi->character == other) {
            if(depth == 0)
                return currentBlock.position() + pi->position;
            depth--;
        }
    }

    currentBlock = currentBlock.next();
    if(currentBlock.isValid())
        return findRightBracket(orig, other, currentBlock, 0, depth);
    else
        return -1;
}

QPair<int,int> BracketMatcherComponent::findMatchingBracketPosition(QTextCursor cursor) const
{
    typedef QPair<int,int> IntPair;

    TextBlockData* data = static_cast<TextBlockData *>(cursor.block().userData());

    if(!data)
        return IntPair(-1,-1);

    const QVector<BracketInfo*> infos = data->brackets;
    const int cursorPos = cursor.position();
    const int blockPos = cursor.block().position();

    for (int i = 0; i < infos.size(); ++i) {
        const BracketInfo* info = infos.at(i);
        const int relativePos = cursorPos - blockPos;

        if(info->position > relativePos)
            break;

        if(info->position != relativePos-1 && info->position != relativePos)
            continue;

        const char c = info->character;

        QPair<int,int> retVal = {blockPos + info->position,-1};

        if (isLeftBracket(c))
            retVal.second = findRightBracket(c, opposingBracket(c), cursor.block(), i+1, 0);
        else if(isRightBracket(c))
            retVal.second = findLeftBracket(c, opposingBracket(c), cursor.block(), infos.size()-i, 0);
        else
            return IntPair(-1,-1);

        return retVal;
    }

    return IntPair(-1,-1);
}

} // namespace ote
