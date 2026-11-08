#ifndef P_HELMERT7PARAM_H
#define P_HELMERT7PARAM_H

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QtGlobal>
#include <string>
#include <vector>

#include "oi_mat.h"
#include "oi_vec.h"
#include "plugin_metadata.h"
#include "point.h"
#include "system_transformation.h"
#include "trafo_param.h"
#include "util.h"

using namespace oi;
using namespace std;

enum class ScaleTypes { noScale, pointScale, tempScale };

class Helmert7Param : public SystemTransformation, public Producer<TrafoParam> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<TrafoParam, std::string> exec() override;

 private:
  bool setUpResult(TrafoParam& trafoParam);

  // attributes
  bool svdError{};

  ScaleTypes scaleType;

  double setScaleValue();

  OiVec p6_translation;
  OiVec p6_rotation;

  std::vector<OiVec> extendVector(std::vector<OiVec> vec);
  std::vector<OiVec> locSystem;
  std::vector<OiVec> refSystem;

  // general functions
  void getScaleType();
  void initPoints();

  // 7 parameter functions
  bool calc_7p(TrafoParam& tp);

  vector<OiVec> calcCentroidCoord();
  vector<OiVec> centroidReducedCoord(std::vector<OiVec> input, OiVec centroid);
  vector<OiMat> modelMatrix(vector<OiVec> locC, vector<OiVec> refC);
  OiMat normalEquationMatrix(vector<OiMat> vecA);
  OiVec quaternion(OiMat n);
  OiMat rotationMatrix(OiVec q);
  void fillTrafoParam(OiMat r, vector<OiVec> locC, vector<OiVec> refC,
                      vector<OiVec> centroidCoords, TrafoParam& tp);
  bool adjust_7p(TrafoParam& tp);
  OiMat fillAMatrix(OiVec x0);
  OiVec fillLVector();
  OiVec fillL0Vector(OiVec x0);

  // 6 Parameter ohne Maßstab/ mit Maßstab aus Temperatur
  bool calc_6p(TrafoParam& tp);

  OiMat p6_fillAMatrix(OiVec x0);
  OiVec p6_fillLVector();
  OiVec p6_fillL0Vector(OiVec x0);
  void p6_preliminaryTransformation();
  OiVec p6_approxRotation();
  OiVec p6_approxTranslation(OiVec rot);

  OiMat p6_getRotationMatrix(OiVec rot);
  OiVec p6_getRotationAngles(OiMat r);
  OiMat p6_getTranslationMatrix(OiVec trans);
  OiMat p6_getScaleMatrix(OiVec s);
};

#endif  // P_HELMERT7PARAM_H
