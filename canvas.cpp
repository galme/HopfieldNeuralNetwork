#include "canvas.h"


Canvas::Canvas(QWidget *parent, int cSize, int xOffset, int yOffset, Qt::WindowFlags f):QLabel(parent,f)
{
    canvasSize = cSize;
    img = new QImage(canvasSize, canvasSize, QImage::Format::Format_RGB32);
    for (int i = 0; i != canvasSize; i++)
        for (int j = 0; j != canvasSize; j++)
        {
            img->setPixel(i, j, 0);
            img->setPixelColor(i, j, QColor(255, 255, 255));
        }
    this->setPixmap(QPixmap::fromImage(*img));

    this->setMouseTracking(true);
    //this->setGeometry(xOffset, yOffset, this->parentWidget()->width(), this->parentWidget()->height());
    this->move(xOffset, yOffset);
    this->resize(canvasSize, canvasSize);

    topLeft = QPoint(10000, 10000);
    bottomRight = QPoint(0, 0);
}
void Canvas::setN(int n)
{
    N = n;
}

Canvas::Canvas(const QString &text, QWidget *parent, Qt::WindowFlags f):QLabel(text,parent,f){}
Canvas::~Canvas(){
    delete img;
}

void Canvas::mousePressEvent(QMouseEvent *ev)
{
    if (ev->buttons() == Qt::LeftButton)
        isClicked = true;

    ev->pos();
    //emit clicked(ev->pos());
}

void Canvas::enableDrawing(bool enable)
{
    drawingEnabled = enable;
}

void Canvas::mouseReleaseEvent(QMouseEvent * event)
{
    isClicked = false;
}

void Canvas::mouseMoveEvent(QMouseEvent *ev)
{
    if (isClicked && drawingEnabled)
    {
        int x = ev->pos().x();
        int y = ev->pos().y();

        int thickness = 10;
        for (int i = x - thickness; i < x + thickness; i++)
        {
            for (int j = y - thickness; j < y + thickness; j++)
            {
                if (i < canvasSize && j < canvasSize && i >= 0 && j >= 0)
                {
                    if (i < topLeft.x())
                        topLeft.setX(i);
                    if (j < topLeft.y())
                        topLeft.setY(j);

                    if (i > bottomRight.x())
                        bottomRight.setX(i);
                    if (j > bottomRight.y())
                        bottomRight.setY(j);

                    img->setPixelColor(i, j, QColor(0, 0, 0));

                }
            }
        }

        this->setPixmap(QPixmap::fromImage(*img));
    }
}

std::vector<int> Canvas::getPattern(int n)
{
    setN(n);
    QImage croppedImg = img->copy(topLeft.x(), topLeft.y(), bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y());
    croppedImg = croppedImg.scaled(N, N);

    std::vector<int> result;
    result.reserve(N*N);
    for (int i = 0; i != N; i++)
    {
        for (int j = 0; j != N; j++)
        {
            result.push_back((croppedImg.pixelColor(i, j) == QColor(0, 0, 0)) ? 1 : -1); // črne barve (znak) označimo z 1, bele barve (ozadje) pa z -1
        }
    }

    //this->setPixmap(QPixmap::fromImage(croppedImg));

    return result;
}

void Canvas::clearCanvas()
{
    img = new QImage(canvasSize, canvasSize, QImage::Format::Format_RGB32);
    for (int i = 0; i != canvasSize; i++)
        for (int j = 0; j != canvasSize; j++)
        {
            img->setPixel(i, j, 0);
            img->setPixelColor(i, j, QColor(255, 255, 255));
        }
    this->setPixmap(QPixmap::fromImage(*img));

    topLeft = QPoint(10000, 10000);
    bottomRight = QPoint(0, 0);
}

void Canvas::drawPattern(std::vector<int> pattern)
{
    int size_sqrt = sqrt(pattern.size());
    img = new QImage(size_sqrt, size_sqrt, QImage::Format::Format_RGB32);
    for (int i = 0; i != pattern.size(); i++)
    {
        QColor color = (pattern[i] == 1 ? QColor(0, 0, 0) : QColor(255, 255, 255));
        img->setPixelColor(i / size_sqrt, i % size_sqrt, color);
    }

    QImage scaled = img->scaled(canvasSize, canvasSize);

    this->setPixmap(QPixmap::fromImage(scaled));
}
