#include "helios.h"
#include <QVariant>
#include <QTimer>
#include <QNetworkAccessManager>


const int TCP_PORT = 80;

#define REFRESH_ITEM(i, n, t) if(currentId == #i) \
{ \
    if(( res.to ## t () ) != ( d->m_##n )) \
    { \
        d->m_##n = res.to ## t (); \
        emit n ## Changed(); \
    } \
}

class HeliosPrivate {
    Q_DISABLE_COPY(HeliosPrivate)
    Q_DECLARE_PUBLIC(Helios)
    Helios * const q_ptr;

    QMap<QString, QPair<QDateTime, QMap<QString, QString>>> m_werte;
    QNetworkAccessManager m_manager;

    QTimer m_timer;
    QString m_hostname;
    int m_port {TCP_PORT};

    int m_forceMode;
    int m_standbyMode;
    int m_operatingMode;
    int m_fanStage;
    int m_fanPercentage;
    int m_rpmSupply;
    int m_rpmExtract;
    int m_byPass;
    float m_humidity;
    float m_inOutsideTemperature;
    float m_outOutsideTemperature;
    float m_inInsideTemperature;
    float m_outInsideTemperature;
    int m_refreshRate;
    int m_forceModeSpeed;
    int m_forceModeRemaning;
    int m_forceModeTime;

    HeliosPrivate(Helios * helios):
    q_ptr(helios) {}

public:
    bool setVariable(const QString& varname, const QVariant& varval, const std::function<QString(const QVariant&)>& conversion)
    {
        QTcpSocket socket;
        socket.connectToHost("helios", 80);
        socket.waitForConnected();
        QString postData = "v00402=helios";
        QByteArray postDataBytes = postData.toUtf8();
        QString httpRequest;
        httpRequest += "POST /info.htm HTTP/1.1\r\n";
        httpRequest += "Host: helios\r\n";
        httpRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
        httpRequest += QString("Content-Length: %1\r\n").arg(postDataBytes.length());
        httpRequest += "Connection: Keep-Alive\r\n";
        httpRequest += "\r\n";
        httpRequest += postData;
        socket.write(httpRequest.toUtf8());
        socket.flush();
        QByteArray res;
        while(!res.contains("</html>"))
        {
            socket.waitForReadyRead();
            res.append(socket.readAll());
        }


        // Send Data
        socket.waitForDisconnected();
        socket.connectToHost("helios", 80);
        socket.waitForConnected();

        QString varcontent;
        if (conversion) {
            varcontent = conversion(varval);
        } else {
            varcontent = varval.toString();
        }

        postData = varname + "=" + varcontent;
        postDataBytes = postData.toUtf8();
        res.clear();
        httpRequest.clear();
        httpRequest += "POST /info.htm HTTP/1.1\r\n";
        httpRequest += "Host: helios\r\n";
        httpRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
        httpRequest += QString("Content-Length: %1\r\n").arg(postDataBytes.length());
        httpRequest += "Connection: Keep-Alive\r\n";
        httpRequest += "\r\n";
        httpRequest += postData;

        socket.write(httpRequest.toUtf8());
        socket.flush();

        while(!res.contains("</html>"))
        {
            socket.waitForReadyRead();
            res.append(socket.readAll());
        }

        return true;
    }

    friend class Helios;
};

Helios::Helios(QObject *parent)
: QObject(parent)
, d_ptr(new HeliosPrivate(this))
{
    Q_D(Helios);
    QObject::connect(&d->m_timer,        &QTimer::timeout,                this, &Helios::refresh);
    qDebug() << "Helios integration loaded ...";
}


Helios::~Helios() {

}


QString Helios::hostname() const
{
    Q_D(const Helios);
    return d->m_hostname;
}

void Helios::setHostname(const QString &newHostname)
{
    Q_D(Helios);
    int port  = d->m_port;
    QString hostname = newHostname;
    if(hostname.contains(":"))
    {
        QStringList h = hostname.split(":");
        hostname = h[0];
        port = h[1].toInt();
    }

    if (d->m_hostname == hostname && d->m_port == port)
        return;
    d->m_hostname = hostname;
    d->m_port = port;
    refresh();
    emit hostnameChanged();
}

bool Helios::connected() const
{
    Q_D(const Helios);
    return true;
}


int Helios::forceMode() const
{
    Q_D(const Helios);
    return d->m_forceMode;
}


int Helios::operatingMode() const
{
    Q_D(const Helios);
    return d->m_operatingMode;
}

void Helios::setOperatingMode(int newOperatingMode)
{
    Q_D(Helios);
    qDebug() << "HELIOS: Set Operating mode " << newOperatingMode;
    d->setVariable("v00101", newOperatingMode, [](const QVariant& val) { return QString::number(val.toInt()); });

    emit operatingModeChanged();
}

int Helios::fanStage() const
{
    Q_D(const Helios);
    return d->m_fanStage;
}

void Helios::setFanStage(int newFanStage)
{
    Q_D(Helios);
    qDebug() << "HELIOS: Set Fan " << newFanStage;
    d->setVariable("v00102", newFanStage, [](const QVariant& val) { return QString::number(val.toInt()); });

    emit fanStageChanged();
}

void Helios::setForceModeSpeed(int newModeSpeed)
{
    Q_D(Helios);
    qDebug() << "HELIOS: Set ForceModeSpeed " << newModeSpeed;
    d->setVariable("v00092", newModeSpeed, [](const QVariant& val) { return QString::number(val.toInt()); });

    emit forceModeSpeedChanged();
}

void Helios::setForceTime(int newForceTime)
{
    Q_D(Helios);
    qDebug() << "HELIOS: Set ForceTime " << newForceTime;
    d->setVariable("v00091", newForceTime, [](const QVariant& val) { return QString::number(val.toInt()); });

    emit forceModeSpeedChanged();
}

void Helios::setForceMode(int newForceMode)
{
    Q_D(Helios);
    qDebug() << "HELIOS: Set ForceMode " << newForceMode;
    d->setVariable("v00094", newForceMode, [](const QVariant& val) { return QString::number(val.toInt()); });

    emit forceModeSpeedChanged();
}


void Helios::refresh()
{
    Q_D(Helios);
    QTcpSocket socket;
    socket.connectToHost("helios", 80);
    socket.waitForConnected();
    QString postData = "v00402=helios";
    QByteArray postDataBytes = postData.toUtf8();
    QString httpRequest;
    httpRequest += "POST /info.htm HTTP/1.1\r\n";
    httpRequest += "Host: helios\r\n";
    httpRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
    httpRequest += QString("Content-Length: %1\r\n").arg(postDataBytes.length());
    httpRequest += "Connection: Keep-Alive\r\n";
    httpRequest += "\r\n";
    httpRequest += postData;
    socket.write(httpRequest.toUtf8());
    socket.flush();
    QByteArray res;
    while(!res.contains("</html>"))
    {
        socket.waitForReadyRead();
        res.append(socket.readAll());
    }

    // Data request werte3
    if (socket.state() != QAbstractSocket::UnconnectedState) {
        socket.disconnectFromHost();
        if (socket.state() != QAbstractSocket::UnconnectedState) {
            socket.waitForDisconnected(1000);
        }
    }
    socket.connectToHost("helios", 80);
    if (!socket.waitForConnected(5000)) { // timeout 5s
        qDebug() << "HELIOS: Connexion error:" << socket.errorString();
        return;
    }

    postData = "xml=/data/werte8.xml";
    postDataBytes = postData.toUtf8();
    res.clear();
    httpRequest.clear();
    httpRequest += "POST /data/werte8.xml HTTP/1.1\r\n";
    httpRequest += "Host: helios\r\n";
    httpRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
    httpRequest += QString("Content-Length: %1\r\n").arg(postDataBytes.length());
    httpRequest += "Connection: Keep-Alive\r\n";
    httpRequest += "\r\n";
    httpRequest += postData;

    socket.write(httpRequest.toUtf8());
    socket.flush();

    while(!res.contains("</PARAMETER>"))
    {
        if (!socket.waitForReadyRead(5000)) {
            qDebug() << "HELIOS: Read Timeout";
            break;
        }
        res.append(socket.readAll());
    }

    _parseWerte("werte8", res.mid(res.indexOf("<?xml")));

    // Data request werte3
    //qDebug() << "Start werte3" << socket.state();
    if (socket.state() != QAbstractSocket::UnconnectedState) {
        socket.disconnectFromHost();
        if (socket.state() != QAbstractSocket::UnconnectedState) {
            socket.waitForDisconnected(1000);
        }
    }
    socket.connectToHost("helios", 80);
    if (!socket.waitForConnected(5000)) { // timeout 5s
        qDebug() << "Erreur connexion:" << socket.errorString();
        return;
    }
    postData = "xml=/data/werte3.xml";
    postDataBytes = postData.toUtf8();
    res.clear();
    httpRequest.clear();
    httpRequest += "POST /data/werte3.xml HTTP/1.1\r\n";
    httpRequest += "Host: helios\r\n";
    httpRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
    httpRequest += QString("Content-Length: %1\r\n").arg(postDataBytes.length());
    httpRequest += "Connection: Keep-Alive\r\n";
    httpRequest += "\r\n";
    httpRequest += postData;

    socket.write(httpRequest.toUtf8());
    socket.flush();

    while(!res.contains("</PARAMETER>"))
    {
        if (!socket.waitForReadyRead(5000)) {
            qDebug() << "HELIOS: Read Timeout";
            break;
        }
        res.append(socket.readAll());
    }
    _parseWerte("werte3", res.mid(res.indexOf("<?xml")));

    if (socket.state() != QAbstractSocket::UnconnectedState) {
        socket.disconnectFromHost();
        if (socket.state() != QAbstractSocket::UnconnectedState) {
            socket.waitForDisconnected(1000);
        }
    }
}

void Helios::_parseWerte(const QString id, const QString &data)
{
    Q_D(Helios);
    QMap<QString, QString> map;
    QStringList lines = data.split('\n');
    QString currentId;
    QString res;

    for (const QString &line : lines)
    {
        if (line.length() > 9 && line.mid(1, 2) == "ID")
        {
            currentId = line.mid(4, line.length() - 9);
        }
        if (line.length() > 9 && line.mid(1, 2) == "VA")
        {
            res = line.mid(4, line.length() - 9);

            REFRESH_ITEM(v00094, forceMode,     Int);
            REFRESH_ITEM(v00101, operatingMode, Int);
            REFRESH_ITEM(v00102, fanStage,      Int);
            REFRESH_ITEM(v00103, fanPercentage, Int);
            REFRESH_ITEM(v00104, inOutsideTemperature,  Float);
            REFRESH_ITEM(v00107, inInsideTemperature,   Float);
            REFRESH_ITEM(v00106, outOutsideTemperature, Float);
            REFRESH_ITEM(v00105, outInsideTemperature,  Float);
            REFRESH_ITEM(v00348, rpmSupply, Int);
            REFRESH_ITEM(v00349, rpmExtract, Int);
            REFRESH_ITEM(v02119, byPass, Int);
            REFRESH_ITEM(v02136, humidity,  Float);

            REFRESH_ITEM(v00092, forceModeSpeed, Int);
            REFRESH_ITEM(v00093, forceModeRemaning, Int);
            REFRESH_ITEM(v00091, forceModeTime, Int);
        }

    }

}

int Helios::fanPercentage() const
{
    Q_D(const Helios);
    return d->m_fanPercentage;
}

int Helios::rpmSupply() const
{
    Q_D(const Helios);
    return d->m_rpmSupply;
}

int Helios::rpmExtract() const
{
    Q_D(const Helios);
    return d->m_rpmExtract;
}

int Helios::byPass() const
{
    Q_D(const Helios);
    return d->m_byPass;
}

float Helios::humidity() const
{
    Q_D(const Helios);
    return d->m_humidity;
}

float Helios::inOutsideTemperature() const
{
    Q_D(const Helios);
    return d->m_inOutsideTemperature;
}

float Helios::outOutsideTemperature() const
{
    Q_D(const Helios);
    return d->m_outOutsideTemperature;
}

float Helios::inInsideTemperature() const
{
    Q_D(const Helios);
    return d->m_inInsideTemperature;
}

float Helios::outInsideTemperature() const
{
    Q_D(const Helios);
    return d->m_outInsideTemperature;
}

int Helios::refreshRate() const
{
    Q_D(const Helios);
    return d->m_refreshRate;
}

void Helios::setRefreshRate(int newRefreshRate)
{
    Q_D(Helios);
    if (d->m_refreshRate == newRefreshRate)
        return;
    d->m_timer.setInterval(newRefreshRate * 1000);
    d->m_timer.setSingleShot(false);
    d->m_timer.start();
    d->m_refreshRate = newRefreshRate;
    emit refreshRateChanged();
}

int Helios::forceModeSpeed() const
{
    Q_D(const Helios);
    return d->m_forceModeSpeed;
}

int Helios::forceModeRemaning() const
{
    Q_D(const Helios);
    return d->m_forceModeRemaning;
}

int Helios::forceModeTime() const
{
    Q_D(const Helios);
    return d->m_forceModeTime;
}
