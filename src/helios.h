#ifndef HELIOS_H
#define HELIOS_H

#include <QtQml/qqmlregistration.h>
#include <QObject>

class HeliosPrivate;

class Helios : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hostname     READ hostname       WRITE setHostname       NOTIFY hostnameChanged      FINAL)
    Q_PROPERTY(int refreshRate      READ refreshRate    WRITE setRefreshRate    NOTIFY refreshRateChanged   FINAL)

    Q_PROPERTY(bool connected              READ connected               NOTIFY connectedChanged             FINAL)

    Q_PROPERTY(int forceMode               READ forceMode               NOTIFY forceModeChanged             FINAL) // device.ventilation.force_mode
    Q_PROPERTY(int forceModeSpeed          READ forceModeSpeed          NOTIFY forceModeSpeedChanged        FINAL) // device.ventilation.force_speed
    Q_PROPERTY(int forceModeRemaning       READ forceModeRemaning       NOTIFY forceModeRemaningChanged     FINAL) // device.ventilation.force_remaining
    Q_PROPERTY(int forceModeTime           READ forceModeTime           NOTIFY forceModeTimeChanged         FINAL) // device.ventilation.force_time
    Q_PROPERTY(int operatingMode           READ operatingMode           NOTIFY operatingModeChanged         FINAL) // device.ventilation.mode
    Q_PROPERTY(int fanStage                READ fanStage                NOTIFY fanStageChanged              FINAL) // device.ventilation.speed_id
    Q_PROPERTY(int fanPercentage           READ fanPercentage           NOTIFY fanPercentageChanged         FINAL) // device.ventilation.speed_relative
    Q_PROPERTY(int rpmSupply               READ rpmSupply               NOTIFY rpmSupplyChanged             FINAL) // device.ventilation.fan_in
    Q_PROPERTY(int rpmExtract              READ rpmExtract              NOTIFY rpmExtractChanged            FINAL) // device.ventilation.fan_out
    Q_PROPERTY(int byPass                  READ byPass                  NOTIFY byPassChanged                FINAL) // device.ventilation.by_pass
    Q_PROPERTY(float humidity              READ humidity                NOTIFY humidityChanged              FINAL) // device.ventilation.humidity
    Q_PROPERTY(float inOutsideTemperature  READ inOutsideTemperature    NOTIFY inOutsideTemperatureChanged  FINAL) // device.ventilation.outside_input
    Q_PROPERTY(float outOutsideTemperature READ outOutsideTemperature   NOTIFY outOutsideTemperatureChanged FINAL) // device.ventilation.outside_output
    Q_PROPERTY(float inInsideTemperature   READ inInsideTemperature     NOTIFY inInsideTemperatureChanged   FINAL) // device.ventilation.inside_input
    Q_PROPERTY(float outInsideTemperature  READ outInsideTemperature    NOTIFY outInsideTemperatureChanged  FINAL) // device.ventilation.inside_outpout


    Q_DECLARE_PRIVATE(Helios)
    QML_ELEMENT
    Q_DISABLE_COPY(Helios)
    QScopedPointer<HeliosPrivate> const d_ptr;

public:
    explicit Helios(QObject *parent = nullptr);
    ~Helios() override;
    QString hostname() const;
    void setHostname(const QString &newHostname);
    void setRefreshRate(int newRefreshRate);
    bool connected() const;
    int refreshRate() const;

    int forceMode() const;
    int operatingMode() const;
    int fanStage() const;
    int fanPercentage() const;
    int rpmSupply() const;
    int rpmExtract() const;
    int byPass() const;
    float humidity() const;
    float inOutsideTemperature() const;
    float outOutsideTemperature() const;
    float inInsideTemperature() const;
    float outInsideTemperature() const;
    int forceModeSpeed() const;
    int forceModeRemaning() const;
    int forceModeTime() const;

    Q_INVOKABLE void setOperatingMode(int newOperatingMode);
    Q_INVOKABLE void setFanStage(int newFanStage);
    Q_INVOKABLE void setForceModeSpeed(int newModeSpeed);
    Q_INVOKABLE void setForceTime(int newForceTime);
    Q_INVOKABLE void setForceMode(int newForceMode);


public slots:
    void refresh();

signals:
    void hostnameChanged();
    void connectedChanged();
    void forceModeChanged();
    void operatingModeChanged();
    void fanStageChanged();
    void fanPercentageChanged();
    void rpmSupplyChanged();
    void rpmExtractChanged();
    void byPassChanged();
    void humidityChanged();
    void inOutsideTemperatureChanged();
    void outOutsideTemperatureChanged();
    void inInsideTemperatureChanged();
    void outInsideTemperatureChanged();
    void refreshRateChanged();
    void forceModeSpeedChanged();
    void forceModeRemaningChanged();
    void forceModeTimeChanged();

private:
    void _parseWerte(const QString id, const QString &data);
};

#endif // HELIOS_H
