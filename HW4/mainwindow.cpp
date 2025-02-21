#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    progress(0)  // Инициализируем прогресс в 0
{
    ui->setupUi(this);

    // Настроим RadioButtons
    ui->rB_nCh->setText("n-канальный");
    ui->rB_nCh->setChecked(true);
    ui->rB_pCh->setText("p-канальный");

    // Добавляем элементы в выпадающий список (не более 10)
    ui->cB_volt->addItem("1В");
    ui->cB_volt->addItem("2В");
    ui->cB_volt->addItem("3В");
    ui->cB_volt->addItem("4В");
    ui->cB_volt->addItem("5В");
    ui->cB_volt->addItem("6В");
    ui->cB_volt->addItem("7В");
    ui->cB_volt->addItem("8В");
    ui->cB_volt->addItem("9В");
    ui->cB_volt->addItem("10В");

    // Настроим кнопку (изменим текст и тип на Toggle)
    ui->pB->setText("Start");
    ui->pB->setCheckable(true);  // Делает кнопку переключаемой

    // Настроим прогресс-бар
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(10);
    ui->progressBar->setValue(0);

    // Подключим слот к кнопке
    connect(ui->pB, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Слот для обработки нажатия на кнопку
void MainWindow::onPushButtonClicked()
{
    // Проверим, если кнопка нажата (переключена)
    if (ui->pB->isChecked()) {
        progress += 1;  // Увеличиваем прогресс на 1
    }

    // Если прогресс больше максимума, сбрасываем его на 0
    if (progress > 10) {
        progress = 0;
    }

    // Обновляем прогресс-бар
    ui->progressBar->setValue(progress);

    // Меняем текст на кнопке в зависимости от состояния
    if (ui->pB->isChecked()) {
        ui->pB->setText("Stop");
    } else {
        ui->pB->setText("Start");
    }
}
