#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include "mainwindow.h"
#include "../engine.h"

void* api = nullptr;

int main(int argc, char* argv[]) {
#if defined(__APPLE__) && defined(__MACH__)
    QApplication::setDesktopSettingsAware(false);
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif
    QApplication a(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    a.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
    MainWindow w;
    api = (void*)&w;
    w.show();
    int r = a.exec();
    return r;
}