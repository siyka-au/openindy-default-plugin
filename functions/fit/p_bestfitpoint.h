#ifndef P_BESTFITPOINT_H
#define P_BESTFITPOINT_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;
using namespace std;

/*!
 * \brief The BestFitPoint class
 */
class BestFitPoint : public FitFunction, public Producer<Point> {
 public:
  explicit BestFitPoint()
      : FitFunction(),
        key("disable all observations but last one"),
        value_no("no"),
        value_yes("yes") {}

  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Point, QString> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Point& point);

  const QString key;  // disable all observations but last one
  const QString value_no;
  const QString value_yes;
};

#endif  // P_BESTFITPOINT_H
