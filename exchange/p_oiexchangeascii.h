#ifndef P_OIEXCHANGEASCII_H
#define P_OIEXCHANGEASCII_H
#include <QRegularExpression>
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
  std::map<ExchangeSimpleAscii::ColumnType, std::vector<QVariant>>
  getFilePreview(const GeometryTypes& typeOfGeometry);
  std::vector<ExchangeSimpleAscii::ColumnType> getPossibleColumns(
      const GeometryTypes& typeOfGeometry);

 private:
  // ##############
  // helper methods
  // ##############

  [[nodiscard]] QRegularExpression getDelimiter(
      const QString& delimiterName) const;
};

#endif  // P_OIEXCHANGEASCII_H
