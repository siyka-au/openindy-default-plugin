#include "xyzfilter.h"

XYZFilter::XYZFilter(QString name, bool removeX, bool removeY, bool removeZ)
    : ConstructFunction(),
      removeX(removeX),
      removeY(removeY),
      removeZ(removeZ) {
  this->metaData.name = name;
}

void XYZFilter::init() {
  this->applicableFor.append(FeatureTypes::ePointFeature);
}

std::expected<Point, QString> XYZFilter::exec(const Point& input) {
  if (this->inputElements[0].isEmpty()) {
    emit this->sendMessage("inputElements[0] is empty",
                           MessageTypes::eWarningMessage);
    return std::unexpected(QString("XYZFilter: inputElements[0] is empty"));
  }

  oi::math::OiVec p =
      this->inputElements[0].first().geometry->getPosition().getVector();

  Point point = input;

  oi::math::OiVec v = point.getPosition().getVector();

  v.setAt(0, removeX ? 0.0 : p.getAt(0));
  v.setAt(1, removeY ? 0.0 : p.getAt(1));
  v.setAt(2, removeZ ? 0.0 : p.getAt(2));

  Position position = point.getPosition();
  position.setVector(v);
  point.setPosition(position);

  return point;
}
