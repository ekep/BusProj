#include "backend.h"
#include <QGuiApplication>
#include <QQuickItem>
#include <QQuickView>


int CoordinateRole = Qt::UserRole + 1000;

QVector<QGeoCoordinate> decode(const std::string& encoded) {
  size_t i = 0;     // what byte are we looking at

  // Handy lambda to turn a few bytes of an encoded string into an integer
  auto deserialize = [&encoded, &i](const int previous) {
    // Grab each 5 bits and mask it in where it belongs using the shift
    int byte, shift = 0, result = 0;
    do {
      byte = static_cast<int>(encoded[i++]) - 63;
      result |= (byte & 0x1f) << shift;
      shift += 5;
    } while (byte >= 0x20);
    // Undo the left shift from above or the bit flipping and add to previous
    // since its an offset
    return previous + (result & 1 ? ~(result >> 1) : (result >> 1));
  };

  // Iterate over all characters in the encoded string
  QVector<QGeoCoordinate> shape;
  int last_lon = 0, last_lat = 0;
  while (i < encoded.length()) {
    // Decode the coordinates, lat first for some reason
    double lat = deserialize(last_lat);
    double lon = deserialize(last_lon);

    // Shift the decimal point 5 places to the left
    shape.append(QGeoCoordinate(lat / 1E5, lon / 1E5));
    // Remember the last one we encountered
    last_lon = lon;
    last_lat = lat;
  }
  return shape;
}

BackEnd::BackEnd(QObject *parent)
{
    QFile f1("stops.txt");
    f1.open(QIODevice::ReadOnly);
    QTextStream s1(&f1);
    while (!s1.atEnd()){
      QString s=s1.readLine(); // reads line from file
      Stop stop;
      stop.stop_id = s.split(",").at(0);
      stop.stop_num = s.split(",").at(1).toInt();
      stop.name = s.split(",").at(2);
      stop.lat = s.split(",").at(4).toDouble();
      stop.lon = s.split(",").at(5).toDouble();
      stops.push_back(stop);
    }
    f1.close();
    QVector<QGeoCoordinate> path = decode("_pnkKojwzCD@[hB[rCShAQp@a@pAq@pAeAzA}@`Aq@l@kA|@eBpAyEnCoE`BoLbD_c@hGqBd@{Cp@kEbAmBl@eFbCqG~C{IrEeJ|EuDZ}BAoDk@cFmDu@[w@Fo@DM?iAVq@p@S\\EAD@]j@o@|Au@vBs@jBkAhBmAdA_AbAU?S?kG@wDMaH_AiMyBkMeCqASyCm@oF}@aBq@o@WmBq@iB}Au@q@i@{@]g@NKOJiCyDq@{@y@mAW]MOi@_@e@Kc@A[HYR{@tAMPoAxBwAtBgBfBu@t@MJKKJJkAdAk@h@{A`AaBf@uATw@BAW@Vc@@]@_BCcBUy@SwAg@c@I_CeAGCa@GSE?t@DlCRhGB~@Bv@D~AB|@Bh@@`AVpJH`EBnARdJG?F?FhCJjFDpCD`F?~@Ah@EdFMfEMfBQrCW~BE?D?SlBOhAMx@ETMz@LNNVrAxBR^pCxEPZV`@~BzDDFMHLIt@lAT^Xd@t@nAjChEPZBDxC|ELTeAjHy@xEy@pDENG^_@rBIAH@}@xEIb@g@hCm@fCIdAQh@Md@{@zEKv@Kj@wA~HId@Gb@cAtFKA");
    QGeoPath Gpath;
    for (int i=0; i<path.size(); ++i)
      Gpath.addCoordinate(path[i]);
    setGeoPath(Gpath);
}


QString BackEnd::processRoute(QJsonObject route) {
  //qDebug() << "Route";
  QString res = "";
  if(route.contains("shortName") && route["shortName"].isString()) {
    res = route["shortName"].toString();
  }
  return res;
}

QString BackEnd::processTrip(QJsonObject trip) {
  //qDebug() << "Trip";
  QString res = "";
  if(trip.contains("route") && trip["route"].isObject()) {
    res = processRoute(trip["route"].toObject());
  }
  return res;
}

void BackEnd::OpenPopup()
{
  QMetaObject::invokeMethod(engine->rootObjects().at(0), "openDrawer");
}

void BackEnd::ClosePopup()
{
  QMetaObject::invokeMethod(engine->rootObjects().at(0), "closeDrawer");
}

void BackEnd::processStoptimesWithoutPatterns(QJsonArray stoptimesWithoutPatterns) {
  line1b->setProperty("text", "");
  QString lines = "";
  QString bus = "";
  for (int Index = 0; Index < stoptimesWithoutPatterns.size(); Index++) {
    QJsonObject times = stoptimesWithoutPatterns[Index].toObject();
    if (times.contains("realtimeArrival") && times["realtimeArrival"].isDouble()) {
      int time = times["realtimeArrival"].toInt();
      QTime t = QTime::fromMSecsSinceStartOfDay(time*1000);
      qDebug() << "Time: " << t.toString("H:mm"); //time/3600 << ":" << (time-(time/3600)*3600)/60;
      if(times.contains("trip") && times["trip"].isObject()) {
        bus = processTrip(times["trip"].toObject());
      }
      lines = lines + bus + "\t" + t.toString("H:mm") + "\n";
      line1b->setProperty("text", lines);
    }
  }
}

