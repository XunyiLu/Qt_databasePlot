#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QByteArray>
#include <QByteArray>
#include <QColor>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QtMath>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Plot Application");
//    QString test = "4.55555555555555555555555555555";
//    double test2 = test.toDouble()
//    qDebug()<< "Test 2 is " << test2;

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_upload_clicked()
{
    //    ui->widget->clearGraphs();
    QString path = QFileDialog::getOpenFileName(this,"Upload File","", "*.csv" );
    if(path.isEmpty()){
        return;
    }


    ui->listWidget_file->addItem(path);
}


void MainWindow::on_listWidget_file_itemClicked(QListWidgetItem *item)
{
    paths.append(item->text());
}



void MainWindow::on_pushButton_plot_clicked()
{
    if(paths.isEmpty()){
        QMessageBox::warning(this, "No File Select", "No File Select");
        return;
    }


    plot(paths);
//           qDebug()<<paths;
//    paths.clear();
//    ui->widget->clearGraphs();
}

void MainWindow::plot(QStringList paths)
{
    ui->widget->clearGraphs();
    if(paths.isEmpty()){
        ui->widget->legend->setVisible(false);
        ui->widget->clearGraphs();
        ui->widget->replot();
    }

    // Avoid Duplication!!! Very important
    QStringList distpaths;
    for(int i = 0; i < paths.length(); i++){
       if(!distpaths.contains(paths[i])){
          distpaths.append(paths[i]);
       }
    }
//     qDebug()<<distpaths.size();


    for(int i = 0; i < distpaths.size(); i++){

        QList<QList<double>> data = read_csvFile(distpaths[i]);
        if(data.at(0).size() < 2){
            QMessageBox::warning(this, "Wrong", "Dataset must have at least 2 columns");
            return;
        }
//        qDebug()<< data;
        QVector<double> x(data.size()), y(data.size());
//        qDebug() << "2";
//        qDebug()<<data.size();
        for (int i=0; i<data.size(); ++i)
        {
          x[i] = data.at(i).at(0);
          y[i] = data.at(i).at(1);

        }

        qDebug()<<'3';
        ui->widget->legend->setVisible(true);
        ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        ui->widget->addGraph();
        ui->widget->graph(i);
        ui->widget->graph(i)->addData(x,y);
        ui->widget->graph(i)->rescaleAxes(true);
        ui->widget->replot();

    }
}

QList<QList<double>> MainWindow::read_csvFile(QString path){
    QFile a(path);
    if(!a.exists()){
        QMessageBox::warning(this, "Wrong", "File Not Exist");
    }
    QList<QList<double>> db_list;

    QStringList lines;
    QFile inFile(path);
    if (inFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream_text(&inFile);
        while (!stream_text.atEnd())
        {
            lines.push_back(stream_text.readLine());
        }
        for (int j = 0; j < lines.size(); j++)
        {
            QString line = lines.at(j);
            QStringList split = line.split(",");

            QList<double> db;
            for (int col = 0; col < split.size(); col++)
            {
//                qDebug() << split.at(col).toDouble() << " ";
                QString temp01 = split.at(col);
                QByteArray test01 = temp01.toLatin1();
                if(test01 != "0" && temp01.toDouble() == 0){
                    QMessageBox::warning(this, "Wrong", "Data in database is not number");
                    return db_list;
                }

                db.append(temp01.toDouble());



            }
//            qDebug()<<"/n";
//            str_list.append(str);
            db_list.append(db);
        }
        inFile.close();
    }


    return db_list;
}

void MainWindow::on_pushButton_Clear_clicked()
{
    paths.clear();
    plot(paths);

}

void MainWindow::on_actionChange_Color_triggered()
{
    Dialog_colorChoose color_dlg;
    color_dlg.exec();
    QColor colorByUser = color_dlg.col;
    int temp = color_dlg.graph;
    if(temp != -1){
        int graphByUser = color_dlg.graph-1;


        if(temp > ui->widget->graphCount() || temp <= 0){
            QMessageBox::warning(this, "Warning", "Graph Not Exist");
            return;
        }

        ui->widget->graph(graphByUser)->setPen(colorByUser);
        ui->widget->replot();

    }


}

