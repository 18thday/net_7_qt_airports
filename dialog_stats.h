#ifndef DIALOG_STATS_H
#define DIALOG_STATS_H

#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QDialog>
#include <QLineSeries>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QValueAxis>

#include "database.h"

namespace Ui {
class DialogStats;
}

class DialogStats : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStats(QWidget *parent = nullptr, DataBase* db = nullptr);
    ~DialogStats();

    void UpdateForAirport(const QString& airport_name, const QString& airport_code);

    void UpdatePlotStatsPerDayInMonth(const QString& airport_code);
    void UpdatePlotStatsPerMonthInYear(const QString& airport_code);

    void PlotStatsPerDayInMonth();
    void PlotStatsPerMonthInYear();

public slots:
    void ReceiveStatsByDayPerYear(QSqlQueryModel* qm);
    void ReceiveStatsByMonthPerYear(QSqlQueryModel* qm);

private slots:
    void on_pb_close_clicked();

    void on_cb_month_currentIndexChanged(int index);

private:
    Ui::DialogStats *ui;
    DataBase* db_;
    QString airport_code_;

    QLineSeries* qls_stats_per_d_in_m;
    QBarSet* qbs_data;
    QBarSeries* qbs_stats_per_m_in_y;

    QChart* chart_qls;
    QChartView* chart_view_qls;

    QChart* chart_qbs;
    QChartView* chart_view_qbs;

    int qbs_max_value = 0;
    int qls_max_value = 0;

    QCalendar qcal;
};

#endif // DIALOG_STATS_H
