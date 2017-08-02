#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hopfield.h"
#include <QImage>
#include <QPainter>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    N = 16;
    int canvasSize = 500;

    canvas = new Canvas(this, canvasSize, 0, 100);
    result = new Canvas(this, 32, 468, 50);
    result->enableDrawing(false);

    ui->InputN->setAlignment(Qt::AlignCenter);
    ui->InputN->setValidator(new QIntValidator(1, 50, this));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete canvas;
    delete result;
}

void MainWindow::on_pushButton_clicked() // shrani vzorec
{
    if (patterns.size() == 0)
    {
        N = ui->InputN->text().toInt();
        ui->InputN->setEnabled(false);
    }

    patterns.push_back(canvas->getPattern(N));
    canvas->clearCanvas();
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked() // končaj učenje
{
    hopfieldNN = new Hopfield(N*N);
    hopfieldNN->Learn(patterns, LearningMethod::STORKEY);
    ui->pushButton_3->setEnabled(true);
    patterns.clear();
    ui->pushButton_2->setEnabled(false);
    ui->InputN->setEnabled(true);
}

void MainWindow::on_pushButton_3_clicked() // razpoznaj vzorec
{
    int totalIterationLimit = 10000;
    int noChangeLimit = N*N*2; // hevristično :)
    std::vector<int> pattern = hopfieldNN->Recognise(canvas->getPattern(N), totalIterationLimit, noChangeLimit);
    qDebug() << pattern.size() << endl;
    result->drawPattern(pattern);
    canvas->clearCanvas();
}
