#include "textedit.h"

#include <QTextBlock>
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QMenu>

#include <algorithm>
#include <cmath>

#include "texteditgutter.h"

#include "Highlighting/bracketmatchercomponent.h"

namespace ote {

static bool isFixedPitch(const QFont & font) {
	const QFontInfo fi(font);
	//qDebug() << fi.family() << fi.fixedPitch();
	return fi.fixedPitch();
}

static QFont getMonospaceFont(){
	QFont font("monospace");
	font.setFixedPitch(true);
	font.setStyleHint(QFont::Monospace);
	if (isFixedPitch(font)) return font;
	font.setStyleHint(QFont::TypeWriter);
	if (isFixedPitch(font)) return font;
	font.setFamily("courier");
	if (isFixedPitch(font)) return font;
	return font;
}



TextEdit::TextEdit(QWidget* parent)
	: QPlainTextEdit (parent),
	   m_sideBar(new TextEditGutter(this)),
	   m_currentTheme(Theme(""))
{
	setFont( getMonospaceFont() );

	connect(this, &QPlainTextEdit::blockCountChanged, this, &TextEdit::updateSidebarGeometry);
	connect(this, &QPlainTextEdit::updateRequest, this, &TextEdit::updateSidebarArea);
	connect(this, &QPlainTextEdit::cursorPositionChanged, this, &TextEdit::onCursorPositionChanged);
	connect(this, &QPlainTextEdit::cursorPositionChanged, this, &TextEdit::cursorPositionChanged);
	connect(this, &QPlainTextEdit::selectionChanged, this, &TextEdit::onSelectionChanged);

	setWordWrap(false);

	m_highlighter = new CompositeHighlighter();
	m_highlighter->setDocument(this->document());

	updateSidebarGeometry();
	onCursorPositionChanged();
}

void TextEdit::setTheme(Theme t) {
	m_currentTheme = t;

	auto pal = qApp->palette();

	pal.setColor(QPalette::Base, t.getColor(Theme::TextEditBackground));
	pal.setColor(QPalette::Text, t.getColor(Theme::TextEditText));
	pal.setColor(QPalette::Highlight, t.getColor(Theme::TextEditActiveBackground));
	pal.setColor(QPalette::HighlightedText, t.getColor(Theme::TextEditActiveText));

	setPalette(pal);

	m_highlighter->setTheme(t);
	m_highlighter->rehighlight();
	onCursorPositionChanged();
	onSelectionChanged();
}

void TextEdit::setSyntaxDefnition(SyntaxDefinition d)
{
	m_currentSyntaxDefinition = d;

	m_highlighter->setSyntaxDefinition(d);
	m_highlighter->rehighlight();
}

void TextEdit::setWhitespaceVisible(bool show)
{
	auto opts = document()->defaultTextOption();
	auto flags = opts.flags();

	flags.setFlag(QTextOption::ShowTabsAndSpaces, show);
	opts.setFlags(flags);
	document()->setDefaultTextOption( opts );
}

void TextEdit::setShowLinebreaks(bool show)
{
	if(show == m_showLinebreaks)
		return;

	m_showLinebreaks = show;
	update();
}

void TextEdit::setSmartIndent(bool enable)
{
	m_smartIndent = enable;
}

void TextEdit::setTabToSpaces(bool enable)
{
	m_tabToSpaces = enable;
}

void TextEdit::setWordWrap(bool enable)
{
	enable ?
		setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere) :
		setWordWrapMode(QTextOption::NoWrap);
}

void TextEdit::setTabWidth(int tabWidth)
{
	// Calculating letter width using QFrontMetrics isn't 100% accurate. Small inaccuracies
	// can accumulate over time. Instead, we can calculate a good letter spacing value and
	// make the font use it.
	// https://stackoverflow.com/a/42071875/1038629
	m_tabWidth = tabWidth;

	auto font = this->font();
	QFontMetricsF fm (font);
	auto stopWidth = tabWidth * fm.width(' ');
	auto letterSpacing = (ceil(stopWidth) - stopWidth) / tabWidth;

	font.setLetterSpacing(QFont::AbsoluteSpacing, letterSpacing);
	QPlainTextEdit::setFont(font);

	setTabStopDistance(ceil(stopWidth));
}

