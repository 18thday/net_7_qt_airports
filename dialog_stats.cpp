#include "dialog_stats.h"
#include "ui_dialog_stats.h"

#include <QLayout>

#include <QSqlRecord>

DialogStats::DialogStats(QWidget *parent, DataBase* db)
    : QDialog(parent)
    , ui(new Ui::DialogStats)
    , db_(db)
{
    ui->setupUi(this);

    qbs_data = new QBarSet("qbs_data", this);
    qbs_data->append({0,0,0,0,0,0,0,0,0,0,0,0});
    qbs_stats_per_m_in_y = new QBarSeries(this);
    qbs_stats_per_m_in_y->setName("qbs_stats_per_m_in_y");
    qbs_stats_per_m_in_y->append(qbs_data);


    qls_stats_per_d_in_m = new QLineSeries(this);
    qls_stats_per_d_in_m->setName("qls_stats_per_d_in_m");

    chart_qls = new QChart();
    chart_view_qls = new QChartView(chart_qls);
    ui->gl_wid_plot->addWidget(chart_view_qls);
    chart_qls->legend()->hide();
    chart_qls->addSeries(qls_stats_per_d_in_m);
    chart_qls->createDefaultAxes();
    chart_view_qls->setRenderHint(QPainter::Antialiasing);

    chart_qbs = new QChart();
    chart_view_qbs = new QChartView(chart_qbs);
    ui->gl_tab_stats_year->addWidget(chart_view_qbs);
    chart_qbs->addSeries(qbs_stats_per_m_in_y);
    chart_qbs->legend()->hide();
    chart_qbs->createDefaultAxes();
    chart_view_qbs->setRenderHint(QPainter::Antialiasing);

    connect(db, &DataBase::SendStatsByDayPerMonth, this, &DialogStats::ReceiveStatsByDayPerMonth);
    connect(db, &DataBase::SendStatsByDayPerYear, this, &DialogStats::ReceiveStatsByDayPerYear);
    connect(db, &DataBase::SendStatsByMonthPerYear, this, &DialogStats::ReceiveStatsByMonthPerYear);

}

DialogStats::~DialogStats()
{
    delete ui;
    delete chart_qls;
    delete chart_view_qls;
    delete chart_qbs;
    delete chart_view_qbs;
}

void DialogStats::UpdateForAirport(const QString &airport_name, const QString &airport_code)
{
    ui->lb_airport_name->setText(airport_name);
    airport_code_ = airport_code;
    PlotStatsPerMonthInYear();
    ui->cb_month->setCurrentText("Январь");
    // PlotStatsPerDayInMonthFromDB();
    db_->GetStatsByDayPerYearForAirport(airport_code_);
    PlotStatsPerDayInMonth();
}

void DialogStats::PlotStatsPerDayInMonthFromDB()
{
    int month =  ui->cb_month->currentIndex() + 1;
    // qDebug() << month;
    db_->GetStatsByDayPerMonthForAirport(airport_code_, month);

    int limit_x = qls_max_value + (20 - (qls_max_value) % 20);
    chart_qls->axes(Qt::Vertical).first()->setRange(0, limit_x);
    chart_qls->axes(Qt::Horizontal).first()->setRange(1, qcal.daysInMonth(month, month > 8 ? 2016 : 2017));
    // axisY
    QValueAxis* axisY = qobject_cast<QValueAxis*>(chart_qls->axes(Qt::Vertical).first());
    axisY->setLabelFormat("%i");
    axisY->setTickCount(11);
    // axisX
    QValueAxis* axisX = qobject_cast<QValueAxis*>(chart_qls->axes(Qt::Horizontal).first());
    axisX->setLabelFormat("%i");
    axisX->setTickCount(qcal.daysInMonth(month, month > 8 ? 2016 : 2017));

    chart_view_qls->show();
}

void DialogStats::PlotStatsPerDayInMonth()
{
    int month =  ui->cb_month->currentIndex() + 1;

    qls_stats_per_d_in_m->clear();
    qls_max_value = 0;
    for (auto it = stats_month_to_day_to_cnt_[month].cbegin(); it != stats_month_to_day_to_cnt_[month].cend() ; ++it) {
        if (it.value() > qls_max_value) {
            qls_max_value = it.value();
        }
        qls_stats_per_d_in_m->append(it.key(), it.value());
    }

    int limit_x = qls_max_value + (20 - (qls_max_value) % 20);
    chart_qls->axes(Qt::Vertical).first()->setRange(0, limit_x);
    chart_qls->axes(Qt::Horizontal).first()->setRange(1, qcal.daysInMonth(month, month > 8 ? 2016 : 2017));
    // axisY
    QValueAxis* axisY = qobject_cast<QValueAxis*>(chart_qls->axes(Qt::Vertical).first());
    axisY->setLabelFormat("%i");
    axisY->setTickCount(11);
    // axisX
    QValueAxis* axisX = qobject_cast<QValueAxis*>(chart_qls->axes(Qt::Horizontal).first());
    axisX->setLabelFormat("%i");
    axisX->setTickCount(qcal.daysInMonth(month, month > 8 ? 2016 : 2017));

    chart_view_qls->show();
}

void DialogStats::PlotStatsPerMonthInYear()
{
    db_->GetStatsByMonthPerYearForAirport(airport_code_);
    chart_qbs->axes(Qt::Vertical).first()->setRange(0, qbs_max_value+1);
    chart_view_qbs->show();
}

void DialogStats::ReceiveStatsByDayPerMonth(QSqlQueryModel *qm)
{
    // qDebug() << "ReceiveStatsByDayPerYear";
    qls_stats_per_d_in_m->clear();
    qls_max_value = 0;
    for (int i = 0; i < qm->rowCount(); ++i) {
        // qDebug() << qm->record(i).value("d").toDate().day() << qm->record(i).value("cnt").toInt();
        auto value = qm->record(i).value("cnt").toInt();
        if (value > qls_max_value) {
            qls_max_value = value;
        }
        qls_stats_per_d_in_m->append(qm->record(i).value("d").toDate().day(), qm->record(i).value("cnt").toInt());
    }
}

void DialogStats::ReceiveStatsByDayPerYear(QSqlQueryModel *qm)
{
    for (int i = 0; i < qm->rowCount(); ++i) {
        // qDebug() << qm->record(i).value("d").toDate().day() << qm->record(i).value("d").toDate().month()<< qm->record(i).value("cnt").toInt();
        stats_month_to_day_to_cnt_[qm->record(i).value("d").toDate().month()][qm->record(i).value("d").toDate().day()]
            = qm->record(i).value("cnt").toInt();
    }
}

void DialogStats::ReceiveStatsByMonthPerYear(QSqlQueryModel *qm)
{
    // qDebug() << "ReceiveStatsByMonthPerYear";

    for (int i = 0; i < qbs_data->count(); ++i) {
         qbs_data->replace(i, 0);
    }
    qbs_max_value = 0;
    for (int i = 0; i < qm->rowCount(); ++i) {
        // qDebug() << qm->record(i).value("m").toDate().month()-1 << qm->record(i).value("cnt").toInt();
        auto value = qm->record(i).value("cnt").toInt();
        if (value > qbs_max_value) {
            qbs_max_value = value;
        }
        qbs_data->replace(qm->record(i).value("m").toDate().month()-1, qm->record(i).value("cnt").toInt());
    }
}

void DialogStats::on_pb_close_clicked()
{
    DialogStats::close();
}

void DialogStats::on_cb_month_currentIndexChanged(int index)
{
    // PlotStatsPerDayInMonthFromDB();
    PlotStatsPerDayInMonth();
}

