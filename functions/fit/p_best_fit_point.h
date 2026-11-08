#ifndef P_BESTFITPOINT_H
#define P_BESTFITPOINT_H

#include <QObject>
#include <QPointer>
#include <string>

#include "fit_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

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

  std::expected<Point, std::string> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Point& point);

  const std::string key;  // disable all observations but last one
  const std::string value_no;
  const std::string value_yes;
};

#endif  // P_BESTFITPOINT_H
