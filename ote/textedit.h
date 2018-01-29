#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <vector>

#include <QPlainTextEdit>

#include "Highlighting/compositehighlighter.h"
#include "Theming/theme.h"

namespace ote {

class TextEditGutter;

class TextEdit : public QPlainTextEdit
{
	Q_OBJECT

public:
	TextEdit(QWidget* parent);

	void setTheme(Theme t);

	void setShowWhitespace(bool show);
	void setShowLinebreaks(bool show);
	void setSmartIndent(bool enable);
	void setTabToSpaces(bool enable);
	void setWordWrap(bool enable);
	void setTabWidth(int tabWidth);
	void setFont(QFont font);

	QString getCurrentWord() const;
	int getLineCount() const;

	void setCursorPosition(int line, int column);

	void resetZoom();
	void setZoomTo(int value);
	void zoomIn();
	void zoomOut();

	int getZoomLevel() const;

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
		ESSameItems = 1,
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
	void compositeExtraSelections();

	bool m_showLinebreaks = false;
	bool m_smartIndent = false;
	bool m_tabToSpaces = false;

	int m_tabWidth = 4;
	int m_pointZoom = 0;

	ExtraSelectionMap m_extraSelections;

	CompositeHighlighter* m_highlighter;
	TextEditGutter* m_sideBar;
	Theme m_currentTheme;

	void createParenthesisSelection(int pos);
};

} // namespace ote

#endif // TEXTEDITOR_H
