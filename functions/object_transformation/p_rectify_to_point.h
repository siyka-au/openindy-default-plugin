#ifndef P_RECTIFYTOPOINT_H
#define P_RECTIFYTOPOINT_H

#include <QObject>
#include <QPointer>

#include "object_transformation.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The RectifyToPoint class
 */
class RectifyToPoint : public ObjectTransformation,
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
  bool point(OiVec& point);
  bool setUpResult(Geometry& geometry);
};

#endif  // P_RECTIFYTOPOINT_H
