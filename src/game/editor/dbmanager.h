#pragma once

#include <QString>
#include <QSqlDatabase>

class DbManager {
public:
    static DbManager &getInstance(){
        static DbManager instance;
        return instance;
    };
    void open(const QString& path);
    struct GameMap* loadMap();
    void close();
private:
    QSqlDatabase m_db;
};
