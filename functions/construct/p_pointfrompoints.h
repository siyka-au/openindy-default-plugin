#ifndef P_POINTFROMPOINTS_H
#define P_POINTFROMPOINTS_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "constructfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;
using namespace std;

/*!
 * \brief The PointFromPoints class
 */
class PointFromPoints : public ConstructFunction, public Producer<Point> {
  Q_OBJECT

 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Point, QString> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Point& point);
};

#endif  // P_POINTFROMPOINTS_H
