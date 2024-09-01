#include "database.h"
// #include "qdebug.h"

#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QMap>
#include <QDebug>
#include <QDate>
#include <QCalendar>

DataBase::DataBase(QObject *parent) : QObject(parent) {
    db_ = new QSqlDatabase();
    qm_ = new QSqlQueryModel();
    qm_stats_= new QSqlQueryModel();
}

DataBase::~DataBase() {
    delete db_;
    delete qm_;
    delete qm_stats_;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString name_db) {
    *db_ = QSqlDatabase::addDatabase(driver, name_db);
}

void DataBase::ConnectToDataBase(const InfoDB& db_info) {
    db_->setHostName(db_info.host);
    db_->setDatabaseName(db_info.db_name);
    db_->setUserName(db_info.login);
    db_->setPassword(db_info.password);
    db_->setPort(db_info.port);

    bool status = db_->open();
    emit SendStatusConnectionToDB(status);
}

void DataBase::TryToConnect()
{
    bool status = db_->open();
    emit SendStatusConnectionToDB(status);
}

void DataBase::DisconnectFromDataBase(QString name_db) {
    *db_ =  QSqlDatabase::database(name_db);
    db_->close();
}

QSqlError DataBase::GetLastError() const {
    return db_->lastError();
}

void DataBase::RequestToDataBase(QString request) {}

void DataBase::GetAirports()
{
    QString query = "SELECT airport_name->>'ru' AS airport_name, airport_code FROM bookings.airports_data";
    qm_->setQuery(query, GetDataBase());

    QMap<QString, QString> airports;
    for (int i = 0; i < qm_->rowCount(); ++i) {
        airports[qm_->record(i).value("airport_name").toString() + " (" + qm_->record(i).value("airport_code").toString() + ")"]
            = qm_->record(i).value("airport_code").toString();
    }
    emit SendAirports(std::move(airports));
}

void DataBase::GetArrivalFlights(QString airport_code, QDate date)
{
    QString query = "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' AS airport_name "
                    "FROM bookings.flights AS f "
                    "JOIN bookings.airports_data AS ad ON ad.airport_code = f.departure_airport "
                    "WHERE f.arrival_airport = '" + airport_code + "' "
                    "AND scheduled_arrival::date = date('" + date.toString("yyyy-MM-dd") + "') "
                    "ORDER BY scheduled_arrival::time";

    qm_->setQuery(query, GetDataBase());
    qm_->setHeaderData(0, Qt::Horizontal, tr("Номер рейса"));
    qm_->setHeaderData(1, Qt::Horizontal, tr("Время прилета"));
    qm_->setHeaderData(2, Qt::Horizontal, tr("Аэропорт отправления"));

    qDebug() << qm_->rowCount();

    emit SendFlightsInfo(qm_);
}

void DataBase::GetDepartureFlights(QString airport_code, QDate date)
{
    QString query = "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' AS airport_name "
                    "FROM bookings.flights AS f "
                    "JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport "
                    "WHERE f.departure_airport = '" + airport_code + "' "
                    "AND scheduled_departure::date = date('" + date.toString("yyyy-MM-dd") + "') "
                    "ORDER BY scheduled_departure::time";

    qm_->setQuery(query, GetDataBase());
    qm_->setHeaderData(0, Qt::Horizontal, tr("Номер рейса"));
    qm_->setHeaderData(1, Qt::Horizontal, tr("Время вылета"));
    qm_->setHeaderData(2, Qt::Horizontal, tr("Аэропорт назначения"));

    qDebug() << qm_->rowCount();

    emit SendFlightsInfo(qm_);
}

void DataBase::GetStatsByDayPerYearForAirport(QString airport_code, int month)
{
    int year = month > 8 ? 2016 : 2017;
    QCalendar qcal;
    auto min_day = QDate(year, month, 1);
    auto max_day = QDate(year, month, qcal.daysInMonth(month, year));
    QString query = "SELECT count(flight_no) AS cnt, date_trunc('day', scheduled_departure) AS d "
                    "FROM bookings.flights AS f "
                    "WHERE (scheduled_departure::date >= date('" + min_day.toString("yyyy-MM-dd") + "') "
                    "AND scheduled_departure::date <= date('" + max_day.toString("yyyy-MM-dd") + "')) "
                    "AND ((departure_airport = '" + airport_code + "') "
                    "OR (arrival_airport = '" + airport_code + "')) "
                    "GROUP BY d";
    qDebug() << query;
    qm_stats_->setQuery(query, GetDataBase());
    qDebug() << qm_stats_->rowCount();

    emit SendStatsByDayPerYear(qm_stats_);
}

void DataBase::GetStatsByMonthPerYearForAirport(QString airport_code)
{
    QString query = "SELECT count(flight_no) AS cnt, date_trunc('month', scheduled_departure) AS m "
                    "FROM bookings.flights AS f "
                    "WHERE (scheduled_departure::date > date('2016-08-31') AND scheduled_departure::date <= date('2017-08-31')) "
                    "AND ((departure_airport = '" + airport_code + "') "
                    "OR (arrival_airport = '" + airport_code + "')) "
                    "GROUP BY m";
    qDebug() << query;
    qm_stats_->setQuery(query, GetDataBase());
    qDebug() << qm_stats_->rowCount();

    emit SendStatsByMonthPerYear(qm_stats_);
}


const QSqlDatabase &DataBase::GetDataBase() const {
    return *db_;
}
