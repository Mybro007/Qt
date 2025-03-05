#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , chart(new QChart()) // Создаём объект QChart один раз
    , series(new QLineSeries()) // Создаём объект QLineSeries один раз
    , chartView(new QChartView(chart)) // Создаём QChartView один раз
{
    ui->setupUi(this);
    ui->pb_clearResult->setCheckable(true);

    connect(this, &MainWindow::dataReadyForPlot, this, &MainWindow::onDataReadyForPlot);

    // Инициализируем график
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("График данных");
    chartView->setRenderHint(QPainter::Antialiasing);
}

MainWindow::~MainWindow()
{
    delete ui;
    // Удаляем графику и серию, если они больше не нужны
    delete series;
    delete chart;
    delete chartView;
}

/****************************************************/
/*!
@brief: Метод считывает данные из файла
@param: path - путь к файлу
        numberChannel - какой канал АЦП считать
*/
/****************************************************/
QVector<uint32_t> MainWindow::ReadFile(QString path, uint8_t numberChannel)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    if (file.isOpen() == false) {
        if (pathToFile.isEmpty()) {
            QMessageBox mb;
            mb.setWindowTitle("Ошибка");
            mb.setText("Ошибка открытия файла");
            mb.exec();
        }
    }

    QDataStream dataStream;
    dataStream.setDevice(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    QVector<uint32_t> readData;
    uint32_t currentWord = 0, sizeFrame = 0;

    while (dataStream.atEnd() == false) {
        dataStream >> currentWord;

        if (currentWord == 0xFFFFFFFF) {
            dataStream >> currentWord;

            if (currentWord < 0x80000000) {
                dataStream >> sizeFrame;

                if (sizeFrame > 1500) {
                    continue;
                }

                for (int i = 0; i < sizeFrame / sizeof(uint32_t); i++) {
                    dataStream >> currentWord;

                    if ((currentWord >> 24) == numberChannel) {
                        readData.append(currentWord);
                    }
                }
            }
        }
    }

    ui->chB_readSucces->setChecked(true);
    return readData;
}

QVector<double> MainWindow::ProcessFile(const QVector<uint32_t> dataFile)
{
    QVector<double> resultData;
    foreach (int word, dataFile) {
        word &= 0x00FFFFFF;
        if (word > 0x800000) {
            word -= 0x1000000;
        }

        double res = ((double)word / 6000000) * 10;
        resultData.append(res);
    }
    ui->chB_procFileSucces->setChecked(true);
    return resultData;
}

QVector<double> MainWindow::FindMax(QVector<double> resultData)
{
    double max = 0, sMax = 0;

    foreach (double num, resultData) {
        if (num > max) {
            max = num;
        }
    }

    foreach (double num, resultData) {
        if (num > sMax && (qFuzzyCompare(num, max) == false)) {
            sMax = num;
        }
    }

    QVector<double> maxs = {max, sMax};
    ui->chB_maxSucess->setChecked(true);
    return maxs;
}

QVector<double> MainWindow::FindMin(QVector<double> resultData)
{
    double min = 0, sMin = 0;

    foreach (double num, resultData) {
        if (num < min) {
            min = num;
        }
    }

    foreach (double num, resultData) {
        if (num < sMin && (qFuzzyCompare(num, min) == false)) {
            sMin = num;
        }
    }

    QVector<double> mins = {min, sMin};
    ui->chB_minSucess->setChecked(true);
    return mins;
}

void MainWindow::DisplayResult(QVector<double> mins, QVector<double> maxs)
{
    ui->te_Result->append("Расчет закончен!");

    ui->te_Result->append("Первый минимум " + QString::number(mins.first()));
    ui->te_Result->append("Второй минимум " + QString::number(mins.at(1)));

    ui->te_Result->append("Первый максимум " + QString::number(maxs.first()));
    ui->te_Result->append("Второй максимум " + QString::number(maxs.at(1)));
}

/****************************************************/
/*!
@brief: Обработчик клика на кнопку "Выбрать путь"
*/
/****************************************************/
void MainWindow::on_pb_path_clicked()
{
    pathToFile = "";
    ui->le_path->clear();

    pathToFile = QFileDialog::getOpenFileName(this,
                                              tr("Открыть файл"), "/home/", tr("ADC Files (*.adc)"));

    ui->le_path->setText(pathToFile);
}

/****************************************************/
/*!
@brief: Обработчик клика на кнопку "Старт"
*/
/****************************************************/
void MainWindow::on_pb_start_clicked()
{
    if (pathToFile.isEmpty()) {
        QMessageBox mb;
        mb.setWindowTitle("Ошибка");
        mb.setText("Выберите файл!");
        mb.exec();
        return;
    }

    ui->chB_maxSucess->setChecked(false);
    ui->chB_procFileSucces->setChecked(false);
    ui->chB_readSucces->setChecked(false);
    ui->chB_minSucess->setChecked(false);

    int selectIndex = ui->cmB_numCh->currentIndex();
    if (selectIndex == 0) {
        numberSelectChannel = 0xEA;
    } else if (selectIndex == 1) {
        numberSelectChannel = 0xEF;
    } else if (selectIndex == 2) {
        numberSelectChannel = 0xED;
    }

    auto read = [&]{ return ReadFile(pathToFile, numberSelectChannel); };
    auto process = [&](QVector<uint32_t> res){ return ProcessFile(res);};
    auto findMax = [&](QVector<double> res){
        maxs = FindMax(res);
        mins = FindMin(res);
        DisplayResult(mins, maxs);

        QVector<QPointF> points;
        int numPoints = qMin(res.size(), 1000);
        for (int i = 0; i < numPoints; ++i) {
            points.append(QPointF(i / FD, res[i]));
        }

        emit dataReadyForPlot(points);
    };

    auto result = QtConcurrent::run(read)
                      .then(process)
                      .then(findMax);
}

void MainWindow::onDataReadyForPlot(const QVector<QPointF>& points)
{
    // Ограничиваем количество точек до 1000 (для 1 секунды при частоте 1 точка/мс)
    int numPointsToDisplay = qMin(points.size(), 1000);

    // Очищаем серию перед добавлением новых данных
    series->clear();

    // Добавляем нормализованные точки в серию
    for (int i = 0; i < numPointsToDisplay; ++i) {
        // Нормализуем ось X: X = i / 1000.0 (для 1000 точек на 1 секунду)
        double normalizedX = i / 1000.0;
        series->append(normalizedX, points[i].y());
    }

    // Настроим оси для корректного отображения
    // Устанавливаем диапазон оси X от 0 до 1
    chart->axisX()->setRange(0, 1);

    // Рассчитываем минимальное и максимальное значение для оси Y
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (int i = 0; i < numPointsToDisplay; ++i) {
        const QPointF& point = points[i];
        if (point.y() < minY) {
            minY = point.y();
        }
        if (point.y() > maxY) {
            maxY = point.y();
        }
    }

    // Устанавливаем диапазон оси Y
    chart->axisY()->setRange(minY, maxY);

    // Отображаем график
    chartView->show();
}
