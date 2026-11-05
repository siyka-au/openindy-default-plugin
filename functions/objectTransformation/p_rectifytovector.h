#ifndef P_RECTIFYTOVECTOR_H
#define P_RECTIFYTOVECTOR_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "objecttransformation.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

enum Axis {
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
  Q_OBJECT

 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Plane, QString> exec(const Plane& input) override;
  std::expected<Circle, QString> exec(const Circle& input) override;
  std::expected<Line, QString> exec(const Line& input) override;
  std::expected<Cylinder, QString> exec(const Cylinder& input) override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Geometry& plane);
  bool direction(OiVec& direction);
  Axis axis();
};

#endif  // P_RECTIFYTOVECTOR_H
