#ifndef DIALOG_DB_H
#define DIALOG_DB_H

#include <QDialog>

#include "database.h"

namespace Ui {
class DialogDB;
}

class DialogDB : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDB(QWidget *parent = nullptr);
    ~DialogDB();

    const InfoDB& GetDbInfo() const;

signals:
    void SendDbInfo(const InfoDB& db_info);

private slots:
    void on_pb_db_conn_ok_clicked();

private:
    Ui::DialogDB *ui;
    InfoDB db_info_;

    void UpdateDbInfo();
};

#endif // DIALOG_DB_H
