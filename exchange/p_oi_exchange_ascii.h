#ifndef OI_DEFAULT_PLUGIN_EXCHANGE_P_OI_EXCHANGE_ASCII_H_
#define OI_DEFAULT_PLUGIN_EXCHANGE_P_OI_EXCHANGE_ASCII_H_
#include <exception>
#include <map>
#include <vector>

#include "exchange_simple_ascii.h"
#include "oi_job.h"
#include "util.h"

using namespace std;
using namespace oi;

class OiExchangeAscii : public ExchangeSimpleAscii {
 public:
  // ##############################
  // exchange initialization method
  // ##############################

  void init() override;

  // #########################
  // import and export methods
  // #########################

  void importOiData(const OperationContext& ctx = {}) override;
  void exportOiData(const OperationContext& ctx = {}) override;

  // #########################################
  // methods to get or set further information
  // #########################################

  std::vector<ExchangeSimpleAscii::ColumnType> getDefaultColumnOrder(
      const GeometryTypes& typeOfGeometry);
  std::map<ExchangeSimpleAscii::ColumnType, std::vector<oi::Value>>
  getFilePreview(const GeometryTypes& typeOfGeometry);
  std::vector<ExchangeSimpleAscii::ColumnType> getPossibleColumns(
      const GeometryTypes& typeOfGeometry);

 private:
  // ##############
  // helper methods
  // ##############

  [[nodiscard]] std::vector<std::string> splitLine(
      const std::string& line) const;
};

#endif  // OI_DEFAULT_PLUGIN_EXCHANGE_P_OI_EXCHANGE_ASCII_H_
