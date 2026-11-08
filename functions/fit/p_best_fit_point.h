#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_POINT_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_POINT_H_

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

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_POINT_H_
