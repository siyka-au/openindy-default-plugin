#ifndef P_TRANSLATEBYVALUE_H
#define P_TRANSLATEBYVALUE_H

#include <QObject>
#include <QPointer>

#include "object_transformation.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

class TranslateByValue : public ObjectTransformation,
                         public Transform<Point>,
                         public Transform<Line>,
                         public Transform<Plane>,
                         public Transform<Sphere> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Point, std::string> exec(const Point& input) override;
  std::expected<Line, std::string> exec(const Line& input) override;
  std::expected<Plane, std::string> exec(const Plane& input) override;
  std::expected<Sphere, std::string> exec(const Sphere& input) override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Point& point);
  bool setUpResult(Line& line);
  bool setUpResult(Plane& plane);
  bool setUpResult(Sphere& sphere);
};

#endif  // P_TRANSLATEBYVALUE_H
