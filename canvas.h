#ifndef CANVAS_H
#define CANVAS_H
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <vector>
#include <QPoint>

class Canvas : public QLabel
{
    Q_OBJECT

    QImage* img;
    bool isClicked = false;
    QPoint topLeft;
    QPoint bottomRight;
    bool drawingEnabled = true;

    public:
    Canvas(QWidget *parent, int cSize, int xOffset, int yOffset, Qt::WindowFlags f=0);
    Canvas(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
    ~Canvas();
    int N = 16; // stranica obrezane in skalirane slike
    int canvasSize;
    void clearCanvas();
    void drawPattern(std::vector<int>);
    void enableDrawing(bool);
    void setN(int);

    std::vector<int> getPattern(int);

    protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent * event);
};

#endif // CANVAS_H