void TextEdit::setFont(QFont font)
{
	if(!font.fixedPitch())
		qWarning() << "Selected font is not fixed pitch.";

	QPlainTextEdit::setFont(font);
	setTabWidth(m_tabWidth);
}

bool TextEdit::isTabToSpaces() const
{
	return m_tabToSpaces;
}

int TextEdit::getTabWidth() const
{
	return m_tabWidth;
}

QString TextEdit::getCurrentWord() const
{
	auto c = textCursor();
	c.select(QTextCursor::WordUnderCursor);
	return c.selectedText();
}

int TextEdit::getLineCount() const
{
	return blockCount();
}

void TextEdit::setCursorPosition(int line, int column)
{
	setAbsoluteCursorPosition( cursorPosToAbsolutePos({line, column}) );
}

void TextEdit::setCursorPosition(const TextEdit::CursorPos& pos)
{
	setCursorPosition(pos.line, pos.column);
}

TextEdit::CursorPos TextEdit::getCursorPosition() const
{
	const auto& c = textCursor();
	return { c.blockNumber(), c.positionInBlock() };
}

void TextEdit::setAbsoluteCursorPosition(int pos)
{
	auto c = textCursor();
	c.setPosition(pos);
	setTextCursor(c);
}

int TextEdit::getAbsoluteCursorPosition() const
{
	return textCursor().position();
}

QString TextEdit::getSelectedText() const
{
	return textCursor().selectedText();
}

TextEdit::Selection TextEdit::getSelection() const
{
	Selection s;
	auto cursor = textCursor();
	auto c = cursor;
	c.setPosition(cursor.selectionStart());
	s.start = {c.blockNumber(), c.positionInBlock()};
	c.setPosition(cursor.selectionEnd());
	s.end = {c.blockNumber(), c.positionInBlock()};

	return s;
}

void TextEdit::setSelection(const TextEdit::Selection& sel)
{
	auto cur = textCursor();
	cur.setPosition(cursorPosToAbsolutePos(sel.start));
	cur.setPosition(cursorPosToAbsolutePos(sel.end), QTextCursor::KeepAnchor);
	setTextCursor(cur);
}

void TextEdit::setTextInSelection(const QString& text)
{
	textCursor().insertText(text);
}

QPoint TextEdit::getScrollPosition() const
{
	return { horizontalScrollBar()->sliderPosition(), verticalScrollBar()->sliderPosition() };
}

void TextEdit::setScrollPosition(const QPoint& p)
{
	horizontalScrollBar()->setSliderPosition(p.x());
	verticalScrollBar()->setSliderPosition(p.y());
}

void TextEdit::startFind(QString term, int from, int to)
{
	m_findRange = {from, to};

	m_findTerm = term;
	m_findFlags = QTextDocument::FindFlags();
	m_findActive = true;

	/*auto c = textCursor();
	c.setPosition(from);
	c.setPosition(to, QTextCursor::KeepAnchor);

	QTextEdit::ExtraSelection selection;
	selection.format.setBackground(m_currentTheme.getColor(Theme::TextEditActiveBackground));
	selection.cursor = c;

	m_extraSelections[ESSearchRange] = ExtraSelectionList() << selection;*/
}

void TextEdit::findNext()
{
	//if(!m_findActive)
	//	return;

	auto curr = textCursor();
	const auto startPos = m_findRange.first;
	const auto endPos = m_findRange.second>0 ? m_findRange.second : document()->characterCount()-1;


	if(curr.position() < startPos)
		curr.setPosition(startPos);
	else if(curr.position() > endPos)
		curr.setPosition(endPos);


	auto c = document()->find(m_findTerm, curr, m_findFlags);
	const bool fwd = !m_findFlags.testFlag(QTextDocument::FindBackward);

	// If c moved past the search range or failed to find a match we want to loop around and retry.
	if((fwd && c.isNull()) || c.selectionEnd() > endPos)
		c = document()->find(m_findTerm, startPos, m_findFlags);
	else if((!fwd && c.isNull()) || c.selectionStart() < startPos)
		c = document()->find(m_findTerm, endPos, m_findFlags);

	if(!c.isNull() && c.selectionEnd() <= endPos && c.selectionStart() >= startPos) {
		setTextCursor(c);
	}

	return;
}

