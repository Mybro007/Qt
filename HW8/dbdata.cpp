#include "dbdata.h"
#include "ui_dbdata.h"

// Количество полей для подключения
const int NUM_CONNECTION_FIELDS = 5;

DbData::DbData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbData),
    connectionData(NUM_CONNECTION_FIELDS)
{
    ui->setupUi(this);
    setWindowTitle("Параметры подключения");
    initDefaultValues();
}

DbData::~DbData()
{
    delete ui;
}

QVector<QString> DbData::getConnectionData() const
{
    return connectionData;
}

void DbData::initDefaultValues()
{
    // Установка значений по умолчанию
    ui->le_host->setText("981757-ca08998.tmweb.ru");
    ui->le_dbName->setText("netology_cpp");
    ui->le_login->setText("netology_usr_cpp");
    ui->le_pass->setText("CppNeto3");
    ui->spB_port->setValue(5432);
}

void DbData::on_buttonBox_accepted()
{
    // Сохранение введенных данных
    connectionData[0] = ui->le_host->text();
    connectionData[1] = ui->le_dbName->text();
    connectionData[2] = ui->le_login->text();
    connectionData[3] = ui->le_pass->text();
    connectionData[4] = ui->spB_port->text();

    emit sig_sendData(connectionData);
    accept();
}

void DbData::on_buttonBox_rejected()
{
    reject();
}
