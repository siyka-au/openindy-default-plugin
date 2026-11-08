#ifndef P_OIREPORTCREATOR_H
#define P_OIREPORTCREATOR_H

#include "oi_request_response.h"
#include "pi_oitool.h"

class OiReportCreator : public OiTool {
 public:
  PluginMetaData* getMetaData() const;

 public slots:
  void customRequest(OiRequestResponse* request);
  void watchWindowKeyPressed(Qt::Key);
};

#endif  // P_OIREPORTCREATOR_H