void TextEdit::ReplaceAndNext(const QString& replacement)
{
	auto cursor = textCursor();

	if(cursor.selectedText() == m_findTerm) {
		cursor.insertText(replacement);
	}

	findNext();
}

void TextEdit::setFindRange(int from, int to)
{
	m_findRange = {from, to};
}

void TextEdit::setFindTerm(QString term, bool tentativeSearch)
{
	m_findTerm = term;

	if(tentativeSearch) {
		auto c = textCursor();
		c.setPosition(c.selectionStart());
		setTextCursor(c);
		findNext();
	}
}

void TextEdit::setFindFlags(QTextDocument::FindFlags flags)
{
	m_findFlags = flags;
}

void TextEdit::setFindFlag(QTextDocument::FindFlag flag, bool set)
{
	m_findFlags.setFlag(flag, set);
}

QTextDocument::FindFlags TextEdit::getFindFlags() const
{
	return m_findFlags;
}

void TextEdit::endFind()
{
	m_findActive = false;
}

void TextEdit::resetZoom()
{
	m_pointZoom<0 ? QPlainTextEdit::zoomIn(m_pointZoom) :
					QPlainTextEdit::zoomOut(m_pointZoom);

	m_pointZoom = 0;
}

void TextEdit::setZoomTo(int value)
{
	const auto diff = value - m_pointZoom;

	diff>0 ?	QPlainTextEdit::zoomIn(diff) :
				QPlainTextEdit::zoomOut(diff);
	m_pointZoom = value;
}

void TextEdit::zoomIn()
{
	m_pointZoom++;
	QPlainTextEdit::zoomIn();
}

void TextEdit::zoomOut()
{
	m_pointZoom--;
	QPlainTextEdit::zoomOut();
}

int TextEdit::getZoomLevel() const
{
	return m_pointZoom;
}

void TextEdit::clearHistory()
{
	document()->clearUndoRedoStacks();
}

int TextEdit::getModificationRevision() const
{
	return document()->revision();
}

bool TextEdit::isModified() const
{
	return document()->isModified();
}

void TextEdit::setModified(bool modified)
{
	document()->setModified(modified);
}

void TextEdit::moveSelectedBlocksUp()
{
	auto c = textCursor();

	bool success;
	c.setPosition(c.selectionStart()); //Remove selection
	success =  c.movePosition(QTextCursor::PreviousBlock);
	success &= c.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);

	if(!success)
		return;

	c.beginEditBlock();
	auto text = c.selectedText();
	c.removeSelectedText();

	c = textCursor();
	c.setPosition(c.selectionEnd());
	c.movePosition(QTextCursor::NextBlock);
	c.insertText(text);

	c.endEditBlock();
}

void TextEdit::moveSelectedBlocksDown()
{
	auto c = textCursor();

	bool success;
	c.setPosition(c.selectionEnd()); //Remove selection
	success =  c.movePosition(QTextCursor::NextBlock);
	success &= c.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);

	if(!success)
		return;

	c.beginEditBlock();
	auto text = c.selectedText();
	c.removeSelectedText();

	c = textCursor();
	c.setPosition(c.selectionStart());
	c.movePosition(QTextCursor::StartOfBlock);
	c.insertText(text);

	c.endEditBlock();
}

void TextEdit::duplicateSelectedBlocks()
{
	auto c = textCursor();
	auto blockCursor = c;

	bool success;
	blockCursor.setPosition(c.selectionStart());
	success = blockCursor.movePosition(QTextCursor::StartOfBlock);
	blockCursor.setPosition(c.selectionEnd(), QTextCursor::KeepAnchor);
	success &= blockCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);

	if(!success)
		return;

	c.beginEditBlock();
	auto text = blockCursor.selectedText();

	c = textCursor();
	c.setPosition(c.selectionStart());
	c.movePosition(QTextCursor::StartOfBlock);
	c.insertText(text);

	c.endEditBlock();
}

void TextEdit::deleteSelectedBlocks()
{
	auto c = textCursor();
	auto ce = c;

	ce.setPosition(c.selectionStart());
	ce.movePosition(QTextCursor::StartOfBlock);
	ce.setPosition(c.selectionEnd(), QTextCursor::KeepAnchor);
	ce.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
	ce.removeSelectedText();
}

