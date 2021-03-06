#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionSelect_Light_Theme_triggered();

    void on_actionSelect_Dark_Theme_triggered();

    void on_actionShow_Whitespace_toggled(bool arg1);

    void on_actionShow_Linebreaks_toggled(bool arg1);

    void on_actionEnable_Smart_Indent_toggled(bool arg1);

    void on_actionEnable_Line_Wrap_toggled(bool arg1);

    void on_actionAuto_convert_tab_to_spaces_toggled(bool arg1);

    void on_actionReset_Zoom_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionSet_Document_Unmodified_triggered();

    void on_actionReload_Theme_Files_triggered();

    void on_actionMove_Selection_Up_triggered();

    void on_actionMove_Selection_Down_triggered();

    void on_actionDelete_Selected_Blocks_triggered();

    void on_actionDuplicate_Selected_Blocks_triggered();

    void on_actionLeading_WS_to_Tabs_triggered();

    void on_actionLeading_WS_to_Spaces_triggered();

    void on_actionReload_Syntax_Files_triggered();

    void on_actionShow_End_of_Line_markers_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
