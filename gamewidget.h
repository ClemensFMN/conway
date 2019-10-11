#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QColor>
#include <QWidget>

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

signals:
    //when one of the cell has been changed,emit this signal to lock the universeSize
    void environmentChanged(bool ok);
    //when game is over or clear is called,emit it to unlock the universeSize
    void gameEnds(bool ok);

public slots:
    void startGame(const int &number = -1); // start
    void stopGame(); // finish
    void clear(); // clear
    void fillRand();

    size_t cellNumber(); // number of the cells in one row
    void setCellNumber(const size_t &s); // set number of the cells in one row

    int interval(); // interval between generations
    void setInterval(int msec); // set interval between generations

    QColor masterColor(); // color of the cells
    void setMasterColor(const QColor &color); // set color of the cells

    QString dump(); // dump of current universe
    void setDump(const QString &data); // set current universe from it's dump

private slots:
    void paintGrid(QPainter &p);
    void paintUniverse(QPainter &p);
    void newGeneration();

private:
    QColor m_masterColor;
    QTimer* timer;
    int generations;
    int* universe; // map
    int* next; // map
    size_t universeSize;
    int isAlive(size_t k, size_t j); // return true if universe[k][j] accept rules
    void resetUniverse();// reset the size of universe
    size_t getIndex(size_t k, size_t l);
};

#endif // GAMEWIDGET_H
