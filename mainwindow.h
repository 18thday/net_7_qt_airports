#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QLabel>

#include "dialog_db.h"
#include "dialog_stats.h"
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void UpdateStatusBar();
    void UpdateConnectionStatus();
    void InitUI();
    void UpdateUIEnable();

public slots:
    void ReceiveStatusConnectionToDB(bool status);
    void StartTimerForConnectionToDB();
    void ReceiveAirports(QMap<QString, QString> airports);
    void ReceiveFlightsInfo(QSqlQueryModel* qm);

private slots:
    void on_act_db_info_triggered();

    void on_act_db_conn_triggered();

    void on_pb_flight_info_clicked();

    void on_pb_airport_stats_clicked();

private:
    Ui::MainWindow *ui;
    DialogDB* dial_db_conn;
    DialogStats* dial_stats;
    DataBase* db;
    QMessageBox* msg;
    QLabel* lb_db_conn_status;

    bool is_connected_to_db = false;
    QTimer* timer;

    QMap<QString, QString> airports_;
};
#endif // MAINWINDOW_H
