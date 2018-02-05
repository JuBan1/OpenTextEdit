#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>

#include <QLabel>

#include "ote/Themes/themedatabase.h"
#include "ote/Syntax/syntaxdefinitiondatabase.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	using namespace ote;

	ui->setupUi(this);

	/*
	auto p = qApp->palette();
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
	*/


	ote::ThemeDatabase::loadFromDir("/home/s3rius/dev/qt/OpenTextEdit/themes");
	ote::SyntaxDefinitionDatabase::loadFromDir("/home/s3rius/dev/qt/OpenTextEdit/syntax");


	auto list = ote::SyntaxDefinitionDatabase::getAllDefinitions();

	for(auto s : list) {
		QString name = s.getName();
		QAction* action = new QAction(s.getName());
		connect(action, &QAction::triggered, [this, name](){
			ui->plainTextEdit->setSyntaxDefnition(ote::SyntaxDefinition(name));
		});

		ui->menuTheming->addAction(action);
	}


	ui->plainTextEdit->setTheme(Theme("dracula"));
	ui->plainTextEdit->setSyntaxDefnition(SyntaxDefinition("C++"));


	QLabel* l = new QLabel("Document unmodified");

	connect(ui->plainTextEdit, &ote::TextEdit::modificationChanged, [l, this](bool modified){
		modified ?
					l->setText("Document modified") :
					l->setText("Document unmodified");

			qDebug() << ui->plainTextEdit->getScrollPosition();
	});

	ui->statusBar->addWidget(l);


	QTimer* t = new QTimer();
	t->setInterval(2000);
	t->setSingleShot(false);


	QFile f("/home/s3rius/dev/src/ktexteditor/src/syntax/katehighlight.cpp");
	f.open(QFile::ReadOnly);

	auto b = f.readAll();

	ui->plainTextEdit->setPlainText(b);
	//ui->plainTextEdit->setTextCursor( ui->plainTextEdit->cursorForPosition(QPoint(0,0)) );


	ui->plainTextEdit->startFind("bool", 1000, 17500);
	connect(t, &QTimer::timeout, [this](){
		ui->plainTextEdit->ReplaceAndNext("BLAH");
	});
	//t->start();


	connect(ui->plainTextEdit, &ote::TextEdit::selectionChanged, [this](){
		auto s = ui->plainTextEdit->getSelection();
		qDebug() << s.start.line << s.start.column << s.end.line << s.end.column;
	});

	//ui->plainTextEdit->setCursorPosition(2000, 2);
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
	ui->plainTextEdit->setTheme( ote::ThemeDatabase::getTheme("dracula") );
}

void MainWindow::on_actionShow_Whitespace_toggled(bool arg1)
{
	ui->plainTextEdit->setWhitespaceVisible(arg1);
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

void MainWindow::on_actionReload_Theme_Files_triggered()
{
	ote::ThemeDatabase::loadFromDir("/home/s3rius/dev/qt/OpenTextEdit/themes");
	ui->plainTextEdit->setTheme(ui->plainTextEdit->getTheme());
}

void MainWindow::on_actionMove_Selection_Up_triggered()
{
	ui->plainTextEdit->moveSelectedBlocksUp();
}

void MainWindow::on_actionMove_Selection_Down_triggered()
{
	ui->plainTextEdit->moveSelectedBlocksDown();
}

void MainWindow::on_actionDelete_Selected_Blocks_triggered()
{
	ui->plainTextEdit->deleteSelectedBlocks();
}

void MainWindow::on_actionDuplicate_Selected_Blocks_triggered()
{
	ui->plainTextEdit->duplicateSelectedBlocks();
}

void MainWindow::on_actionLeading_WS_to_Tabs_triggered()
{
	ui->plainTextEdit->convertLeadingWhitespaceToTabs();
}

void MainWindow::on_actionLeading_WS_to_Spaces_triggered()
{
	ui->plainTextEdit->convertLeadingWhitespaceToSpaces();
}

void MainWindow::on_actionReload_Syntax_Files_triggered()
{
	ote::SyntaxDefinitionDatabase::loadFromDir("/home/s3rius/dev/qt/OpenTextEdit/syntax");
	ui->plainTextEdit->setSyntaxDefnition(ui->plainTextEdit->getSyntaxDefinition());
}
