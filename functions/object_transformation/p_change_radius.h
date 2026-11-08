#ifndef P_CHANGERADIUS_H
#define P_CHANGERADIUS_H

#include <QObject>
#include <QPointer>

#include "object_transformation.h"
#include "oi_mat.h"
#include "oi_vec.h"

using namespace oi;

/*!
 * \brief The ChangeRadius class
 */
class ChangeRadius : public ObjectTransformation,
                     public Transform<Sphere>,
                     public Transform<Circle>,
                     public Transform<Cylinder> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Sphere, std::string> exec(const Sphere& input) override;
  std::expected<Circle, std::string> exec(const Circle& input) override;
  std::expected<Cylinder, std::string> exec(const Cylinder& input) override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Sphere& sphere);
  bool setUpResult(Circle& circle);
  bool setUpResult(Cylinder& cylinder);
};

#endif  // P_CHANGERADIUS_H