void TextEdit::convertLeadingWhitespaceToTabs()
{
	auto lines = toPlainText().split('\n');
	QString final;

	QRegularExpression regex = QRegularExpression("\\S");

	for(auto& line : lines) {
		auto firstNonWS = line.indexOf(regex);
		auto ws = 0;
		for(int i=0; i<firstNonWS; ++i)
			ws += (line[i]=='\t' ? m_tabWidth : 1);

		QString newBegin;
		while(ws>=4) {
			newBegin += '\t';
			ws -= 4;
		}
		while(ws-->0)
			newBegin += ' ';

		line = newBegin + line.mid(firstNonWS);
		final += line + '\n';
	}

	if(!final.isEmpty())
		final.chop(1); // Remove last '\n' since it creates an extra line at the end

	auto c = textCursor();
	c.select(QTextCursor::Document);

	c.insertText(final);
}

void TextEdit::convertLeadingWhitespaceToSpaces()
{
	auto lines = toPlainText().split('\n');
	QString final;

	QRegularExpression regex = QRegularExpression("\\S");

	for(auto& line : lines) {
		auto firstNonWS = line.indexOf(regex);
		auto ws = 0;
		for(int i=0; i<firstNonWS; ++i)
			ws += (line[i]=='\t' ? m_tabWidth : 1);

		line = QString(ws, ' ') + line.mid(firstNonWS);
		final += line + '\n';
	}

	if(!final.isEmpty())
		final.chop(1); // Remove last '\n' since it creates an extra line at the end

	auto c = textCursor();
	c.select(QTextCursor::Document);

	c.insertText(final);
}

void TextEdit::trimWhitespace(bool leading, bool trailing)
{
	if(!leading && !trailing)
		return;

	auto lines = toPlainText().split('\n');
	QString final;

	QRegularExpression regex = QRegularExpression("\\S");

	for(auto& line : lines) {
		int start = leading ? line.indexOf(regex) : 0;
		int end = trailing ? line.lastIndexOf(regex) : -1;

		line = line.mid(start, end-start+1);
		final += line + '\n';
	}

	if(!final.isEmpty())
		final.chop(1); // Remove last '\n' since it creates an extra line at the end

	auto c = textCursor();
	c.select(QTextCursor::Document);

	c.insertText(final);
}

void TextEdit::updateSidebarGeometry()
{
	auto gutterWidth = m_sideBar->sizeHint().width();

	setViewportMargins(gutterWidth, 0, 0, 0);
	const auto r = contentsRect();
	m_sideBar->setGeometry(QRect(r.left(), r.top(), gutterWidth, r.height()));

	auto g = horizontalScrollBar()->geometry();
	g.setLeft(gutterWidth);
	horizontalScrollBar()->setGeometry( g );
}

void TextEdit::updateSidebarArea(const QRect& rect, int dy)
{
	if (dy)
		m_sideBar->scroll(0, dy);
	else
		m_sideBar->update(0, rect.y(), m_sideBar->width(), rect.height());
}


void TextEdit::onCursorPositionChanged()
{
	// Highlight current line
	QTextEdit::ExtraSelection selection;
	selection.format.setBackground(m_currentTheme.getColor(Theme::TextEditActiveBackground));
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = textCursor();
	selection.cursor.clearSelection();

	m_extraSelections[ESLineHighlight] = ExtraSelectionList() << selection;


	// Try to match brackets
	m_extraSelections[ESMatchingBrackets].clear();

	auto pair = m_highlighter->m_bracketMatcher->findMatchingBracketPosition(textCursor());

	if(pair.first != -1 && pair.second != -1) {
		createParenthesisSelection(pair.first);
		createParenthesisSelection(pair.second);
	}
}

void TextEdit::onSelectionChanged()
{
	const auto& cursor = textCursor();
	const auto& text = cursor.selectedText();

	if(text.length() < 2 || text.trimmed().isEmpty()) {
		m_extraSelections[ESSameItems].clear();
		return;
	}

	const auto& fullText = toPlainText();
	int j = 0;

	ExtraSelectionList list;
	QTextEdit::ExtraSelection selection;
	selection.format.setBackground(m_currentTheme.getColor(Theme::SearchHighlight));

	while ((j = fullText.indexOf(text, j)) != -1) {
		selection.cursor = cursor;
		selection.cursor.setPosition(j);
		selection.cursor.setPosition(j+text.length(), QTextCursor::KeepAnchor);

		list.append(selection);
		++j;
	}

	m_extraSelections[ESSameItems] = list;
}


