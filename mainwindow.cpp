#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>

#include <QLabel>

#include "ote/Theming/themedata.h"
#include "ote/Theming/themedatabase.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	using namespace ote;

	ui->setupUi(this);

	auto p = qApp->palette();

	ThemeData* td = new ThemeData("dark");

	td->set(Theme::TextEditText, QColor(0x50fa7b));
	td->set(Theme::TextEditActiveText, QColor(0xf8f8f2));
	td->set(Theme::TextEditBackground, QColor(0x282a36));
	td->set(Theme::TextEditActiveBackground, QColor(255,255,255,25));

	td->set(Theme::GutterText, QColor(0x6D8A88));
	td->set(Theme::GutterActiveText, QColor(255,255,255));
	td->set(Theme::GutterBackground, QColor(0x282a36));
	td->set(Theme::GutterActiveBackground, QColor(50,50,50));

	td->set(Theme::SyntaxComment, QColor(0x6272a4));
	td->set(Theme::SyntaxString, QColor(0xf1fa8c));
	td->set(Theme::SyntaxNumber, QColor(0xbd93f9));
	td->set(Theme::SyntaxConstant, QColor(0xbd93f9));
	td->set(Theme::SyntaxType, QColor(0xffb86c));
	td->set(Theme::SyntaxKeyword, QColor(0xff79c6));
	td->set(Theme::SyntaxSymbol, QColor(0xffffff));

	QTextCharFormat mbFormat;
	mbFormat.setBackground( QColor(255,255,255,25) );
	mbFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
	td->set(Theme::SyntaxMatchingBracket, mbFormat);

	td->set(Theme::SyntaxOperator, QColor(0xff79c6));
	td->set(Theme::SearchHighlight, QColor(100,255,100, 50));

	ThemeDatabase::addTheme(td);




	ThemeData* t2 = new ThemeData("light");
	t2->set(Theme::TextEditActiveBackground, QColor(50,50,50,30));
	t2->set(Theme::GutterBackground, QColor(80,80,100));
	t2->set(Theme::GutterActiveBackground, QColor(120,90,120));
	t2->set(Theme::GutterActiveText, QColor(255,255,255));
	t2->set(Theme::GutterText, QColor(205,205,205));
	t2->set(Theme::SearchHighlight, QColor(100,100,255));

	t2->set(Theme::TextEditBackground, QColor(205,205,205));
	t2->set(Theme::TextEditText, QColor(0,0,0));
	t2->set(Theme::TextEditActiveText, QColor(150,190,185));
	ThemeDatabase::addTheme(t2);



	ThemeData* t3 = new ThemeData("");
	t3->set(Theme::TextEditActiveBackground, p.highlight().color());
	t3->set(Theme::GutterBackground, p.base().color());
	t3->set(Theme::GutterActiveBackground, p.highlight().color());
	t3->set(Theme::GutterActiveText, p.highlightedText().color());
	t3->set(Theme::GutterText, p.text().color());
	t3->set(Theme::SearchHighlight, p.highlight().color());
	t3->set(Theme::TextEditBackground, p.base().color());
	t3->set(Theme::TextEditText, p.text().color());
	t3->set(Theme::TextEditActiveText, p.highlightedText().color());
	ThemeDatabase::addTheme(t3);


	ui->plainTextEdit->setTheme(Theme("dark"));


	QLabel* l = new QLabel("Document unmodified");

	connect(ui->plainTextEdit, &ote::TextEdit::modificationChanged, [l](bool modified){
		modified ?
					l->setText("Document modified") :
					l->setText("Document unmodified");
	});

	ui->statusBar->addWidget(l);


	QFile f("/home/s3rius/dev/src/ktexteditor/src/syntax/katehighlight.cpp");
	f.open(QFile::ReadOnly);

	auto b = f.readAll();

	ui->plainTextEdit->setPlainText(b);
	//ui->plainTextEdit->setTextCursor( ui->plainTextEdit->cursorForPosition(QPoint(0,0)) );

	ui->plainTextEdit->setCursorPosition(20, 30);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionSelect_Light_Theme_triggered()
{
	ui->plainTextEdit->setTheme( ote::ThemeDatabase::getTheme("light") );
}

void MainWindow::on_actionSelect_Dark_Theme_triggered()
{
	ui->plainTextEdit->setTheme( ote::ThemeDatabase::getTheme("dark") );
}

void MainWindow::on_actionShow_Whitespace_toggled(bool arg1)
{
	ui->plainTextEdit->setShowWhitespace(arg1);
}

void MainWindow::on_actionShow_Linebreaks_toggled(bool arg1)
{
	ui->plainTextEdit->setShowLinebreaks(arg1);
}

void MainWindow::on_actionEnable_Smart_Indent_toggled(bool arg1)
{
	ui->plainTextEdit->setSmartIndent(arg1);
}

void MainWindow::on_actionEnable_Line_Wrap_toggled(bool arg1)
{
	ui->plainTextEdit->setWordWrap(arg1);
}

void MainWindow::on_actionAuto_convert_tab_to_spaces_toggled(bool arg1)
{
	ui->plainTextEdit->setTabToSpaces(arg1);
}

void MainWindow::on_actionReset_Zoom_triggered()
{
	ui->plainTextEdit->resetZoom();
}

void MainWindow::on_actionZoom_In_triggered()
{
	ui->plainTextEdit->zoomIn();
}

void MainWindow::on_actionZoom_Out_triggered()
{
	ui->plainTextEdit->zoomOut();
}

void MainWindow::on_actionSet_Document_Unmodified_triggered()
{
	ui->plainTextEdit->document()->setModified(false);
}
