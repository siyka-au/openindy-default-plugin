#ifndef DISTANCEBETWEENTWOPOINTS_H
#define DISTANCEBETWEENTWOPOINTS_H

#include "construct_function.h"

using namespace oi;

class DistanceBetweenTwoPoints : public ConstructFunction,
                                 public Producer<ScalarEntityDistance> {
 public:
  void init() override;

  std::expected<ScalarEntityDistance, std::string> exec() override;
};

#endif  // DISTANCEBETWEENTWOPOINTS_H
