#ifndef TEXTEDITGUTTER_H
#define TEXTEDITGUTTER_H

#include <QPlainTextEdit>
#include "Theming/theme.h"
#include "textedit.h"

namespace ote {

class TextEditGutter : public QWidget
{
	Q_OBJECT
public:
	explicit TextEditGutter(TextEdit *editor);
	QSize sizeHint() const override;

protected:
	void paintEvent(QPaintEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;


private:
	friend class TextEdit;

	void paintFoldingMarks(QPainter& painter, const TextEdit::BlockList& blockList);
	void paintGutter(QPaintEvent* event, QPainter& painter, const TextEdit::BlockList& blockList);
	void paintFoldingRange(QPainter& painter, const TextEdit::BlockData& blockData);

	void leaveEvent(QEvent* e) override;
	void mouseMoveEvent(QMouseEvent* event) override;

	TextEdit* m_textEdit;

	int m_hoverBlockNumber = -1;
};

} // namespace ote

#endif // TEXTEDITGUTTER_H
