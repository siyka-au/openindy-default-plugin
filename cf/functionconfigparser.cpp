#include "functionconfigparser.h"

#include "cfutil.h"

const char* jsonData =
#include "cfconfig.res"
    ;

QList<ConfiguredFunctionConfig> FunctionConfigParser::readConfigFromJson(
    QString filename) {
  QList<ConfiguredFunctionConfig> configs;

  QString val;
  QFile file;
  file.setFileName(QDir::currentPath() + "/" + filename);
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
QList<ConfiguredFunctionConfig> FunctionConfigParser::readConfigFromJson() {
  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  QJsonObject json = doc.object();

  return this->readConfigFromJson(json);
}

QList<ConfiguredFunctionConfig> FunctionConfigParser::readConfigFromJson(
    QJsonObject json) {
  QList<ConfiguredFunctionConfig> configs;

  if (json["version"].toInt() == 2) {
    foreach (QJsonValue function, json["functions"].toArray()) {
      ConfiguredFunctionConfig config;
      config.version = 2;
      QJsonObject object = function.toObject();
      config.name = object["name"].toString();
      config.description = object["description"].toString();
      config.applicableFor = applicableFor(object["applicableFor"]);

      config.neededElements = neededElements(object["neededElements"]);

      QPointer<Node> node = new CFCParameter(
          config.name, "configured function",
          object["feature"]
              .toString(),  // "global" or TypeOfElement name e.g. Plane
          parameter(object["parameter"]) /* parameter tree */);
      config.parameter = node;

      configs.append(config);
    }
  }

  return configs;
}

/**
 * @brief parameter
 * @param value
 * @return tree with parameter CFCParameter and sub nodes (CFCParameter)
 */
QList<QPointer<Node> > FunctionConfigParser::parameter(QJsonValue value) {
  QList<QPointer<Node> > list;
  foreach (QJsonValue element, value.toArray()) {
    QJsonObject object = element.toObject();

    QPointer<Node> p = new CFCParameter(
        object["name"].toString(), object["comment"].toString(),
        object["feature"].toString(), parameter(object["parameter"]));

    list.append(p);
  }

  return list;
}

QMultiMap<int, InputElementMapping> FunctionConfigParser::inputElementsMapping(
    ConfiguredFunctionConfig config, QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }

  QMultiMap<int, InputElementMapping> inputElementsMapping;
  foreach (QJsonValue element, value.toArray()) {
    QJsonObject object = element.toObject();

    InputElementMapping mapping{};
    mapping.functionIndex = -1;
    mapping.srcInputElementIndex = -1;
    mapping.dstInputElementIndex = -1;

    QString fromNeededElementName =
        object["from"].toObject()["neededElement"].toString();
    for (int i = 0; i < config.neededElements.size(); i++) {
      if (config.neededElements[i].first.compare(fromNeededElementName) == 0) {
        mapping.srcInputElementIndex = i;
        break;
      }
    }

    QString toFunctionName = object["to"].toObject()["function"].toString();
    mapping.functionIndex = config.functionNames.indexOf(toFunctionName);

    mapping.dstInputElementIndex = object["to"].toObject()["index"].toInt();

    inputElementsMapping.insert(mapping.functionIndex, mapping);
  }

  return inputElementsMapping;
}

QList<QString> FunctionConfigParser::functionNames(QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }
  QList<QString> list;
  foreach (QJsonValue element, value.toArray()) {
    QJsonObject object = element.toObject();
    list.append(object["name"].toString());
  }
  return list;
}

ElementTypes FunctionConfigParser::typeOfElement(QJsonValue value) {
  if (value.isNull()) {
    throw logic_error("can not parse json");
  }
  QString s = value.toString();
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

QList<QPair<QString, NeededElement> > FunctionConfigParser::neededElements(
    QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }
  QList<QPair<QString, NeededElement> > list;
  foreach (QJsonValue element, value.toArray()) {
    QJsonObject object = element.toObject();

    NeededElement neededElement;
    neededElement.description = object["description"].toString();
    neededElement.infinite = object["infinite"].toBool();
    neededElement.typeOfElement = typeOfElement(object["typeOfElement"]);
    QPair<QString, NeededElement> ne;
    ne.first = object["name"].toString();
    ne.second = neededElement;
    list.append(ne);
  }

  return list;
}

QList<FeatureTypes> FunctionConfigParser::applicableFor(QJsonValue value) {
  if (value.isNull() || !value.isArray()) {
    throw logic_error("can not parse json");
  }
  QList<FeatureTypes> list;
  foreach (QJsonValue element, value.toArray()) {
    QString s = element.toString();
    list.append(CFUtil::featureTypesForName(s));
  }

  return list;
}
