#include "plugin.h"

#include "helios.h"

#include <qqml.h>

void HeliosPlugin::registerTypes(const char *uri)
{
    // @uri org.kazoe.helios
    qmlRegisterType<Helios>(uri, 1, 0, "Helios");
}
