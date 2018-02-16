#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <vector>

#include <QPlainTextEdit>

#include "Highlighting/compositehighlighter.h"
#include "Themes/theme.h"
#include "Syntax/syntaxdefinition.h"

namespace ote {

class TextEditGutter;

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget* parent);

    void setTheme(Theme t);
    Theme getTheme() const { return m_currentTheme; }

    void setSyntaxDefnition(SyntaxDefinition d);
    SyntaxDefinition getSyntaxDefinition() const { return m_currentSyntaxDefinition; }

    void setEndOfLineMarkersVisible(bool enable);
    void setWhitespaceVisible(bool show);
    void setShowLinebreaks(bool show);
    void setSmartIndent(bool enable);
    void setTabToSpaces(bool enable);
    void setWordWrap(bool enable);
    void setTabWidth(int tabWidth);
    void setFont(QFont font);

    bool isTabToSpaces() const;
    int getTabWidth() const;

    QString getCurrentWord() const;
    int getLineCount() const;

    // Cursor Positioning
    // Note that these functions take input starting from 0. First line, first character is {0,0}
    struct CursorPos {
        int line = 0;
        int column = 0;
    };

    struct Selection {
        CursorPos start;
        CursorPos end;
    };

    void setCursorPosition(int line, int column);
    void setCursorPosition(const CursorPos& pos);
    CursorPos getCursorPosition() const;

    void setAbsoluteCursorPosition(int pos);
    int getAbsoluteCursorPosition() const;

    // Selection
    QString getSelectedText() const;
    Selection getSelection() const;
    void setSelection(const Selection& sel);
    void setTextInSelection(const QString& text);

    // Scrolling
    QPoint getScrollPosition() const;
    void setScrollPosition(const QPoint& p);

    // Finding and replacing
    void startFind(QString term, int from=0, int to=-1);
    bool findNext();
    bool replaceAndNext(const QString& replacement);
    void setFindRange(int from=0, int to=-1);
    void setFindTerm(QString term, bool tentativeSearch=true);
    void setFindFlags(QTextDocument::FindFlags flags);
    void setFindFlag(QTextDocument::FindFlag flag, bool set);
    QTextDocument::FindFlags getFindFlags() const;
    void endFind();

    // Zoom
    void resetZoom();
    void setZoomTo(int value);
    void zoomIn();
    void zoomOut();
    int getZoomLevel() const;

    // Modification status
    void clearHistory();
    int getModificationRevision() const;
    bool isModified() const;
    void setModified(bool modified);

    // Text editing
    void moveSelectedBlocksUp();
    void moveSelectedBlocksDown();
    void duplicateSelectedBlocks();
    void deleteSelectedBlocks();

    // Whitespace management
    void convertLeadingWhitespaceToTabs();
    void convertLeadingWhitespaceToSpaces();
    void trimWhitespace(bool leading, bool trailing);

signals:
    void cursorPositionChanged();
    void contentsChanged();
    void mouseWheelUsed(QWheelEvent *ev);
    void gotFocus();
    void urlsDropped(const QList<QUrl>& urls);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent* event) override;
    void dropEvent(QDropEvent *event) override;

    void focusInEvent(QFocusEvent* event) override;
    void paintEvent(QPaintEvent* e) override;

    void contextMenuEvent(QContextMenuEvent* event) override;
private:
    friend class TextEditGutter;

    QTextBlock blockAtPosition(int y) const;
    void updateSidebarGeometry();
    void updateSidebarArea(const QRect& rect, int dy);

    void onCursorPositionChanged();
    void onSelectionChanged();

private:
    struct BlockData {
        QTextBlock block;
        QRect translatedRect;
    };

    enum ESType {
        ESLineHighlight = 0,
        ESSameItems,
        ESMatchingBrackets,
    };

    typedef std::vector<BlockData> BlockList;
    typedef QList<QTextEdit::ExtraSelection> ExtraSelectionList;
    typedef QMap<ESType, ExtraSelectionList> ExtraSelectionMap;

    mutable int m_blockListCounter = 0;

    BlockList getBlocksInViewport() const;
    BlockList getBlocksInRect(QRect rect) const;

    int cursorPosToAbsolutePos(const CursorPos& pos) const;

    void resizeEvent(QResizeEvent* event) override;
    QTextBlock findClosingBlock(const QTextBlock& startBlock) const;
    void paintLineBreaks(QPainter& painter, const BlockList& blockList) const;
    void paintSearchBlock(QPainter& painter, const QRect& eventRect, const QTextBlock& block);
    void paintEndOfLineMarkers(QPainter& painter, const BlockList& blockList) const;
    void compositeExtraSelections();

    bool m_showEndOfLineMarkers = false;
    bool m_showLinebreaks = false;
    bool m_smartIndent = false;
    bool m_tabToSpaces = false;

    bool m_findActive = false;
    bool m_findTermSelected = false;
    QString m_findTerm;
    QPair<int,int> m_findRange;
    QTextDocument::FindFlags m_findFlags;

    int m_tabWidth = 4;
    int m_pointZoom = 0;

    ExtraSelectionMap m_extraSelections;

    CompositeHighlighter* m_highlighter;
    TextEditGutter* m_sideBar;

    Theme m_currentTheme;
    SyntaxDefinition m_currentSyntaxDefinition;

    void createParenthesisSelection(int pos);

public:
    bool isFoldable(const QTextBlock& block) const;
    bool isFolded(const QTextBlock& block) const;
    void toggleFold(const QTextBlock& startBlock);
};

} // namespace ote

#endif // TEXTEDITOR_H