void TextEdit::createParenthesisSelection(int pos)
{
	QTextCursor cursor = textCursor();
	cursor.setPosition(pos);
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

	m_extraSelections[ESMatchingBrackets]
			<< QTextEdit::ExtraSelection { cursor, m_currentTheme.getFormat(Theme::SyntaxMatchingBracket) };
}


void TextEdit::keyPressEvent(QKeyEvent* e)
{

	if(e->key() == Qt::Key_Tab && m_tabToSpaces) {
		auto cursor = textCursor();
		auto numSpaces = m_tabWidth - cursor.positionInBlock() % m_tabWidth;
		if(numSpaces==0) numSpaces = m_tabWidth;
		cursor.insertText(QString(numSpaces, ' '));
		return;
	}

	if(e->key() == Qt::Key_Return && m_smartIndent) {
		auto cursor = textCursor();
		cursor.beginEditBlock();
		QPlainTextEdit::keyPressEvent(e);
		auto txt = textCursor().block().previous().text();

		int txtPos = 0;
		while(txt[txtPos] == ' ' || txt[txtPos] == '\t')
			++txtPos;

		textCursor().insertText( txt.mid(0,txtPos) );
		cursor.endEditBlock();
		return;
	}

	if(e->key() == Qt::Key_Backspace && m_tabToSpaces) {
		auto txt = textCursor().block().text();

		if(txt.endsWith(QString(m_tabWidth, ' ')) && txt.length()%m_tabWidth==0) {
			auto c = textCursor();
			c.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, m_tabWidth);
			c.removeSelectedText();
			return;
		}
	}

	QPlainTextEdit::keyPressEvent(e);

	if(e->key() == Qt::Key_Insert)
		setOverwriteMode(!overwriteMode());
}

void TextEdit::wheelEvent(QWheelEvent* event)
{
	emit mouseWheelUsed(event);
	QPlainTextEdit::wheelEvent(event);
}

void TextEdit::focusInEvent(QFocusEvent* event)
{
	emit gotFocus();
	QPlainTextEdit::focusInEvent(event);
}

void TextEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QPlainTextEdit::contextMenuEvent(event);
	/*auto menu = createStandardContextMenu(event->pos());
	menu->exec(event->globalPos());
	delete menu;*/
}


void TextEdit::paintLineBreaks(QPainter &painter, const BlockList& blockList) const
{
	if(!m_showLinebreaks)
		return;

	const QChar visualArrow = ushort(0x21b5);
	const auto arrowWidth = fontMetrics().width(visualArrow);

	painter.save();
	painter.setPen(m_currentTheme.getColor(Theme::TextEditText));

	for (const auto& blockData : blockList) {
		const auto block = blockData.block;
		const auto geom = blockData.translatedRect;

		const QTextLayout* layout = block.layout();
		const int lineCount = layout->lineCount();
		const int arrowX = geom.width() - contentsMargins().right() - arrowWidth;

		if(lineCount <= 1)
			continue;

		for (int i = 0; i < lineCount-1; ++i) {
			const QTextLine line = layout->lineAt(i);
			const QRectF lineRect = line.naturalTextRect().
									translated(contentOffset().x(), geom.top());

			painter.drawText(QPointF(arrowX, lineRect.top() + line.ascent()),
							 visualArrow);
		}
	}

	painter.restore();
}

