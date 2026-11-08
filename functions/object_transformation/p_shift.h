#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_SHIFT_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_SHIFT_H_
#include <string>

#include "object_transformation.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

class Shift : public ObjectTransformation,
              public Transform<Plane>,
              public Transform<Circle> {
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

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Plane& plane);
  bool setUpResult(Circle& circle);
};

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_SHIFT_H_
