#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitUI();

    // DIALOG_DB
    dial_db_conn = new DialogDB(this);
    dial_db_conn->setModal(true);

    // DATABASE
    db = new DataBase(this);
    db->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    // DIALOG_STATS
    dial_stats = new DialogStats(this, db);
    dial_stats->setModal(true);

    // MESSAGE
    msg = new QMessageBox(this);

    // Timer
    timer = new QTimer(this);

    // STATUS BAR
    lb_db_conn_status = new QLabel(this);
    statusBar()->addWidget(lb_db_conn_status);
    UpdateConnectionStatus();

    // CONNECT
    connect(db, &DataBase::SendStatusConnectionToDB, this, &MainWindow::ReceiveStatusConnectionToDB);
    connect(msg, &QMessageBox::finished, this, &MainWindow::StartTimerForConnectionToDB);
    connect(timer, &QTimer::timeout, db, &DataBase::TryToConnect);
    connect(db, &DataBase::SendAirports, this, &MainWindow::ReceiveAirports);
    connect(db, &DataBase::SendFlightsInfo, this, &MainWindow::ReceiveFlightsInfo);

    // FIRST CONNECTION
    db->ConnectToDataBase(dial_db_conn->GetDbInfo());

    // UI
    UpdateUIEnable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    if (status) {
        is_connected_to_db = true;
        UpdateConnectionStatus();
        ui->statusbar->clearMessage();
        ui->act_db_conn->setText("Отключиться");
        db->GetAirports();
        UpdateUIEnable();

    } else {
        is_connected_to_db = false;
        UpdateConnectionStatus();
        UpdateUIEnable();

        timer->stop();
        msg->setIcon(QMessageBox::Critical);
        msg->setText(db->GetLastError().text());
        msg->exec();
        UpdateUIEnable();
    }
}

void MainWindow::StartTimerForConnectionToDB()
{
    timer->start(5000);
}

void MainWindow::ReceiveAirports(QMap<QString, QString> airports)
{
    airports_ = std::move(airports);
    ui->cb_airport->clear();
    for (auto it = airports_.cbegin(); it != airports_.cend() ; ++it) {
        ui->cb_airport->addItem(it.key());
    }
}

void MainWindow::ReceiveFlightsInfo(QSqlQueryModel* qm)
{
    ui->tv_flight_info->setModel(qm);
    ui->tv_flight_info->resizeColumnsToContents();
    ui->tv_flight_info->show();
}

void MainWindow::UpdateConnectionStatus()
{
    if (is_connected_to_db) {
        lb_db_conn_status->setStyleSheet("color : green");
        lb_db_conn_status->setText("Подключено");
    } else {
        lb_db_conn_status->setStyleSheet("color : red");
        lb_db_conn_status->setText("Отключено");
    }
}

void MainWindow::InitUI()
{
    ui->rb_departure->setChecked(true);
    ui->pb_flight_info->setEnabled(false);
    ui->gb_airport_data->setEnabled(false);
    ui->gb_stats->setEnabled(false);
}

void MainWindow::UpdateUIEnable()
{
    if (is_connected_to_db) {
        ui->gb_airport_data->setEnabled(true);
        if (ui->cb_airport->count() != 0) {
            ui->pb_flight_info->setEnabled(true);
            ui->gb_stats->setEnabled(true);
        }
    } else {
        ui->gb_airport_data->setEnabled(false);
        ui->gb_stats->setEnabled(false);
    }
}

void MainWindow::on_act_db_info_triggered()
{
    dial_db_conn->show();
}

void MainWindow::on_act_db_conn_triggered()
{
    if (!is_connected_to_db) {
        ui->statusbar->setStyleSheet("color : black");
        ui->statusbar->showMessage("Идет подключение");

        auto func_conn = [&](){db->ConnectToDataBase(dial_db_conn->GetDbInfo());};
        (void) QtConcurrent::run(func_conn);

    } else {
        db->DisconnectFromDataBase(DB_NAME);
        ui->act_db_conn->setText("Подключиться");
        is_connected_to_db = false;
        UpdateConnectionStatus();
        UpdateUIEnable();
    }
}

void MainWindow::on_pb_flight_info_clicked()
{
    qDebug() << "on_pb_flight_info_clicked";
    if (ui->rb_arrival->isChecked()) {
        qDebug() << "ui->rb_arrival->isChecked()";
        qDebug() << ui->cb_airport->currentText();
        if (airports_.contains(ui->cb_airport->currentText())) {
            db->GetArrivalFlights(airports_[ui->cb_airport->currentText()], ui->de_flight_date->date());
        }
    } else if (ui->rb_departure->isChecked()) {
        qDebug() << "ui->rb_departure->isChecked()";
        qDebug() << ui->cb_airport->currentText();
        if (airports_.contains(ui->cb_airport->currentText())) {
            qDebug() << airports_[ui->cb_airport->currentText()];
            db->GetDepartureFlights(airports_[ui->cb_airport->currentText()], ui->de_flight_date->date());
        }
    }
}


void MainWindow::on_pb_airport_stats_clicked()
{
    if (airports_.contains(ui->cb_airport->currentText())) {
        dial_stats->UpdateForAirport(ui->cb_airport->currentText(), airports_[ui->cb_airport->currentText()]);
        dial_stats->show();
    }
}

