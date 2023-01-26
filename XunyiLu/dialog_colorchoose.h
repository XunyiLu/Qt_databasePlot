#ifndef DIALOG_COLORCHOOSE_H
#define DIALOG_COLORCHOOSE_H
#include <QDialog>
#include <QColor>


namespace Ui {
class Dialog_colorChoose;
}

class Dialog_colorChoose : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_colorChoose(QWidget *parent = nullptr);

    QColor col;

    int graph = 0;
    ~Dialog_colorChoose();

private slots:
    void on_color_clicked();

    void on_Dialog_colorChoose_accepted();

    void on_buttonBox_rejected();

private:
    Ui::Dialog_colorChoose *ui;
};

#endif // DIALOG_COLORCHOOSE_H
