#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include <QContextMenuEvent>
#include <QColor>
#include "dialog_colorchoose.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    QStringList paths;

    QColor m_color;

    QList<QList<double> > read_csvFile(QString path);

    ~MainWindow();

    void plot(QStringList paths);
private slots:
    void on_pushButton_upload_clicked();

    void on_listWidget_file_itemClicked(QListWidgetItem *item);

    void on_pushButton_plot_clicked();

    void on_pushButton_Clear_clicked();

    void on_actionChange_Color_triggered();

    void contextMenuEvent(QContextMenuEvent *event);

    void on_listWidget_file_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_add_clicked();

    void on_pushButton_min_clicked();

    void on_pushButton_sqrt_clicked();

    void on_pushButton_Ln_clicked();

    void on_pushButton_eq_clicked();

    void on_pushButton_AC_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
