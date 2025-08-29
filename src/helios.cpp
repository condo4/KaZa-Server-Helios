#include "helios.h"
#include <QVariant>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QModbusTcpClient>

const int TCP_PORT = 80;
const int MODBUS_TCP_PORT = 502;
const int KWL_UNIT_ID = 180;
const int SOCKET_TIMEOUT = 10000; // 10 seconds in milliseconds

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

    bool setVariable(const QString& varname, const QVariant& varval, const std::function<QString(const QVariant&)>& conversion = nullptr)
    {
        qDebug() << "HELIOS: setVariable " << varname << " to " << varval;

        QString varcontent;
        if (conversion) {
            varcontent = conversion(varval);
        } else {
            varcontent = varval.toString();
        }
        QString vardef = QString("%1=%2").arg(varname, varcontent);
        QByteArray data = vardef.toUtf8();
        int datalen = (data.size() + 2) / 2 * 2;
        data.resize(datalen);

        QModbusTcpClient modbusClient;
        modbusClient.setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_hostname);
        modbusClient.setConnectionParameter(QModbusDevice::NetworkPortParameter, MODBUS_TCP_PORT);
        modbusClient.setTimeout(SOCKET_TIMEOUT);
        modbusClient.setNumberOfRetries(0);

        if (!modbusClient.connectDevice()) {
            qDebug() << "Failed to connect to device:" << modbusClient.errorString();
            return false;
        }

        while( modbusClient.state() != QModbusDevice::ConnectedState)
        {
            QEventLoop loop;
            QObject::connect(&modbusClient, &QModbusDevice::stateChanged, &loop, &QEventLoop::quit);
            loop.exec();
        }

        QModbusDataUnit writeUnit(QModbusDataUnit::RegisterType::HoldingRegisters, 1, datalen / 2);
        for (int i = 0; i < datalen; i += 2) {
            quint16 value = (static_cast<quint8>(data[i]) << 8) | static_cast<quint8>(data[i + 1]);
            writeUnit.setValue(i / 2, value);
        }

        QModbusReply* reply = modbusClient.sendWriteRequest(writeUnit, KWL_UNIT_ID);
        if (!reply) {
            qDebug() << "Failed to send write request:" << modbusClient.errorString();
            return false;
        }

        if (!reply->isFinished()) {
            QEventLoop loop;
            QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }

        if (reply->error() != QModbusDevice::NoError) {
            qDebug() << "Write error:" << reply->errorString();
            reply->deleteLater();
            return false;
        }

        reply->deleteLater();
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
    qDebug() << "Helios integration loaded";
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
    // Autentication
    QEventLoop loop;
    QNetworkRequest request(QUrl(QString("http://%1:%2/info.htm").arg(d->m_hostname).arg(d->m_port)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
    QByteArray postData = "v00402=helios";
    QNetworkReply *reply = d->m_manager.post(request, postData);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    reply->deleteLater();

    // Data request werte8
    request.setUrl(QUrl(QString("http://%1:%2/data/werte8.xml").arg(d->m_hostname).arg(d->m_port)));
    postData = QString("xml=/data/werte8.xml").toUtf8();
    reply = d->m_manager.post(request, postData);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    reply->deleteLater();
    _parseWerte("werte8", reply->readAll());

    // Data request werte3
    request.setUrl(QUrl(QString("http://%1:%2/data/werte3.xml").arg(d->m_hostname).arg(d->m_port)));
    postData = QString("xml=/data/werte3.xml").toUtf8();
    reply = d->m_manager.post(request, postData);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    reply->deleteLater();
    _parseWerte("werte3", reply->readAll());
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
