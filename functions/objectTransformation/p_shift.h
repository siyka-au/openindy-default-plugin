#ifndef P_SHIFTPLANE_H
#define P_SHIFTPLANE_H
#include <string>

#include "objecttransformation.h"
#include "oimat.h"
#include "oivec.h"

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

#endif  // P_SHIFTPLANE_H
