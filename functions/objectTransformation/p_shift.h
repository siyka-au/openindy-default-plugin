#ifndef P_SHIFTPLANE_H
#define P_SHIFTPLANE_H

#include <QList>
#include <QString>

#include "objecttransformation.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

class Shift : public ObjectTransformation,
              public Transform<Plane>,
              public Transform<Circle> {
  Q_OBJECT

 public:
  // ##############################
  // function initialization method
  // ##############################

  void init();

  // ############
  // exec methods
  // ############

  std::expected<Plane, QString> exec(const Plane& input) override;
  std::expected<Circle, QString> exec(const Circle& input) override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Plane& plane);
  bool setUpResult(Circle& circle);
};

#endif  // P_SHIFTPLANE_H
