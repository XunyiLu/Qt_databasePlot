#include "dialog_colorchoose.h"
#include "ui_dialog_colorchoose.h"

#include <QColorDialog>

Dialog_colorChoose::Dialog_colorChoose(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_colorChoose)
{
    ui->setupUi(this);
}

Dialog_colorChoose::~Dialog_colorChoose()
{
    delete ui;
}

void Dialog_colorChoose::on_color_clicked()
{
    QColor c = QColorDialog::getColor(Qt::white, this, "Choose Color");
    if(c.isValid()){
        col = c;
    }
}


void Dialog_colorChoose::on_Dialog_colorChoose_accepted()
{
    graph = ui->spinBox->text().toInt();
}


void Dialog_colorChoose::on_buttonBox_rejected()
{
    graph = -1;
}

