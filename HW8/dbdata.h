#ifndef DBDATA_H
#define DBDATA_H

#include <QDialog>
#include <QVector>

namespace Ui {
class DbData;
}

class DbData : public QDialog
{
    Q_OBJECT

public:
    explicit DbData(QWidget *parent = nullptr);
    ~DbData();

    // Получение введенных данных
    QVector<QString> getConnectionData() const;

signals:
    void sig_sendData(QVector<QString> dbData);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::DbData *ui;
    QVector<QString> connectionData;

    // Инициализация полей значениями по умолчанию
    void initDefaultValues();
};

#endif // DBDATA_H
