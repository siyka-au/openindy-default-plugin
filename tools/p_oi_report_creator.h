#ifndef OI_DEFAULT_PLUGIN_TOOLS_P_OI_REPORT_CREATOR_H_
#define OI_DEFAULT_PLUGIN_TOOLS_P_OI_REPORT_CREATOR_H_

#include "oi_request_response.h"
#include "pi_oitool.h"

class OiReportCreator : public OiTool {
 public:
  PluginMetaData* getMetaData() const;

 public slots:
  void customRequest(OiRequestResponse* request);
  void watchWindowKeyPressed(Qt::Key);
};

#endif  // OI_DEFAULT_PLUGIN_TOOLS_P_OI_REPORT_CREATOR_H_
