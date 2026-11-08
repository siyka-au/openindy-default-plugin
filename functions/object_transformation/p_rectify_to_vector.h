#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_RECTIFY_TO_VECTOR_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_RECTIFY_TO_VECTOR_H_

#include <QObject>
#include <QPointer>
#include <string>

#include "object_transformation.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

enum class Axis {
  eXAxis = 0,
  eYAxis,
  eZAxis  // ijk
};

/*!
 * \brief The RectifyToVector class
 */
class RectifyToVector : public ObjectTransformation,
                        public Transform<Plane>,
                        public Transform<Circle>,
                        public Transform<Line>,
                        public Transform<Cylinder> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Plane, std::string> exec(const Plane& input) override;
  std::expected<Circle, std::string> exec(const Circle& input) override;
  std::expected<Line, std::string> exec(const Line& input) override;
  std::expected<Cylinder, std::string> exec(const Cylinder& input) override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Geometry& plane);
  bool direction(OiVec& direction);
  Axis axis();
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_RECTIFY_TO_VECTOR_H_