void MainWindow::contextMenuEvent(QContextMenuEvent *event){
    QMenu contextMenu;
    contextMenu.addAction(ui->actionChange_Color);
    contextMenu.exec(event->globalPos());
}

void MainWindow::on_listWidget_file_itemDoubleClicked(QListWidgetItem *item)
{
    ui->listWidget_function->addItem(item->text());
}

void MainWindow::on_pushButton_add_clicked()
{
    ui->listWidget_function->addItem("+");
}

void MainWindow::on_pushButton_min_clicked()
{
    ui->listWidget_function->addItem("-");
}


void MainWindow::on_pushButton_sqrt_clicked()
{
    ui->listWidget_function->addItem("√");
}


void MainWindow::on_pushButton_Ln_clicked()
{
    ui->listWidget_function->addItem("Ln");
}


void MainWindow::on_pushButton_eq_clicked()
{
    QList<QList<double>> sum;

    QList<QList<double>> process;


    //Uses of status：
    //1. set to 1 when read as files
    //2. set to 2 when read as addition or subtraction
    //3. set to 3 when reading log or sqrt

    //a. When reading + -, the previous one must be path, so if status=1, you can proceed. Errors are reported for 2, 3 and 0
    //b. When reading log sqrt, the previous one can be addition or subtraction, not path, and not itself. So when status=2, it can proceed. When it is 1, 3 gives an error
    //c. When reading path, the previous one can be additive or subtractive and can be sqrt log, but cannot be path. so status = 0, 2, 3, can proceed, 1 gives an error.
    int status = 0;

    //Uses of symbol：
    // + is 1;
    // - is 2;
    // √ is 3;
    // Ln is 4
    //When the address is read, look at the symbol to select the sum and the current address for calculation.
    int symbol = 0;
    qDebug() << "3. There are " << ui->widget->graphCount() << "graphs in the plot before plotting";

    //When only 1 or none database added to the function
    if(ui->listWidget_function->count() == 0 || ui->listWidget_function->count() == 1){
        QMessageBox::warning(this, "wrong","It is not a function" );
        on_pushButton_AC_clicked();
        return;
    }

     qDebug() << "2. There are " << ui->widget->graphCount() << "graphs in the plot before plotting";
    //When + or - comes first in the function
    QByteArray temp = ui->listWidget_function->item(0)->text().toLatin1();
    if(temp == "+" || temp == "-"){
        QMessageBox::warning(this, "wrong","Wrong, first item cannot be + or -" );
        on_pushButton_AC_clicked();
        return;
    }


    //read each item in the listwidget_function
    for(int i = 0; i < ui->listWidget_function->count(); i++){
        QString item = ui->listWidget_function->item(i)->text();
        if(item == "+"){
            qDebug() << "+";
            if(status == 0 ||status == 2 || status == 3){
                QMessageBox::warning(this, "wrong","Wrong, Wrong Function 1" );
                on_pushButton_AC_clicked();
                return;
            }
            status = 2;
            symbol = 1;

        }else if(item == "-"){
            qDebug() << "-";
            if(status == 0 ||status == 2 || status == 3){
                QMessageBox::warning(this, "wrong","Wrong Function 2" );
                on_pushButton_AC_clicked();
                return;
            }
            status = 2;
            symbol = 2;
        }else if(item == "√"){
            qDebug() << "√";
            if(status == 1 || status == 3){
                QMessageBox::warning(this, "wrong","Wrong, Wrong Function 3" );
                on_pushButton_AC_clicked();
                return;
            }
            status = 3;
            symbol = 3;
        }else if(item == "Ln"){
//            qDebug() << "log";
            if(status == 1 || status == 3){
                QMessageBox::warning(this, "wrong","Wrong, Wrong Function 4" );
                on_pushButton_AC_clicked();
                return;
            }
            status = 3;
            symbol = 4;
        }else{  //In this case, the item is a Path
            qDebug() << "This is a path";
            if(status == 1){
//                qDebug() << "This is path";
                QMessageBox::warning(this, "wrong","Wrong, Wrong Function 5" );
                on_pushButton_AC_clicked();
                return;
            }else if(status == 0){
                sum = read_csvFile(item);
            }else if(status == 2){ // Symbo is - or +
                process = read_csvFile(item);
                switch(symbol)
                {
                    case 1:
                            for(int i = 0; i < process.size(); i++){
                                sum[i][0] = process[i][0];
                                sum[i][1] += process[i][1];
                          }
                    break;
                    case 2:
                            for(int i = 0; i < process.size(); i++){
                                sum[i][0] = process[i][0];
                                sum[i][1] = sum[i][1]-process[i][1];
                            }
                    break;

                }
            }else if(status == 3){ //Symbol is sqrt or ln; y cannot be negative or zero
                process = read_csvFile(item);
                switch(symbol)
                {
                case 3: //sqrt
                    for(int i = 0; i < process.size(); i++){
                        if(process[i][1]<0){
                            QMessageBox::warning(this, "Wrong", "Number Cannot be Negative in sqrt");
                            on_pushButton_AC_clicked();
                            return;
                        }
                        process[i][0] = process[i][0];
                        process[i][1] = qSqrt(process[i][1]);
                    }
                    break;
                case 4: //Ln

                    for(int i = 0; i < process.size(); i++){
                        if(process[i][1]<=0){
                            QMessageBox::warning(this, "Wrong", "Number Cannot be Negative or Zero in Ln");
                            on_pushButton_AC_clicked();
                            return;
                        }
                        process[i][0] = process[i][0];
                        process[i][1] = qLn(process[i][1]);
                    }
                    break;
                }

                if(i < 2){  //Means sqrt or Ln is the first symbol
                    sum = process;
                }
                if(i >= 2){ //Means sqrt or Ln is not first symbol
                    QByteArray temp2 = ui->listWidget_function->item(i-2)->text().toLatin1();
                    if(temp2 == "+"){
                        for(int i = 0; i < process.size(); i++){
                            sum[i][0] = process[i][0];
                            sum[i][1] = sum[i][1] + process[i][1];
                        }
                    }
                    if(temp2 == "-"){
                        for(int i = 0; i < process.size(); i++){
                            sum[i][0] = process[i][0];
                            sum[i][1] = sum[i][1] - process[i][1];
                        }
                    }
                }
            }

//            qDebug()<< "TEST TEST TEST";
            status = 1;

        }
    }

    //After finish the calculating, the final item in the equantion should not be + - sqrt Ln
    //which means the status should not equal to 2, 3
    if(status == 2|| status == 3){
      QMessageBox::warning(this, "warning", "Wrong equation");
      return;
    }
    qDebug() << "1,There are " << ui->widget->graphCount() << "graphs in the plot before plotting";

    QVector<double> x(sum.size()), y(sum.size());
    for (int j=0; j<sum.size(); j++)
    {
        x[j] = sum.at(j).at(0);
        y[j] = sum.at(j).at(1);

    }


    qDebug() << "There are " << ui->widget->graphCount() << "graphs in the plot before plotting";
    //If there is no graph
    if(ui->widget->graphCount() == 0){
        ui->widget->legend->setVisible(true);
        ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        ui->widget->addGraph();
        ui->widget->graph(0);
        ui->widget->graph(0)->addData(x,y);
        ui->widget->graph(0)->rescaleAxes();
        ui->widget->replot();
    }else{//When there are graphs
        ui->widget->addGraph();
        ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        ui->widget->graph(ui->widget->graphCount()-1);
        ui->widget->graph(ui->widget->graphCount()-1)->addData(x,y);
        ui->widget->graph(ui->widget->graphCount()-1)->rescaleAxes(true);
        ui->widget->replot();
    }
        qDebug() << "There are " << ui->widget->graphCount() << "graphs in the plot after plotting";

}


void MainWindow::on_pushButton_AC_clicked()
{
    ui->listWidget_function->clear();
}

