#include <QMessageBox>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QRectF>
#include <QPainter>
#include <qmath.h>
#include <QRandomGenerator>
#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    timer(new QTimer(this)),
    generations(-1),
    universeSize(50)
{
    timer->setInterval(300);
    m_masterColor = "#000";
    universe = new int[(universeSize + 2) * (universeSize + 2)];
    next = new int[(universeSize + 2) * (universeSize + 2)];
    connect(timer, SIGNAL(timeout()), this, SLOT(newGeneration()));
    memset(universe, 0, sizeof(int)*(universeSize + 2) * (universeSize + 2));
    memset(next, 0, sizeof(int)*(universeSize + 2) * (universeSize + 2));
}

GameWidget::~GameWidget()
{
    delete [] universe;
    delete [] next;
}

void GameWidget::startGame(const int &number)
{
    generations = number;
    timer->start();
}

void GameWidget::stopGame()
{
    timer->stop();
}

void GameWidget::clear()
{
    for(size_t k = 1; k <= universeSize; k++) {
        for(size_t j = 1; j <= universeSize; j++) {
            universe[getIndex(k,j)] = 0;
        }
    }
    gameEnds(true);
    update();
}

void GameWidget::fillRand()
{
    for(size_t k = 1; k <= universeSize; k++) {
        for(size_t j = 1; j <= universeSize; j++) {
            universe[getIndex(k,j)] = QRandomGenerator::global()->bounded(0,2);
        }
    }
    gameEnds(true);
    update();
}

size_t GameWidget::cellNumber()
{
    return universeSize;
}

void GameWidget::setCellNumber(const size_t &s)
{
    universeSize = s;
    resetUniverse();
    update();
}

void GameWidget::resetUniverse()
{
    delete [] universe;
    delete [] next;
    universe = new int[(universeSize + 2) * (universeSize + 2)];
    next = new int[(universeSize + 2) * (universeSize + 2)];
    memset(universe, 0, sizeof(int)*(universeSize + 2) * (universeSize + 2));
    memset(next, 0, sizeof(int)*(universeSize + 2) * (universeSize + 2));
}

size_t GameWidget::getIndex(size_t k, size_t l)
{
    return(universeSize*k + l);
}

QString GameWidget::dump()
{
    char temp;
    QString master = "";
    for(size_t k = 1; k <= universeSize; k++) {
        for(size_t j = 1; j <= universeSize; j++) {
            if(universe[k*universeSize + j] == 1) {
                temp = '*';
            } else {
                temp = 'o';
            }
            master.append(temp);
        }
        master.append("\n");
    }
    return master;
}

void GameWidget::setDump(const QString &data)
{
    int current = 0;
    for(size_t k = 1; k <= universeSize; k++) {
        for(size_t j = 1; j <= universeSize; j++) {
            if(data[current] == '*') {
                universe[getIndex(k,j)] = 1;
            }
            current++;
        }
        current++;
    }
    update();
}

int GameWidget::interval()
{
    return timer->interval();
}

void GameWidget::setInterval(int msec)
{
    timer->setInterval(msec);
}

int GameWidget::isAlive(size_t k, size_t j)
{
    int power = 0;
    power += universe[getIndex(k+1,j)]; //universe[(k+1)*universeSize +  j];
    power += universe[getIndex(k-1,j)]; //universe[(k-1)*universeSize + j];
    power += universe[getIndex(k,j+1)]; //universe[k*universeSize + (j+1)];
    power += universe[getIndex(k,j-1)]; //universe[k*universeSize + (j-1)];
    power += universe[getIndex(k+1,j-1)]; //universe[(k+1)*universeSize + ( j-1)];
    power += universe[getIndex(k-1,j+1)]; //universe[(k-1)*universeSize + (j+1)];
    power += universe[getIndex(k-1,j-1)]; //universe[(k-1)*universeSize + (j-1)];
    power += universe[getIndex(k+1,j+1)]; //universe[(k+1)*universeSize +  (j+1)];
    if (((universe[getIndex(k,j)] == 1) && (power == 2)) || (power == 3))
           return 1;
    return 0;
}

void GameWidget::newGeneration()
{
    if(generations < 0)
        generations++;
    size_t notChanged=0;
    for(size_t k=1; k <= universeSize; k++) {
        for(size_t j=1; j <= universeSize; j++) {
            next[getIndex(k,j)] = isAlive(k, j);
            if(next[getIndex(k,j)] == universe[getIndex(k,j)])
                notChanged++;
        }
    }
    if(notChanged == universeSize*universeSize) {
        QMessageBox::information(this,
                                 tr("Game lost sense"),
                                 tr("The End. Now game finished because all the next generations will be the same."),
                                 QMessageBox::Ok);
        stopGame();
        gameEnds(true);
        return;
    }
    for(size_t k=1; k <= universeSize; k++) {
        for(size_t j=1; j <= universeSize; j++) {
            universe[getIndex(k,j)] = next[getIndex(k,j)];
        }
    }
    update();
    generations--;
    if(generations == 0) {
        stopGame();
        gameEnds(true);
        QMessageBox::information(this,
                                 tr("Game finished."),
                                 tr("Iterations finished."),
                                 QMessageBox::Ok,
                                 QMessageBox::Cancel);
    }
}

void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    paintGrid(p);
    paintUniverse(p);
}

void GameWidget::mousePressEvent(QMouseEvent *e)
{
    emit environmentChanged(true);
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;
    universe[getIndex(k,j)] = 1 - universe[getIndex(k,j)];
    update();
}

void GameWidget::mouseMoveEvent(QMouseEvent *e)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;
    int currentLocation = getIndex(k,j);
    if(!universe[currentLocation]){                //if current cell is empty,fill in it
        universe [currentLocation]= 1 - universe[currentLocation];
        update();
    }
}

void GameWidget::paintGrid(QPainter &p)
{
    QRect borders(0, 0, width()-1, height()-1); // borders of the universe
    QColor gridColor = m_masterColor; // color of the grid
    gridColor.setAlpha(10); // must be lighter than main color
    p.setPen(gridColor);
    double cellWidth = (double)width()/universeSize; // width of the widget / number of cells at one row
    for(double k = cellWidth; k <= width(); k += cellWidth)
        p.drawLine(k, 0, k, height());
    double cellHeight = (double)height()/universeSize; // height of the widget / number of cells at one row
    for(double k = cellHeight; k <= height(); k += cellHeight)
        p.drawLine(0, k, width(), k);
    p.drawRect(borders);
}

void GameWidget::paintUniverse(QPainter &p)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    for(int k=1; k <= universeSize; k++) {
        for(int j=1; j <= universeSize; j++) {
            if(universe[getIndex(k,j)] == 1) { // if there is any sense to paint it
                qreal left = (qreal)(cellWidth*j-cellWidth); // margin from left
                qreal top  = (qreal)(cellHeight*k-cellHeight); // margin from top
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                p.fillRect(r, QBrush(m_masterColor)); // fill cell with brush of main color
            }
        }
    }
}

QColor GameWidget::masterColor()
{
    return m_masterColor;
}

void GameWidget::setMasterColor(const QColor &color)
{
    m_masterColor = color;
    update();
}
