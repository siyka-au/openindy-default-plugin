#include "functionconfigparser.h"

#include <QtCore>
#include <map>
#include <vector>

#include "cfutil.h"
#include "containers.h"

namespace {
// JSON values are read with Qt; the configuration itself is std::string
std::string text(const QJsonValue& value) {
  return value.toString().toStdString();
}
}  // namespace

const char* jsonData =
#include <utility>

#include "cfconfig.res"
    ;

std::vector<ConfiguredFunctionConfig> FunctionConfigParser::readConfigFromJson(
    std::string filename) {
  std::vector<ConfiguredFunctionConfig> configs;

  QString val;
  QFile file;
  file.setFileName(QDir::currentPath() + "/" +
                   QString::fromStdString(filename));
  if (!file.exists()) {
    return configs;
  }
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  val = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
  QJsonObject json = doc.object();

  return this->readConfigFromJson(json);
}

/**
 * @brief FunctionConfigParser::readConfigFromJson
 * @return internal JSON config
 */
std::vector<ConfiguredFunctionConfig>
FunctionConfigParser::readConfigFromJson() {
  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  QJsonObject json = doc.object();

  return this->readConfigFromJson(json);
}

std::vector<ConfiguredFunctionConfig> FunctionConfigParser::readConfigFromJson(
    QJsonObject json) {
  std::vector<ConfiguredFunctionConfig> configs;

  if (json["version"].toInt() == 2) {
    for (QJsonValue function : json["functions"].toArray()) {
      ConfiguredFunctionConfig config;
      config.version = 2;
      QJsonObject object = function.toObject();
      config.name = text(object["name"]);
      config.description = text(object["description"]);
      config.applicableFor = applicableFor(object["applicableFor"]);

      config.neededElements = neededElements(object["neededElements"]);

      QPointer<Node> node = new CFCParameter(
          config.name, "configured function",
          text(object["feature"]),  // "global" or TypeOfElement name e.g. Plane
          parameter(object["parameter"]) /* parameter tree */);
      config.parameter = node;

      configs.push_back(config);
    }
  }

  return configs;
}

/**
 * @brief parameter
 * @param value
 * @return tree with parameter CFCParameter and sub nodes (CFCParameter)
 */
std::vector<QPointer<Node> > FunctionConfigParser::parameter(QJsonValue value) {
  std::vector<QPointer<Node> > list;
  for (QJsonValue element : value.toArray()) {
    QJsonObject object = element.toObject();

    QPointer<Node> p = new CFCParameter(
        text(object["name"]), text(object["comment"]), text(object["feature"]),
        parameter(object["parameter"]));

    list.push_back(p);
  }

  return list;
}

std::multimap<int, InputElementMapping>
FunctionConfigParser::inputElementsMapping(ConfiguredFunctionConfig config,
                                           QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }

  std::multimap<int, InputElementMapping> inputElementsMapping;
  for (QJsonValue element : value.toArray()) {
    QJsonObject object = element.toObject();

    InputElementMapping mapping{};
    mapping.functionIndex = -1;
    mapping.srcInputElementIndex = -1;
    mapping.dstInputElementIndex = -1;

    std::string fromNeededElementName =
        text(object["from"].toObject()["neededElement"]);
    for (int i = 0; std::cmp_less(i, config.neededElements.size()); i++) {
      if (config.neededElements[i].first.compare(fromNeededElementName) == 0) {
        mapping.srcInputElementIndex = i;
        break;
      }
    }

    std::string toFunctionName = text(object["to"].toObject()["function"]);
    mapping.functionIndex = oi::indexOf(config.functionNames, toFunctionName);

    mapping.dstInputElementIndex = object["to"].toObject()["index"].toInt();

    inputElementsMapping.emplace(mapping.functionIndex, mapping);
  }

  return inputElementsMapping;
}

std::vector<std::string> FunctionConfigParser::functionNames(QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }
  std::vector<std::string> list;
  for (QJsonValue element : value.toArray()) {
    QJsonObject object = element.toObject();
    list.push_back(text(object["name"]));
  }
  return list;
}

