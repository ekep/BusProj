#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "main.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    BackEnd bckEnd;
    QQmlApplicationEngine engine;
    bckEnd.engine = &engine;


    engine.rootContext()->setContextProperty("backEnd", &bckEnd);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    bckEnd.drawer = engine.rootObjects().at(0)->findChild<QObject *>("drawer");
    bckEnd.line1a = engine.rootObjects().at(0)->findChild<QObject *>("line1a");
    bckEnd.line1b = engine.rootObjects().at(0)->findChild<QObject *>("line1b");
    bckEnd.curloc = engine.rootObjects().at(0)->findChild<QObject *>("curloc");
    bckEnd.mapview = engine.rootObjects().at(0)->findChild<QObject *>("mapview");
/*    if(bckEnd.line1a != NULL) {
      qDebug() << "Höö" << bckEnd.line1a->property("text").toString();
    }*/
    return app.exec();
}

