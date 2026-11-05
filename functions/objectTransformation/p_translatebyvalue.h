#ifndef P_TRANSLATEBYVALUE_H
#define P_TRANSLATEBYVALUE_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

class TranslateByValue : public ObjectTransformation,
                         public Transform<Point>,
                         public Transform<Line>,
                         public Transform<Plane>,
                         public Transform<Sphere> {
  Q_OBJECT

 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Point, QString> exec(const Point& input) override;
  std::expected<Line, QString> exec(const Line& input) override;
  std::expected<Plane, QString> exec(const Plane& input) override;
  std::expected<Sphere, QString> exec(const Sphere& input) override;

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
