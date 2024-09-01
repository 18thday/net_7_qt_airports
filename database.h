#ifndef DATABASE_H
#define DATABASE_H

#include <QDate>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QVector>

#include "airports.h"

#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"


struct InfoDB {
    QString host;
    QString db_name;
    QString login;
    QString password;
    uint port;
};

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    void AddDataBase(QString driver, QString name_db = "");

    void ConnectToDataBase(const InfoDB& db_info);
    void TryToConnect();
    void DisconnectFromDataBase(QString name_db = "");

    QSqlError GetLastError() const;

    void RequestToDataBase(QString request);
    void GetAirports();
    void GetArrivalFlights(QString airport_code, QDate date);
    void GetDepartureFlights(QString airport_code, QDate date);
    void GetStatsByDayPerYearForAirport(QString airport_code, int month);
    void GetStatsByMonthPerYearForAirport(QString airport_code);
    const QSqlDatabase& GetDataBase() const;

signals:
    void SendStatusConnectionToDB(bool status);
    void SendAirports(QMap<QString, QString> airports);
    void SendFlightsInfo(QSqlQueryModel* flights_info);
    void SendStatsByDayPerYear(QSqlQueryModel* stats_day_per_year);
    void SendStatsByMonthPerYear(QSqlQueryModel* stats_month_per_year);


private:
    QSqlDatabase* db_;
    QSqlQueryModel* qm_;
    QSqlQueryModel* qm_stats_;
};

#endif // DATABASE_H
