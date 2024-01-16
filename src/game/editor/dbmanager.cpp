#include <QDebug>
#include <QSqlQuery>
#include "dbmanager.h"

#include "../helpers/h_memory.h"
#include "../helpers/h_coordinate.h"
#include "../game_map.h"

void DbManager::open(const QString& path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        qDebug() << "Error: connection with database failed";
    } else {
        qDebug() << "Database: connection ok";
    }

    QSqlQuery query;
    query.exec("drop table if exists layer");
    query.clear();
    query.exec("create table layer (" \
                            "name string primary key)");
    query.clear();
    // TILESET
    query.exec("drop table if exists tileset");
    query.clear();
    query.exec("create table tileset (" \
                    "id   integer primary key autoincrement," \
                    "name string  not null)");// TILE
    query.clear();
    query.exec("drop table if exists tile");
    query.clear();
    query.exec("create table tile (" \
                    "id         integer primary key autoincrement," \
                    "tilesetid  integer not null," \
                    "name       string  not null," \
                    "blocksight integer not null," \
                    "blockmove  integer not null," \
                    "layerid    integer not null)");
//                    "foreign key(layerid) references layer(id)," \
//                    "foreign key(tilesetid) references tileset(id))");
    query.clear();
// MAP
    query.exec("drop table if exists map");
    query.clear();
    query.exec("create table map (" \
                    "id           integer primary key autoincrement," \
                    "name         string  not null," \
                    "wraparound   integer not null," \
                    "width        integer not null," \
                    "height       integer not null," \
                    "playerstartx integer not null," \
                    "playerstarty integer not null)");
    query.clear();
    // MAPDATA (i.e. placed map tiles)
    query.exec("drop table if exists mapdata");
    query.clear();
    query.exec("create table mapdata (" \
                    "id      integer primary key autoincrement," \
                    "mapid   integer not null," \
                    "x       integer not null," \
                    "y       integer not null," \
                    "tileid  integer not null," \
                    "layerid integer not null)");
//                    "foreign key(layerid) references layer(id)," \
//                    "foreign key(tileid) references tile(id)," \
//                    "foreign key(mapid) references map(id))");
    query.clear();

    query.prepare("insert into map(name,wraparound,width,height,playerstartx,playerstarty)" \
                    "values(:name,:wraparound,:width,:height,:playerstartx,:playerstarty)");
    query.bindValue(":name", "MyMap01");
    query.bindValue(":wraparound", false);
    query.bindValue(":width", "default");
    query.bindValue(":height", "default");
    query.bindValue(":playerstartx", 10);
    query.bindValue(":playerstarty", 10);
    query.exec();
    query.clear();

    int floor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 6, 6, 6, 0, 5, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 6,
                   6, 6, 6, 6, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6};
    int shadow[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                    7, 7, 7, 7, 7, 7, 7, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0,
                    0, 9, 0, 0, 0, 0, 0, 8, 7, 7, 7, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0,
                    9, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0,
                    0, 0, 11, 7, 7, 12, 11, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int wall[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 13, 14, 14, 14, 14, 15, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0,
                  0, 0, 0, 16, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 16, 0, 0,
                  0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 17, 14, 4, 0, 18, 19, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                  4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

    query.exec();
    int i = 0;
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 20; x++) {
            query.prepare("insert into mapdata(mapid,x,y,tileid,layerid) " \
                        "values(:mapid,:x,:y,:tileid,:layerid)");
            query.bindValue(":mapid", 1);
            query.bindValue(":x", x);
            query.bindValue(":y", y);
            query.bindValue(":tileid", floor[i]);
            query.bindValue(":layerid", 0);
            query.exec();
            query.clear();

            query.prepare("insert into mapdata(mapid,x,y,tileid,layerid) " \
                        "values(:mapid,:x,:y,:tileid,:layerid)");
            query.bindValue(":mapid", 1);
            query.bindValue(":x", x);
            query.bindValue(":y", y);
            query.bindValue(":tileid", shadow[i]);
            query.bindValue(":layerid", 1);
            query.exec();
            query.clear();

            query.prepare("insert into mapdata(mapid,x,y,tileid,layerid) " \
                        "values(:mapid,:x,:y,:tileid,:layerid)");
            query.bindValue(":mapid", 1);
            query.bindValue(":x", x);
            query.bindValue(":y", y);
            query.bindValue(":tileid", wall[i]);
            query.bindValue(":layerid", 2);
            query.exec();
            query.clear();
            i++;
        }
    }

}

struct GameMap* DbManager::loadMap() {
    struct HCoordinate size = { 20, 20 };
    struct HCoordinate playerStart = { 10, 10};
    int* floorLayer = (int*)GMalloc(sizeof(int) * 20 * 20, "GameMapManager_floorLayer");
    int* shadowLayer = (int*)GMalloc(sizeof(int) * 20 * 20, "GameMapManager_shadowLayer");
    int* wallLayer = (int*)GMalloc(sizeof(int) * 20 * 20, "GameMapManager_wallLayer");

    QSqlQuery query;
    query.prepare("select tileid from mapdata where mapid=:mapid and layerid=:layerid order by y asc, x asc");
    query.bindValue(":mapid", 1);
    query.bindValue(":layerid", 0);
    query.exec();
    int i = 0;
    while (query.next()) {
        floorLayer[i++] = query.value(0).toInt();
    }
    query.clear();

    query.prepare("select tileid from mapdata where mapid=:mapid and layerid=:layerid order by y asc, x asc");
    query.bindValue(":mapid", 1);
    query.bindValue(":layerid", 1);
    query.exec();
    i = 0;
    while (query.next()) {
        shadowLayer[i++] = query.value(0).toInt();
    }
    query.clear();

    query.prepare("select tileid from mapdata where mapid=:mapid and layerid=:layerid order by y asc, x asc");
    query.bindValue(":mapid", 1);
    query.bindValue(":layerid", 2);
    query.exec();
    i = 0;
    while (query.next()) {
        wallLayer[i++] = query.value(0).toInt();
    }
    query.clear();

    struct GameMap* map = GameMap_new(0, false, "foo", size, playerStart, floorLayer, shadowLayer, wallLayer);
    GFree(floorLayer, "GameMapManager_floorLayer");
    GFree(shadowLayer, "GameMapManager_shadowLayer");
    GFree(wallLayer, "GameMapManager_wallLayer");
    return map;
}

void DbManager::close() {
    m_db.close();
    qDebug() << "Database closed";
}