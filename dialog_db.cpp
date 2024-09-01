#include "dialog_db.h"
#include "ui_dialog_db.h"

DialogDB::DialogDB(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogDB)
{
    ui->setupUi(this);
    UpdateDbInfo();
}

DialogDB::~DialogDB()
{
    delete ui;
}

const InfoDB &DialogDB::GetDbInfo() const
{
    return db_info_;
}

void DialogDB::on_pb_db_conn_ok_clicked()
{
    UpdateDbInfo();
    emit SendDbInfo(db_info_);
}

void DialogDB::UpdateDbInfo()
{
    db_info_.host = ui->le_db_host->text();
    db_info_.db_name = ui->le_db_name->text();
    db_info_.login = ui->le_db_login->text();
    db_info_.password = ui->le_db_password->text();
    db_info_.port = ui->sb_db_port->value();
}