ElementTypes FunctionConfigParser::typeOfElement(QJsonValue value) {
  if (value.isNull()) {
    throw logic_error("can not parse json");
  }
  std::string s = text(value);
  if (s.compare("Circle") == 0) {
    return ElementTypes::eCircleElement;
  } else if (s.compare("Cone") == 0) {
    return ElementTypes::eConeElement;
  } else if (s.compare("Cylinder") == 0) {
    return ElementTypes::eCylinderElement;
  } else if (s.compare("Ellipse") == 0) {
    return ElementTypes::eEllipseElement;
  } else if (s.compare("Ellipsoid") == 0) {
    return ElementTypes::eEllipsoidElement;
  } else if (s.compare("Hyperboloid") == 0) {
    return ElementTypes::eHyperboloidElement;
  } else if (s.compare("Line") == 0) {
    return ElementTypes::eLineElement;
  } else if (s.compare("Nurbs") == 0) {
    return ElementTypes::eNurbsElement;
  } else if (s.compare("Paraboloid") == 0) {
    return ElementTypes::eParaboloidElement;
  } else if (s.compare("Plane") == 0) {
    return ElementTypes::ePlaneElement;
  } else if (s.compare("Point") == 0) {
    return ElementTypes::ePointElement;
  } else if (s.compare("PointCloud") == 0) {
    return ElementTypes::ePointCloudElement;
  } else if (s.compare("ScalarEntityAngle") == 0) {
    return ElementTypes::eScalarEntityAngleElement;
  } else if (s.compare("ScalarEntityDistance") == 0) {
    return ElementTypes::eScalarEntityDistanceElement;
  } else if (s.compare("ScalarEntityMeasurementSeries") == 0) {
    return ElementTypes::eScalarEntityMeasurementSeriesElement;
  } else if (s.compare("ScalarEntityTemperature") == 0) {
    return ElementTypes::eScalarEntityTemperatureElement;
  } else if (s.compare("SlottedHole") == 0) {
    return ElementTypes::eSlottedHoleElement;
  } else if (s.compare("Sphere") == 0) {
    return ElementTypes::eSphereElement;
  } else if (s.compare("Torus") == 0) {
    return ElementTypes::eTorusElement;
  } else if (s.compare("Direction") == 0) {
    return ElementTypes::eDirectionElement;
  } else if (s.compare("Position") == 0) {
    return ElementTypes::ePositionElement;
  } else if (s.compare("Radius") == 0) {
    return ElementTypes::eRadiusElement;
  } else if (s.compare("CoordinateSystem") == 0) {
    return ElementTypes::eCoordinateSystemElement;
  } else if (s.compare("Station") == 0) {
    return ElementTypes::eStationElement;
  } else if (s.compare("TrafoParam") == 0) {
    return ElementTypes::eTrafoParamElement;
  } else if (s.compare("Observation") == 0) {
    return ElementTypes::eObservationElement;
  } else if (s.compare("ReadingCartesian") == 0) {
    return ElementTypes::eReadingCartesianElement;
  } else if (s.compare("ReadingPolar") == 0) {
    return ElementTypes::eReadingPolarElement;
  } else if (s.compare("ReadingDistance") == 0) {
    return ElementTypes::eReadingDistanceElement;
  } else if (s.compare("ReadingDirection") == 0) {
    return ElementTypes::eReadingDirectionElement;
  } else if (s.compare("ReadingTemperature") == 0) {
    return ElementTypes::eReadingTemperatureElement;
  } else if (s.compare("ReadingLevel") == 0) {
    return ElementTypes::eReadingLevelElement;
  } else {
    return ElementTypes::eUndefinedElement;
  }
}

std::vector<std::pair<std::string, NeededElement> >
FunctionConfigParser::neededElements(QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }
  std::vector<std::pair<std::string, NeededElement> > list;
  for (QJsonValue element : value.toArray()) {
    QJsonObject object = element.toObject();

    NeededElement neededElement;
    neededElement.description = text(object["description"]);
    neededElement.infinite = object["infinite"].toBool();
    neededElement.typeOfElement = typeOfElement(object["typeOfElement"]);
    std::pair<std::string, NeededElement> ne;
    ne.first = text(object["name"]);
    ne.second = neededElement;
    list.push_back(ne);
  }

  return list;
}

std::vector<FeatureTypes> FunctionConfigParser::applicableFor(
    QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }
  std::vector<FeatureTypes> list;
  for (QJsonValue element : value.toArray()) {
    std::string s = text(element);
    list.push_back(CFUtil::featureTypesForName(s));
  }

  return list;
}
