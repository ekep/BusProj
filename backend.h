#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <qqml.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QtNetwork/qhttpmultipart.h>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QtPositioning>
#include <QList>
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>


class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoPath geopath READ geoPath WRITE setGeoPath NOTIFY geopathChanged)

  private:
    struct Stop {
        QString stop_id = 0;
        int stop_num;
        QString name;
        double lon;
        double lat;
    };
    QList<Stop> stops;
  public:
    BackEnd(QObject *parent = nullptr);
    Q_INVOKABLE bool postMessage(const QGeoCoordinate &p) {
        postRequest(p);
        return true;
        }
    void updatePath(){
        QTimer *timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, [this](){
            mGeoPath.translate(0.001, -0.01);
            emit geopathChanged();
        });
        timer->start(1000);
    }
    QGeoPath geoPath() const{return mGeoPath;}
    void setGeoPath(const QGeoPath &geoPath){
        if(geoPath == mGeoPath)
            return;
        mGeoPath = geoPath;
        emit geopathChanged();
    }
    void parseResponce(QByteArray *resp);
    QString processRoute(QJsonObject route);
    QString processTrip(QJsonObject trip);
    void OpenPopup();
    void ClosePopup();
    void processStoptimesWithoutPatterns(QJsonArray stoptimesWithoutPatterns);
    void processStop(QJsonObject stop);
    void processPlace(QJsonObject place);
    void processNode(QJsonObject node);
    void processEdges(QJsonArray edges);
    void processNearest(QJsonObject nearest);
    void processData(QJsonObject data);
    void postRequest(QGeoCoordinate coord);
    QObject *line1a;
    QObject *line1b;
    QObject *drawer;
    QObject *curloc;
    QObject *mapview;

    QQmlApplicationEngine *engine;
  public slots:
    void onFinish(QNetworkReply *rep);
  private:
    QGeoPath mGeoPath;

  signals:
    void geopathChanged();
    void setTextField(QGeoPath path);

};

#endif // BACKEND_H
