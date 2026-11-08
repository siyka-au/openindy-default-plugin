#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_LINE_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_LINE_H_

#include <QObject>
#include <QPointer>
#include <string>

#include "fit_function.h"
#include "oi_mat.h"
#include "oi_vec.h"

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

  std::expected<Line, std::string> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Line& line);
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_FIT_P_BEST_FIT_LINE_H_
