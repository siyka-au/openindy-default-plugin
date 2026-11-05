#ifndef P_INTERSECTLINELINE_H
#define P_INTERSECTLINELINE_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

enum IntersectionTypes { eAtFirstLine, eAtSecondLine, eMidPoint };

/*!
 * \brief The IntersectLineline class
 */
class IntersectLineLine : public ConstructFunction, public Producer<Point> {
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
  bool isParallel(const QPointer<Line>& line1, const QPointer<Line>& line2);
  IntersectionTypes getIntersectionType();
};

#endif  // P_INTERSECTLINELINE_H
