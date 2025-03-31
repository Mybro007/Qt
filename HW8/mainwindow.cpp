#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbdata.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dataDb(new DbData(this))
    , dataBase(new DataBase(this))
{
    ui->setupUi(this);

    // Переопределяем автоматическое соединение
    disconnect(ui->pb_clear, &QPushButton::clicked, ui->tb_result, 0);
    connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::handleClearRequest);
    connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::clearTableView);

    // Настройка интерфейса
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);

    // Инициализация модели (временно пустая)
    QStandardItemModel *initModel = new QStandardItemModel(0, 2, this); // 0 строк, 2 столбца
    initModel->setHeaderData(0, Qt::Horizontal, "Название фильма");
    initModel->setHeaderData(1, Qt::Horizontal, "Год выпуска");
    ui->tb_result->setModel(initModel);

    // Настройка внешнего вида таблицы
    ui->tb_result->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tb_result->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tb_result->setSelectionMode(QAbstractItemView::SingleSelection);

    // Подключение сигналов и слотов
    connect(dataDb, &DbData::sig_sendData, this, &MainWindow::onDatabaseDataReceived);
    connect(dataBase, &DataBase::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(ui->act_connect, &QAction::triggered, this, &MainWindow::onActConnectTriggered);
    connect(ui->act_addData, &QAction::triggered, this, &MainWindow::onActAddDataTriggered);
    connect(ui->pb_request, &QPushButton::clicked, this, &MainWindow::onPbRequestClicked);
    connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::onPbClearClicked);

    // Инициализация базы данных
    dataBase->connectToDatabase();
}

void MainWindow::handleClearRequest()
{
    if (QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tb_result->model())) {
        model->setQuery(""); // Очищаем SQL модель
    }
    else if (QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tb_result->model())) {
        model->clear(); // Очищаем стандартную модель
    }

    // Восстанавливаем заголовки
    if (auto model = ui->tb_result->model()) {
        model->setHeaderData(0, Qt::Horizontal, "Название фильма");
        model->setHeaderData(1, Qt::Horizontal, "Год выпуска");
    }
}

void MainWindow::initTableView()
{
    QStandardItemModel* model = new QStandardItemModel(0, 2, this);
    model->setHeaderData(0, Qt::Horizontal, "Название фильма");
    model->setHeaderData(1, Qt::Horizontal, "Год выпуска");
    ui->tb_result->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dataDb;
    delete dataBase;
}

void MainWindow::onActAddDataTriggered()
{
    dataDb->show();
}

void MainWindow::onActConnectTriggered()
{
    if (dataBase->isConnected()) {
        dataBase->disconnectFromDatabase();
    } else {
        dataBase->connectToDatabase();
    }
}

void MainWindow::onPbRequestClicked()
{
    if (!dataBase->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к базе данных");
        return;
    }

    // Очищаем предыдущую модель
    ui->tb_result->setModel(nullptr);
    if (currentTableModel) {
        delete currentTableModel;
        currentTableModel = nullptr;
    }
    if (currentQueryModel) {
        delete currentQueryModel;
        currentQueryModel = nullptr;
    }

    QString category = ui->cb_category->currentText();

    if (category == "Все") {
        // Используем QSqlTableModel
        currentTableModel = dataBase->getTableModel("film", this);
        if (currentTableModel) {
            currentTableModel->setHeaderData(0, Qt::Horizontal, "Название");
            currentTableModel->setHeaderData(1, Qt::Horizontal, "Год выпуска");
            ui->tb_result->setModel(currentTableModel);

            // Настраиваем внешний вид
            ui->tb_result->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tb_result->setSelectionBehavior(QAbstractItemView::SelectRows);
        }
    }
    else {
        // Используем QSqlQueryModel
        QString query = QString("SELECT title, release_year FROM film f "
                                "JOIN film_category fc ON f.film_id = fc.film_id "
                                "JOIN category c ON c.category_id = fc.category_id "
                                "WHERE c.name = '%1'")
                            .arg(category == "Комедия" ? "Comedy" : "Horror");

        currentQueryModel = dataBase->getQueryModel(query, this);
        if (currentQueryModel) {
            currentQueryModel->setHeaderData(0, Qt::Horizontal, "Название");
            currentQueryModel->setHeaderData(1, Qt::Horizontal, "Год выпуска");
            ui->tb_result->setModel(currentQueryModel);

            // Настраиваем внешний вид
            ui->tb_result->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tb_result->setSelectionMode(QAbstractItemView::SingleSelection);
        }
    }
}

void MainWindow::clearTableView()
{
    QAbstractItemModel* model = ui->tb_result->model();
    if (!model) return;

    // Для QSqlQueryModel
    if (auto sqlModel = qobject_cast<QSqlQueryModel*>(model)) {
        sqlModel->setQuery(""); // Очищаем запрос
    }
    // Для QStandardItemModel
    else if (auto stdModel = qobject_cast<QStandardItemModel*>(model)) {
        stdModel->clear();
    }
    // Для других моделей
    else {
        model->removeRows(0, model->rowCount());
    }

    // Восстанавливаем заголовки
    model->setHeaderData(0, Qt::Horizontal, "Название фильма");
    model->setHeaderData(1, Qt::Horizontal, "Год выпуска");
}


void MainWindow::onPbClearClicked()
{
    // Получаем текущую модель
    QAbstractItemModel* model = ui->tb_result->model();

    // Если модель существует и является SQL моделью
    if (model) {
        // Для QSqlTableModel/QSqlQueryModel
        if (qobject_cast<QSqlTableModel*>(model) || qobject_cast<QSqlQueryModel*>(model)) {
            // Удаляем модель
            delete model;
            ui->tb_result->setModel(nullptr);
        }
        // Для стандартной модели (если использовали QStandardItemModel)
        else if (qobject_cast<QStandardItemModel*>(model)) {
            qobject_cast<QStandardItemModel*>(model)->clear();
        }
    }
}

void MainWindow::onDatabaseDataReceived(QVector<QString> dbData)
{
    // Здесь можно сохранить данные для подключения
    Q_UNUSED(dbData);
}

void MainWindow::onConnectionStatusChanged(bool connected)
{
    if (connected) {
        ui->lb_statusConnect->setText("Подключено");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->act_connect->setText("Отключиться");
        ui->pb_request->setEnabled(true);
    } else {
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->act_connect->setText("Подключиться");
        ui->pb_request->setEnabled(false);
    }
}
