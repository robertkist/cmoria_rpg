#pragma once
#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include "../engine.h"
#include "../../../src/game/engine_bitmap.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    int timerId;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* out_pix_item = nullptr;
    EngineInputEvent inputEvent = NO_EVENT;
    QImage framebuffer;
    QImage framebufferScaled;
public:
    MainWindow(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent* event);
    void timerEvent(QTimerEvent* event);
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent*);
    void drawImage(const EngineBitmap* bitmap, int x, int y);
    void drawRect(int x1, int y1, int width, int height);
    void clearFrame();
    ~MainWindow();
};