#include "p_oi_report_creator.h"

#include <format>

/*!
 * \brief BestFitLine::getMetaData
 * \return
 */
PluginMetaData* OiReportCreator::getMetaData() const {
  PluginMetaData* metaData = new PluginMetaData();
  metaData->name = "Report Creator";
  metaData->pluginName = "OpenIndy Default Plugin";
  metaData->author = "br";
  metaData->description = std::format("{}", "Tool for creating Reports");
  metaData->iid = "de.openIndy.Plugin.OiTool.v001";
  return metaData;
}

void OiReportCreator::customRequest(OiRequestResponse* request) {}

void OiReportCreator::watchWindowKeyPressed(Qt::Key) {}
