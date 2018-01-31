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

	void setShowWhitespace(bool show);
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
		CursorPos begin;
		CursorPos end;
	};

	void setCursorPosition(int line, int column);
	void setCursorPosition(const CursorPos& pos);
	CursorPos getCursorPosition() const;

	void setAbsoluteCursorPosition(int pos);
	int getAbsoluteCursorPosition() const;

	// Scrolling
	QPoint getScrollPosition() const;
	void setScrollPosition(const QPoint& p);

	// Finding
	void startFind(QString term, QTextDocument::FindFlags flags, int from=0, int to=-1);
	Selection findNext();
	void setFindFlags(QTextDocument::FindFlags);
	QTextDocument::FindFlags getFindFlags() const;
	void endFind();

	// Zoom
	void resetZoom();
	void setZoomTo(int value);
	void zoomIn();
	void zoomOut();
	int getZoomLevel() const;

	// Modification status
	int getModificationRevision() const;
	bool isModified() const;
	void setModified(bool modified);

	// Text editing
	void moveSelectedBlocksUp();
	void moveSelectedBlocksDown();
	void duplicateSelectedBlocks();
	void deleteSelectedBlocks();

	void convertLeadingWhitespaceToTabs();
	void convertLeadingWhitespaceToSpace();

signals:
	void cursorPositionChanged();
	void contentsChanged();
	void mouseWheelUsed(QWheelEvent *ev);
	void gotFocus();

protected:
	void keyPressEvent(QKeyEvent *e) override;
	void wheelEvent(QWheelEvent* event) override;

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

	void resizeEvent(QResizeEvent* event) override;
	QTextBlock findClosingBlock(const QTextBlock& startBlock) const;
	void paintLineBreaks(QPainter& painter, const BlockList& blockList) const;
	void paintSearchBlock(QPainter& painter, const QRect& eventRect, const QTextBlock& block);
	void compositeExtraSelections();

	bool m_showLinebreaks = false;
	bool m_smartIndent = false;
	bool m_tabToSpaces = false;

	bool m_findActive = false;
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
};

} // namespace ote

#endif // TEXTEDITOR_H