void BackEnd::processStop(QJsonObject stop) {
  QString linenum;
  if(stop.contains("name") && stop["name"].isString()) {
    qDebug() << "Name: " << stop["name"].toString();
    line1a->setProperty("text", stop["name"].toVariant());
    qDebug() << line1a->property("text").toString();
  }
  if(stop.contains("stoptimesWithoutPatterns") && stop["stoptimesWithoutPatterns"].isArray()) {
    QJsonArray arr = stop["stoptimesWithoutPatterns"].toArray();
    processStoptimesWithoutPatterns(arr);
  }
}

void BackEnd::processPlace(QJsonObject place) {
  if(place.contains("code") && place["code"].isString()) {
    qDebug() << place["code"].toString();
  }
  if(place.contains("gtfsId") && place["gtfsId"].isString()) {
    qDebug() << place["gtfsId"].toString();
  }
  if(place.contains("name") && place["name"].isString()) {
    qDebug() << place["name"].toString();
  }
}

void BackEnd::processNode(QJsonObject node) {
  if(node.contains("distance") && node["distance"].isDouble()) {
    qDebug() << "distance";
  }
  if(node.contains("place") && node["place"].isObject()) {
    qDebug() << "place";
    processPlace(node["place"].toObject());
  }
}

void BackEnd::processEdges(QJsonArray edges) {
  for(int i=0;i < edges.size(); i++) {
    QJsonObject nodeObject = edges[i].toObject();
    if(nodeObject.contains("node") && nodeObject["node"].isObject()) {
      qDebug() << "node";
      processNode(nodeObject["node"].toObject());
    }
  }
}

void BackEnd::processNearest(QJsonObject nearest) {
  if(nearest.contains("edges") && nearest["edges"].isArray()) {
    qDebug() << "edges";
    processEdges(nearest["edges"].toArray());
  }
}

void BackEnd::processData(QJsonObject data) {
  if(data.contains("stop") && data["stop"].isObject()) {
    qDebug() << "stop";
    processStop(data["stop"].toObject());
  }
  if(data.contains("nearest") && data["nearest"].isObject()) {
    qDebug() << "nearest";
    processNearest(data["nearest"].toObject());
  }
}

QString makeFindNearestStopRequest(QGeoCoordinate coord)
{
  QString lat = QString::fromStdString(std::to_string(coord.latitude()));
  QString lon = QString::fromStdString(std::to_string(coord.longitude()));
  QString result = "{ nearest(lat: " + lat + ", lon: " + lon + ", maxResults: 1, maxDistance: 1500, filterByPlaceTypes: [STOP]) {"
              "edges { "
                "node { "
                    "place { "
                      "lat "
                      "lon "
                      "...on Stop { "
                        "name "
                        "gtfsId "
                        "code "
                      "} "
                    "} "
                    "distance "
                "} "
              "} "
            "} "
          "} ";
  return result;
}

QString makeStopIdRequest(QString stop_name)
{
  QString result = "{ stops(name: \"" + stop_name + "\") {"
              "gtfsId "
              "name "
              "code "
              "lat "
              "lon "
            "} "
          "} ";
  return result;
}

QString makeStopRequestMsg(QString stop_id)
{
  QString result = "{ stop(id: \"" + stop_id + "\") {"
              "name "
              "stoptimesWithoutPatterns { "
                  "trip { "
                    "route { "
                      "shortName "
                      "longName "
                    "} "
                  "} "
                  "scheduledArrival "
                  "realtimeArrival "
                  "arrivalDelay "
                  "scheduledDeparture "
                  "realtimeDeparture "
                  "departureDelay "
                  "realtime "
                  "realtimeState "
                  "serviceDay "
                  "headsign "
              "} "
           "} "
        "}";
  return result;
}

void BackEnd::postRequest(QGeoCoordinate coord)
{
  double min_dist = 9999;
  QString id = "";
  for(int i=0; i < stops.size(); ++i) {
//    if(distance(coord.latitude(), coord.longitude(), stops[i].lat, stops[i].lon) < min_dist) {
    if(coord.distanceTo(QGeoCoordinate(stops[i].lat, stops[i].lon)) < min_dist) {
      min_dist = coord.distanceTo(QGeoCoordinate(stops[i].lat, stops[i].lon));
      id = "OULU:" + stops[i].stop_id;
      curloc->setProperty("center", QVariant::fromValue(QGeoCoordinate(stops[i].lat, stops[i].lon)));
    }
  }

  //qDebug() << id;
  if(id != "") {
    QByteArray arr;

    QNetworkAccessManager * manager = new QNetworkAccessManager(this);

    //QUrl service("https://api.oulunliikenne.fi/proxy/graphql");
    QUrl service("https://api.digitransit.fi/routing/v1/routers/waltti/index/graphql");

    QUrlQuery urlQuery("https://api.oulunliikenne.fi/proxy/graphql");
    QNetworkRequest request(service);

    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/graphql");
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onFinish(QNetworkReply *)));

    arr = makeStopRequestMsg(id).toUtf8();
    //qDebug() << "createTask: POSTING..." << arr;

    manager->post(request, arr);
  }
}

void BackEnd::parseResponce(QByteArray *resp)
{
  QJsonDocument doc = QJsonDocument::fromJson(*resp);

  QJsonObject obj = doc.object();

  if(obj.contains("data") && obj["data"].isObject())
  {
    processData(obj["data"].toObject());
  } else {
    qDebug() << "Invalid responce.";
  }
  OpenPopup();
  emit geopathChanged();
}

void BackEnd::onFinish(QNetworkReply *rep)
{
  QByteArray bytes = rep->readAll(); // bytes
  //QString str(bytes);
  //qDebug() << "Reply " << str << "\n";
  parseResponce(&bytes);
}

