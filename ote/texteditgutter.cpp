#include "texteditgutter.h"

#include <QTextBlock>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMenu>
#include <QPainter>
#include <QPalette>
#include "textedit.h"

namespace ote {

TextEditGutter::TextEditGutter(TextEdit *editor) :
	QWidget(editor),
	m_textEdit(editor)
{
	setMouseTracking(true);
}


void TextEditGutter::mouseMoveEvent(QMouseEvent* event) {
	const auto block = m_textEdit->blockAtPosition(event->y());

	if(!block.isValid() || !block.isVisible())
		return;

	const auto blockNum = block.blockNumber();
	const auto wantRepaint = (m_hoverBlockNumber != blockNum);
	m_hoverBlockNumber = blockNum;

	if(wantRepaint)
		repaint();
}


QSize TextEditGutter::sizeHint() const
{
	auto count = m_textEdit->getLineCount();
	int digits = 1;
	while (count >= 10) {
		++digits;
		count /= 10;
	}

	const auto leftMargin = 2;
	const auto widthOfChar = m_textEdit->fontMetrics().width(QLatin1Char('9'));
	const auto width = leftMargin + widthOfChar * digits + m_textEdit->fontMetrics().lineSpacing();

	return QSize(width, 0);
}

void TextEditGutter::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	const auto& blockList = m_textEdit->getBlocksInRect(event->rect());

	paintGutter(event, painter, blockList);
	paintFoldingMarks(painter, blockList);
}

void TextEditGutter::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->x() >= width() - m_textEdit->fontMetrics().lineSpacing()) {
		auto block = m_textEdit->blockAtPosition(event->y());
		if (!block.isValid() || !m_textEdit->isFoldable(block))
			return;
		m_textEdit->toggleFold(block);
	}
	QWidget::mouseReleaseEvent(event);
}

void TextEditGutter::paintFoldingMarks(QPainter& painter, const TextEdit::BlockList& blockList)
{
	//const Theme& currentTheme = m_textEdit->m_currentTheme;
	const auto foldingMarkerSize = fontMetrics().lineSpacing();

	for (const auto& blockData : blockList) {
		const auto block = blockData.block;
		const auto geom = blockData.translatedRect;

		if (!block.isVisible() || !block.text().startsWith("{"))
			continue;

		const auto blockNumber = block.blockNumber();

		QPolygonF polygon;
		polygon << QPointF(foldingMarkerSize * 0.4, foldingMarkerSize * 0.25);
		polygon << QPointF(foldingMarkerSize * 0.4, foldingMarkerSize * 0.75);
		polygon << QPointF(foldingMarkerSize * 0.8, foldingMarkerSize * 0.5);

		painter.save();
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor("red"));
		painter.translate(width() - foldingMarkerSize, geom.top());
		painter.drawPolygon(polygon);

		if(m_hoverBlockNumber == blockNumber) {
			paintFoldingRange(painter, blockData);
		}

		painter.restore();
	}
}

void TextEditGutter::paintGutter(QPaintEvent* event, QPainter& painter, const TextEdit::BlockList& blockList)
{
	const Theme& currentTheme = m_textEdit->m_currentTheme;
	const int currentBlockNumber = m_textEdit->textCursor().blockNumber();
	const auto foldingMarkerSize = fontMetrics().lineSpacing();

	//painter.fillRect(event->rect(), QColor(QColor::colorNames()[rand() % 10]));
	painter.fillRect(event->rect(), currentTheme.getColor(Theme::GutterBackground));

	for (const auto& blockData : blockList) {
		const auto block = blockData.block;
		const auto geom = blockData.translatedRect;

		if (!block.isVisible())
			continue;

		const auto blockNumber = block.blockNumber();
		const auto color = (blockNumber == currentBlockNumber) ?
							   currentTheme.getColor(Theme::GutterActiveText) :
							   currentTheme.getColor(Theme::GutterText);
		painter.setPen(color);

		painter.drawText(0, geom.top(),
						 width() - foldingMarkerSize, fontMetrics().height(),
						 Qt::AlignRight,
						 QString::number(blockNumber + 1));
	}
}

void TextEditGutter::paintFoldingRange(QPainter& painter, const TextEdit::BlockData& blockData)
{
	if(m_hoverBlockNumber == -1)
		return;

	const auto foldingMarkerSize = fontMetrics().lineSpacing();
	const auto block = blockData.block;
	const auto geom = blockData.translatedRect;
	auto endBlock = m_textEdit->findClosingBlock(block);
	int topy = m_textEdit->blockBoundingGeometry(endBlock).translated(m_textEdit->contentOffset()).top();

	if(!endBlock.isValid())
		return;

	const auto yEnd = topy - geom.top()+ foldingMarkerSize*0.5;

	painter.setPen(QColor("white"));
	painter.drawLine( QPoint(foldingMarkerSize * 0.5,foldingMarkerSize * 0.8),
					  QPoint(foldingMarkerSize * 0.5,yEnd) );

	painter.drawLine( QPoint(foldingMarkerSize * 0.5 + 1,yEnd),
					  QPoint(foldingMarkerSize * 1.0 + 1,yEnd) );

}

void TextEditGutter::leaveEvent(QEvent* /*e*/)
{
	m_hoverBlockNumber = -1;
	m_textEdit->repaint();
}

} // namespace ote
