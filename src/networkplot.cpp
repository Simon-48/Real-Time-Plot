#include "networkplot.h"
#include "ui_networkplot.h"

NetworkPlot::NetworkPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetworkPlot)
{
    ui->setupUi(this);

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    ui->customPlot->xAxis->setLabel("Time");
    ui->customPlot->yAxis->setLabel("Voltage");
    ui->customPlot->xAxis->setRange(0,100);
    ui->customPlot->yAxis->setRange(0,100);

    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->plotTypeComboBox->setCurrentIndex(0);
    ui->lineStyleComboBox->setCurrentIndex(1);

    ServerThread *thread = new ServerThread(this);

    connect(thread,SIGNAL(newDataRecieved(QByteArray)),this,SLOT(parseData(QByteArray)));
    thread->start();

}

NetworkPlot::~NetworkPlot()
{
    delete ui;
}

void NetworkPlot::parseData(QByteArray Data)
{
    qDebug()<< "Got data:"<< Data;

    QJsonParseError parseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(Data,&parseError);

    if(parseError.error != QJsonParseError::NoError){
        qDebug()<< "Parse Error"<< parseError.errorString();
        return;
    }

    QJsonArray jsonArray = jsonResponse.array();

    if(!jsonArray.isEmpty()){
        QJsonObject jsonObject = jsonArray.first().toObject();

        QVector <double> x,y;


        for(int i=0;i<jsonObject.value("X").toArray().size();i++){
            x.push_back(jsonObject.value("X").toArray()[i].toDouble());
            y.push_back(jsonObject.value("Y").toArray()[i].toDouble());

        }

        ui->customPlot->graph(0)->setData(x,y);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
        ui->customPlot->update();
    }
}

void NetworkPlot::on_clearButton_clicked()
{
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->replot();
    ui->customPlot->update();
}

void NetworkPlot::on_lineStyleComboBox_currentIndexChanged(int index)
{
    switch (index) {
                case 0:
                        ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
                        break;
                case 1:
                        ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
                        break;

                default:
                        ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
                        break;

            }
            ui->customPlot->replot();
            ui->customPlot->update();
}

void NetworkPlot::on_plotTypeComboBox_currentIndexChanged(int index)
{
    switch (index) {
                case 0:
                        ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
                        break;

                case 1:
                        ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
                        break;
                case 2:
                        ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
                        break;

                default:
                        ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
                        break;
            }
        ui->customPlot->replot();
        ui->customPlot->update();
}