void TextEdit::paintSearchBlock(QPainter& painter, const QRect& eventRect, const QTextBlock& block)
{
return;
	QTextLayout *layout = block.layout();
	QString text = block.text();

	QTextLine line = layout->lineForTextPosition(0);

	QRectF rr = line.naturalTextRect();
	QRectF r = blockBoundingGeometry(block).translated(contentOffset());

	rr.moveTop(rr.top() + r.top());

	painter.fillRect(rr, QColor("#5000FF00"));

	QColor lineCol = QColor("red");
	QPen pen = painter.pen();
	painter.setPen(lineCol);
	//if (block == d->m_findScopeStart.block())
		painter.drawLine(rr.topLeft(), rr.topRight());
	//if (block == d->m_findScopeEnd.block())
		painter.drawLine(rr.bottomLeft(), rr.bottomRight());
	painter.drawLine(rr.topLeft(), rr.bottomLeft());
	painter.drawLine(rr.topRight(), rr.bottomRight());
	painter.setPen(pen);



	/*qreal spacew = QFontMetricsF(font()).width(QLatin1Char(' '));

	int offset = 0;
	int relativePos  =  ts.positionAtColumn(text,
											d->m_findScopeVerticalBlockSelectionFirstColumn,
											&offset);

	qreal x = line.cursorToX(relativePos) + offset * spacew;

	int eoffset = 0;
	int erelativePos  =  ts.positionAtColumn(text,
											 d->m_findScopeVerticalBlockSelectionLastColumn,
											 &eoffset);
	QTextLine eline = layout->lineForTextPosition(erelativePos);
	qreal ex = eline.cursorToX(erelativePos) + eoffset * spacew;


	rr.moveTop(rr.top() + r.top());
	rr.setLeft(r.left() + x);
	if (line.lineNumber() == eline.lineNumber())
		rr.setRight(r.left() + ex);


	QColor lineCol = QColor("red");
	QPen pen = painter.pen();
	painter.setPen(lineCol);
	if (blockFS == d->m_findScopeStart.block())
		painter.drawLine(rr.topLeft(), rr.topRight());
	if (blockFS == d->m_findScopeEnd.block())
		painter.drawLine(rr.bottomLeft(), rr.bottomRight());
	painter.drawLine(rr.topLeft(), rr.bottomLeft());
	painter.drawLine(rr.topRight(), rr.bottomRight());
	painter.setPen(pen);
*/


/*	auto er = eventRect;

	if (!m_findActive) {
		QTextBlock blockFS = block;
		QPointF offsetFS = contentOffset();
		while (blockFS.isValid()) {

			QRectF r = blockBoundingRect(blockFS).translated(offsetFS);

			if (r.bottom() >= er.top() && r.top() <= er.bottom()) {

				if (blockFS.position() >= d->m_findScopeStart.block().position()
						&& blockFS.position() <= d->m_findScopeEnd.block().position()) {
					QTextLayout *layout = blockFS.layout();
					QString text = blockFS.text();
					const TabSettings &ts = d->m_document->tabSettings();
					qreal spacew = QFontMetricsF(font()).width(QLatin1Char(' '));

					int offset = 0;
					int relativePos  =  ts.positionAtColumn(text,
															d->m_findScopeVerticalBlockSelectionFirstColumn,
															&offset);
					QTextLine line = layout->lineForTextPosition(relativePos);
					qreal x = line.cursorToX(relativePos) + offset * spacew;

					int eoffset = 0;
					int erelativePos  =  ts.positionAtColumn(text,
															 d->m_findScopeVerticalBlockSelectionLastColumn,
															 &eoffset);
					QTextLine eline = layout->lineForTextPosition(erelativePos);
					qreal ex = eline.cursorToX(erelativePos) + eoffset * spacew;

					QRectF rr = line.naturalTextRect();
					rr.moveTop(rr.top() + r.top());
					rr.setLeft(r.left() + x);
					if (line.lineNumber() == eline.lineNumber())
						rr.setRight(r.left() + ex);
					painter.fillRect(rr, QColor("yellow"));

					QColor lineCol = QColor("red");
					QPen pen = painter.pen();
					painter.setPen(lineCol);
					if (blockFS == d->m_findScopeStart.block())
						painter.drawLine(rr.topLeft(), rr.topRight());
					if (blockFS == d->m_findScopeEnd.block())
						painter.drawLine(rr.bottomLeft(), rr.bottomRight());
					painter.drawLine(rr.topLeft(), rr.bottomLeft());
					painter.drawLine(rr.topRight(), rr.bottomRight());
					painter.setPen(pen);
				}
			}
			offsetFS.ry() += r.height();

			if (offsetFS.y() > viewport()->rect().height())
				break;

			blockFS = blockFS.next();
			if (!blockFS.isVisible()) {
				// invisible blocks do have zero line count
				blockFS = document()->findBlockByLineNumber(blockFS.firstLineNumber());
			}

		}
	}*/
}

