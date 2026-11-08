#include "p_oi_exchange_pts.h"

#include <QtCore>
#include <format>

/*!
 * \brief OiExchangePts::getMetaData
 * \return
 */
PluginMetaData* OiExchangePts::getMetaData() const {
  PluginMetaData* metaData = new PluginMetaData();

  metaData->name = "OiExchangePts";
  metaData->pluginName = "OpenIndy Default Plugin";
  metaData->author = "br";
  metaData->description = std::format("{}", "Read *.pts files.");
  metaData->iid = "de.openIndy.Plugin.OiExchange.OiExchangeDefinedFormat.v001";

  return metaData;
}

/*!
 * \brief OiExchangePts::importOiData
 * \param projectData
 * \return
 */
bool OiExchangePts::importOiData(OiExchangeObject& projectData) { return true; }

/*!
 * \brief OiExchangePts::exportOiData
 * \param projectData
 * \return
 */
bool OiExchangePts::exportOiData(OiExchangeObject& projectData) { return true; }

/*!
 * \brief OiExchangePts::getSupportedGeometries
 * \return
 */
QList<GeometryTypes> OiExchangePts::getSupportedGeometries() {
  QList<GeometryTypes> supportedGeometries;

  // add supported geometries
  supportedGeometries.append(Configuration::ePointCloudGeometry);

  return supportedGeometries;
}

/*!
 * \brief OiExchangePts::getSupportedFileExtensions
 * \return
 */
std::vector<std::string> OiExchangePts::getSupportedFileExtensions() const {
  std::vector<std::string> supportedFileExtensions;

  // add supported file extensions
  supportedFileExtensions.append("*.pts");

  return supportedFileExtensions;
}
