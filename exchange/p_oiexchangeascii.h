#ifndef P_OIEXCHANGEASCII_H
#define P_OIEXCHANGEASCII_H
#include <exception>
#include <map>
#include <vector>

#include "exchangesimpleascii.h"
#include "oijob.h"
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

  void importOiData() override;
  void exportOiData() override;

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

#endif  // P_OIEXCHANGEASCII_H
