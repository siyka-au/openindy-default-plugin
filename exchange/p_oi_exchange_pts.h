#ifndef OI_DEFAULT_PLUGIN_EXCHANGE_P_OI_EXCHANGE_PTS_H_
#define OI_DEFAULT_PLUGIN_EXCHANGE_P_OI_EXCHANGE_PTS_H_

#include <QtCore>

#include "exchange_defined_format.h"

class OiExchangePts : public OiExchangeDefinedFormat {
 public:
  // overwrite pure virtual methods of OiExchangeInterface
  PluginMetaData* getMetaData() const;
  bool importOiData(OiExchangeObject& projectData);
  bool exportOiData(OiExchangeObject& projectData);
  QList<GeometryTypes> getSupportedGeometries();

  // overwrite pure virtual methods of OiExchangeDefinedFormat
  std::vector<std::string> getSupportedFileExtensions() const;
};

#endif  // OI_DEFAULT_PLUGIN_EXCHANGE_P_OI_EXCHANGE_PTS_H_
