#ifndef HELIOS_PLUGIN_H
#define HELIOS_PLUGIN_H

#include <QQmlExtensionPlugin>

class HeliosPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

#endif // HELIOS_PLUGIN_H