void TextEdit::compositeExtraSelections()
{
	ExtraSelectionList fullList;

	for(const auto& list : m_extraSelections)
		fullList << list;

	setExtraSelections(fullList);
	//setExtraSelections( m_extraSelections[ESSearchRange] );
}

static void fillBackground(QPainter *p, const QRectF &rect, QBrush brush, const QRectF &gradientRect = QRectF())
{
	p->save();
	if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
		if (!gradientRect.isNull()) {
			QTransform m = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
			m.scale(gradientRect.width(), gradientRect.height());
			brush.setTransform(m);
			const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
		}
	} else {
		p->setBrushOrigin(rect.topLeft());
	}
	p->fillRect(rect, brush);
	p->restore();
}

void TextEdit::paintEvent(QPaintEvent* e)
{
	compositeExtraSelections();


	// What follows is a copy of QPlainTextEdit::paintEvent with some modifications to allow different line-highlighting.
	QPainter painter(viewport());
	Q_ASSERT(qobject_cast<QPlainTextDocumentLayout*>(document()->documentLayout()));

	QPointF offset(contentOffset());

	QRect er = e->rect();
	QRect viewportRect = viewport()->rect();

	bool editable = !isReadOnly();

	QTextBlock block = firstVisibleBlock();
	qreal maximumWidth = document()->documentLayout()->documentSize().width();

	// Set a brush origin so that the WaveUnderline knows where the wave started
	painter.setBrushOrigin(offset);

	// keep right margin clean from full-width selection
	int maxX = offset.x() + qMax((qreal)viewportRect.width(), maximumWidth)
			   - document()->documentMargin();
	er.setRight(qMin(er.right(), maxX));
	painter.setClipRect(er);

	QAbstractTextDocumentLayout::PaintContext context = getPaintContext();

	/*int from = m_findRange.first;
	int to = m_findRange.second;
	auto c = textCursor();
	c.setPosition(from);
	//c.setPosition(to, QTextCursor::KeepAnchor);
	c.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 10);

	QAbstractTextDocumentLayout::Selection selection;
	selection.cursor = c;

	selection.format.setBackground(m_currentTheme.getColor(Theme::SearchHighlight));
	context.selections.push_front(selection);*/


	while (block.isValid()) {

		QRectF r = blockBoundingRect(block).translated(offset);
		QTextLayout *layout = block.layout();

		if (!block.isVisible()) {
			offset.ry() += r.height();
			block = block.next();
			continue;
		}

		if (r.bottom() >= er.top() && r.top() <= er.bottom()) {

			QTextBlockFormat blockFormat = block.blockFormat();

			QBrush bg = blockFormat.background();
			if (bg != Qt::NoBrush) {
				QRectF contentsRect = r;
				contentsRect.setWidth(qMax(r.width(), maximumWidth));
				fillBackground(&painter, contentsRect, bg);
			}

			// Drop the last Selection if the user has selected text. This way we prevent Qt painting all
			// selected text white. We have to add our own Selection to still have proper selection highlighting.
			if(context.selections.size() > 0 && textCursor().hasSelection()) {

				context.selections.pop_back();

				QAbstractTextDocumentLayout::Selection selection;
				selection.cursor = textCursor();

				//QPalette::ColorGroup cg = d->hasFocus ? QPalette::Active : QPalette::Inactive;
				selection.format.setBackground(m_currentTheme.getColor(Theme::TextEditActiveBackground));
				selection.format.setProperty(QTextFormat::FullWidthSelection, true);
				context.selections.push_back(selection);
			}

			QVector<QTextLayout::FormatRange> selections;
			int blpos = block.position();
			int bllen = block.length();
			for (int i = 0; i < context.selections.size(); ++i) {
				const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
				const int selStart = range.cursor.selectionStart() - blpos;
				const int selEnd = range.cursor.selectionEnd() - blpos;
				if (selStart < bllen && selEnd > 0
					&& selEnd > selStart) {
					QTextLayout::FormatRange o;
					o.start = selStart;
					o.length = selEnd - selStart;
					o.format = range.format;
					selections.append(o);
				} else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
						   && block.contains(range.cursor.position())) {
					// for full width selections we don't require an actual selection, just
					// a position to specify the line. that's more convenience in usage.
					QTextLayout::FormatRange o;
					QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
					o.start = l.textStart();
					o.length = l.textLength();
					if (o.start + o.length == bllen - 1)
						++o.length; // include newline
					o.format = range.format;
					selections.append(o);
				}
			}



			bool drawCursor = ((editable || (textInteractionFlags() & Qt::TextSelectableByKeyboard))
							   && context.cursorPosition >= blpos
							   && context.cursorPosition < blpos + bllen);

			bool drawCursorAsBlock = drawCursor && overwriteMode() ;

			if (drawCursorAsBlock) {
				if (context.cursorPosition == blpos + bllen - 1) {
					drawCursorAsBlock = false;
				} else {
					QTextLayout::FormatRange o;
					o.start = context.cursorPosition - blpos;
					o.length = 1;
					o.format.setForeground(palette().base());
					o.format.setBackground(palette().text());
					selections.append(o);
				}
			}

			if (!placeholderText().isEmpty() && document()->isEmpty() && layout->preeditAreaText().isEmpty()) {
				QColor col = palette().text().color();
				col.setAlpha(128);
				painter.setPen(col);
				const int margin = int(document()->documentMargin());
				painter.drawText(r.adjusted(margin, 0, 0, 0), Qt::AlignTop | Qt::TextWordWrap, placeholderText());
			} else {
				layout->draw(&painter, offset, selections, er);
			}
			if ((drawCursor && !drawCursorAsBlock)
				|| (editable && context.cursorPosition < -1
					&& !layout->preeditAreaText().isEmpty())) {
				int cpos = context.cursorPosition;
				if (cpos < -1)
					cpos = layout->preeditAreaPosition() - (cpos + 2);
				else
					cpos -= blpos;
				layout->drawCursor(&painter, offset, cpos, cursorWidth());
			}
		}

		paintSearchBlock(painter, e->rect(), block);

		offset.ry() += r.height();
		if (offset.y() > viewportRect.height())
			break;
		block = block.next();
	}

	if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
		&& (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
		painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().background());
	}

	auto bl = getBlocksInRect(e->rect());
	paintLineBreaks(painter, bl);
}

