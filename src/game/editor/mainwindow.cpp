#include <QWidget>
#include <QMainWindow>
#include <QDebug>
#include <QElapsedTimer>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPoint>
#include <QPainter>
#include "mainwindow.h"
#include "../game_container.h"
#include "../helpers/h_memory.h"
#include "../config.h"
#include "dbmanager.h"

const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 240;
const int FPS = 30;
const int DELAY = 1000 / FPS;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // init UI
    ui->setupUi(this);
    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, WINDOW_WIDTH * gfxScaleFactor, WINDOW_HEIGHT * gfxScaleFactor);
    ui->graphicsView->setScene(scene);
    setWindowTitle("cmoria Editor (qt)");
    framebuffer = QImage(QSize(WINDOW_WIDTH, WINDOW_HEIGHT), QImage::Format_ARGB32);
    framebufferScaled = QImage(QSize(WINDOW_WIDTH * gfxScaleFactor, WINDOW_HEIGHT * gfxScaleFactor), QImage::Format_ARGB32);
    // init database
    DbManager db = DbManager::getInstance();
    db.open("../../../database.sqlite");
}

void MainWindow::showEvent(QShowEvent*) {
    static bool firstStart = false;
    if (!firstStart) {
        GameContainer_init();
        timerId = startTimer(DELAY, Qt::PreciseTimer);
        firstStart = true;
    }
}

void MainWindow::closeEvent(QCloseEvent*) {
    GameContainer_delete();
    DbManager db = DbManager::getInstance();
    db.close();
    printf("memory ref count: %li\n", getMemCount());
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    inputEvent = NO_EVENT;
    if(key == 65) inputEvent = KEY_LEFT;
    else if(key == 68) inputEvent = KEY_RIGHT;
    else if(key == 87) inputEvent = KEY_UP;
    else if(key == 83) inputEvent = KEY_DOWN;
    else if(key == 78) inputEvent = KEY_B;
    else if(key == 77) inputEvent = KEY_A;
}

void MainWindow::drawImage(const EngineBitmap* bitmap, int x, int y) {
    const QImage* image = (QImage*)EngineBitmap_getBitmap(bitmap);
    QPainter painter(&framebuffer);
    painter.drawImage(x, y, *image);
}

void MainWindow::drawRect(int x1, int y1, int width, int height) {
    QPainter painter(&framebuffer);
    painter.setPen(QColor(255, 255, 255));
    painter.drawRect(x1, y1, width - 1, height - 1);
}

void MainWindow::clearFrame() {
    QPainter painter(&framebuffer);
    painter.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Qt::black);
}

void MainWindow::timerEvent(QTimerEvent*) {
    // stop timer
    killTimer(timerId);
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    // call Game's update event
    bool flip = true;
    flip = GameContainer_update(inputEvent);
    inputEvent = NO_EVENT; // clear input event
    // update framebuffer (flip)
    if (flip) {
        framebufferScaled = framebuffer.scaled(WINDOW_WIDTH * gfxScaleFactor, WINDOW_HEIGHT * gfxScaleFactor, Qt::KeepAspectRatio);
        QPixmap out_pix = QPixmap::fromImage(framebufferScaled);
        if (out_pix_item != nullptr)
            delete out_pix_item;
        out_pix_item = new QGraphicsPixmapItem(out_pix);
        scene->clear();
        scene->addItem(out_pix_item);
    }
    // restart timer
    int delay = DELAY - elapsedTimer.elapsed();
    timerId = startTimer(delay < 0 ? 0 : delay, Qt::PreciseTimer);
}

MainWindow::~MainWindow() {

}