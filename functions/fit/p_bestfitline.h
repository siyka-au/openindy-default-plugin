#ifndef P_BESTFTILINE_H
#define P_BESTFTILINE_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;
using namespace std;

/*!
 * \brief The BestFitLine class
 */
class BestFitLine : public FitFunction,
                    public FunctionUtil,
                    public Producer<Line> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Line, QString> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Line& line);
};

#endif  // P_BESTFTILINE_H