TextEdit::BlockList TextEdit::getBlocksInViewport() const
{
	return getBlocksInRect(viewport()->rect());
}

TextEdit::BlockList TextEdit::getBlocksInRect(QRect rect) const
{
	BlockList bl;
	auto block = firstVisibleBlock();
	auto contentOff = contentOffset();

	while (block.isValid()) {
		const auto geom = blockBoundingGeometry(block).translated(contentOff).toRect();

		if (geom.bottom() >= rect.top() ) {
			bl.push_back( BlockData{block, geom} );

			if(geom.top() > rect.bottom())
				break;
		}

		block = block.next();
	}

	m_blockListCounter+=bl.size();
	//qDebug() << m_blockListCounter << bl.size(); // << rect << contentOff << viewport()->height();
	return bl;
}

int TextEdit::cursorPosToAbsolutePos(const TextEdit::CursorPos& pos) const
{
	const auto& block = document()->findBlockByLineNumber(pos.line);
	const auto col = std::max(0, std::min(pos.column, block.length()-1));
	return block.position() + col;
}

QTextBlock TextEdit::blockAtPosition(int y) const
{
	auto block = firstVisibleBlock();
	if (!block.isValid())
		return QTextBlock();

	const auto geom = blockBoundingGeometry(block).translated(contentOffset()).toRect();
	int top = geom.top();
	int bottom = top + geom.height();

	do {
		if (top <= y && y <= bottom)
			return block;
		block = block.next();
		top = bottom;
		bottom = top + blockBoundingRect(block).height();
	} while (block.isValid());
	return QTextBlock();
}

void TextEdit::resizeEvent(QResizeEvent *event)
{
	QPlainTextEdit::resizeEvent(event);
	updateSidebarGeometry();
}

QTextBlock TextEdit::findClosingBlock(const QTextBlock& startBlock) const {
	int depth = 1;
	auto currBlock = startBlock.next();

	while(currBlock.isValid()) {
		if(currBlock.text().startsWith("}"))
			depth--;
		else if(currBlock.text().startsWith("{"))
			depth++;

		if(depth==0)
			return currBlock;

		currBlock = currBlock.next();
	}

	return QTextBlock();
}

} // namespace ote
