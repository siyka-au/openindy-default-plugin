#include "p_oiexchangeascii.h"

#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "containers.h"
#include "qtinterop.h"

struct ColumnData {
  QString featureName;
  QString comment;
  QString groupName;
  QString oiFeatureCommonState;
  OiVec position;
  OiVec direction;
};

/*!
 * \brief OiExchangeAscii::init
 */
void OiExchangeAscii::init() {
  // set plugin meta data
  this->metaData.name = "OiExchangeAscii";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = "Read simple Ascii files.";
  this->metaData.iid = ExchangeSimpleAscii_iidd;

  // set supported delimiters
  this->supportedDelimiters.push_back("whitespace [ ]");
  this->supportedDelimiters.push_back("semicolon [;]");

  // set supported geometries
  this->supportedGeometries.push_back(GeometryTypes::ePointGeometry);
  this->supportedGeometries.push_back(GeometryTypes::ePlaneGeometry);
  this->supportedGeometries.push_back(GeometryTypes::ePlaneLevelGeometry);
}

namespace {
// reads the whole stream from its start
QString readText(std::iostream* stream) {
  stream->clear();
  stream->seekg(0);
  std::string bytes((std::istreambuf_iterator<char>(*stream)),
                    std::istreambuf_iterator<char>());
  stream->clear();
  return QString::fromUtf8(bytes.data(), static_cast<qsizetype>(bytes.size()));
}
}  // namespace

/*!
 * \brief OiExchangeAscii::getDefaultColumnOrder
 * \param typeOfGeometry
 * \return
 */
std::vector<ExchangeSimpleAscii::ColumnType>
OiExchangeAscii::getDefaultColumnOrder(const GeometryTypes& typeOfGeometry) {
  std::vector<ExchangeSimpleAscii::ColumnType> defaultColumnOrder;

  try {
    // check if the stream exists
    if (this->stream == nullptr) {
      return defaultColumnOrder;
    }

    // the whole text of the stream (the stream is read from the start)
    QString text = readText(this->stream);

    // read the first twenty lines to get the maximum number of columns
    int numColumns = 0;
    int sampleSize = 20;
    QTextStream in(&text);
    while (!in.atEnd() && sampleSize >= 0) {
      QString line = in.readLine();

      if (line.startsWith("#")         // skip comment
          || line.startsWith(";")      // skip comment
          || line.trimmed().isEmpty()  // skip empty lines
      ) {
        continue;
      }

      // split the line and compare its column count to the maximum column count
      // found before
      std::vector<QString> columns =
          oi::toVector(line.split(this->getDelimiter(this->usedDelimiter)));
      if (std::cmp_greater(columns.size(), numColumns)) {
        numColumns = columns.size();
      }
    }

    // depending on the geometry type and the number of columns fill the default
    // columns order
    switch (typeOfGeometry) {
      case GeometryTypes::ePlaneGeometry:
      case GeometryTypes::ePlaneLevelGeometry:
        if (numColumns == 7) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnPrimaryI);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnPrimaryJ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnPrimaryK);
        }
        break;
      case GeometryTypes::ePointGeometry:
        if (numColumns == 1) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
        } else if (numColumns == 2) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
        } else if (numColumns == 3) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
        } else if (numColumns == 4) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
        } else if (numColumns == 5) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnGroupName);
        } else if (numColumns == 6) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnGroupName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnComment);
        } else if (numColumns > 6) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnGroupName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnComment);
          for (int i = 6; i < numColumns; i++) {
            defaultColumnOrder.push_back(OiExchangeAscii::eColumnIgnore);
          }
        }
        break;
    }

  } catch (const exception& e) {
    emit this->sendMessage(e.what(), MessageTypes::eErrorMessage);
  }

  return defaultColumnOrder;
}

/*!
 * \brief OiExchangeAscii::getFilePreview
 * \param typeOfGeometry
 * \return
 */
std::map<ExchangeSimpleAscii::ColumnType, std::vector<QVariant>>
OiExchangeAscii::getFilePreview(const GeometryTypes& typeOfGeometry) {
  std::map<ExchangeSimpleAscii::ColumnType, std::vector<QVariant>> filePreview;

  // get the default column order
  std::vector<ExchangeSimpleAscii::ColumnType> defaultColumnOrder =
      this->getDefaultColumnOrder(typeOfGeometry);

  // insert an empty list for each column as default
  for (auto i : defaultColumnOrder) {
    filePreview.insert_or_assign(i, std::vector<QVariant>());
  }

  try {
    // check if the stream exists
    if (this->stream == nullptr) {
      return filePreview;
    }

    // the whole text of the stream (the stream is read from the start)
    QString text = readText(this->stream);

    // get the first twenty lines as preview
    int sampleSize = 20;
    QTextStream in(&text);
    while (!in.atEnd() && sampleSize >= 0) {
      QString line = in.readLine();

      // split the line at delimiter
      std::vector<QString> columns =
          oi::toVector(line.split(this->getDelimiter(this->usedDelimiter)));

      // insert the column entries
      for (int i = 0; std::cmp_less(i, defaultColumnOrder.size()); i++) {
        // add an empty entry if the current row has not enough columns
        if (std::cmp_less_equal(columns.size(), i)) {
          filePreview[defaultColumnOrder.at(i)].push_back(QString());
          continue;
        }

        // get the rows that already exist and append the current one
        filePreview[defaultColumnOrder.at(i)].push_back(columns.at(i));
      }

      sampleSize--;
    }

  } catch (const exception& e) {
    emit this->sendMessage(e.what(), MessageTypes::eErrorMessage);
  }

  return filePreview;
}

/*!
 * \brief OiExchangeAscii::getPossibleColumns
 * \param typeOfGeometry
 * \return
 */
std::vector<ExchangeSimpleAscii::ColumnType>
OiExchangeAscii::getPossibleColumns(const GeometryTypes& typeOfGeometry) {
  std::vector<ExchangeSimpleAscii::ColumnType> possibleColumns;

  // add all possible columns depending on the type of geometry
  switch (typeOfGeometry) {
    case GeometryTypes::ePointGeometry:
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnX);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnY);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnZ);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnFeatureName);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnGroupName);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnComment);
      break;
  }

  return possibleColumns;
}

/*!
 * \brief OiExchangeAscii::getDelimiter
 * \param delimiterName
 * \return
 */
QRegularExpression OiExchangeAscii::getDelimiter(
    const QString& delimiterName) const {
  if (delimiterName.compare("whitespace [ ]") == 0) {
    return QRegularExpression("\\s+");
  } else if (delimiterName.compare("semicolon [;]") == 0) {
    return QRegularExpression("[;]");
  }

  return QRegularExpression(".*");
}

/*!
 * \brief OiExchangeAscii::importOiData
 */
void OiExchangeAscii::importOiData() {
  try {
    this->features.clear();

    // check if nominal system is valid
    if (this->nominalSystem.isNull()) {
      emit this->importFinished(false);
      return;
    }

    // set the number of error prone lines to 0
    int numErrors = 0;

    // check if the stream exists
    if (this->stream == nullptr) {
      emit this->importFinished(false);
      return;
    }

    // the whole text of the stream (the stream is read from the start)
    QString text = readText(this->stream);

    qint64 fileSize = text.size();
    qint64 readSize = 0;
    qint64 numPoints = 0;

    bool notSkipped = this->getSkipFirstLine();
    // read all lines
    QTextStream in(&text);
    while (!in.atEnd()) {
      QString line = in.readLine();
      readSize += line.size();

      if (notSkipped                   // skip first line
          || line.startsWith("#")      // skip comment
          || line.startsWith(";")      // skip comment
          || line.trimmed().isEmpty()  // skip empty lines
      ) {
        notSkipped = false;
        continue;
      }

      // convert , to .
      line.replace(",", ".");  // decimal separator TODO use locale format

      // split the line at delimiter
      std::vector<QString> columns =
          oi::toVector(line.split(this->getDelimiter(this->usedDelimiter)));

      bool errorWhileParsing = false;

      ColumnData columnData = {"", "", "", "", OiVec(3), OiVec(3)};

      for (int i = 0; std::cmp_less(i, columns.size()); i++) {
        // stop parsing if the current line has too many columns
        if (std::cmp_less_equal(this->userDefinedColumns.size(), i)) {
          break;
        }

        // set the point attribute depending on the current column
        switch (this->userDefinedColumns.at(i)) {
          case ExchangeSimpleAscii::eColumnCommonState:
            columnData.oiFeatureCommonState = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnFeatureName:
            columnData.featureName = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnComment:
            columnData.comment = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnGroupName:
            columnData.groupName = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnX: {
            double x = 0.0;
            x = columns.at(i).toDouble(&errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eMetric) &&
                oi::value(this->units, DimensionType::eMetric) !=
                    UnitType::eUnitMeter) {
              x = convertToDefault(
                  x, oi::value(this->units, DimensionType::eMetric));
            }

            if (!errorWhileParsing) {
              columnData.position.setAt(0, x);
            }

            break;
          }
          case ExchangeSimpleAscii::eColumnY: {
            double y = 0.0;
            y = columns.at(i).toDouble(&errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eMetric) &&
                oi::value(this->units, DimensionType::eMetric) !=
                    UnitType::eUnitMeter) {
              y = convertToDefault(
                  y, oi::value(this->units, DimensionType::eMetric));
            }

            if (!errorWhileParsing) {
              columnData.position.setAt(1, y);
            }

            break;
          }
          case ExchangeSimpleAscii::eColumnZ: {
            double z = 0.0;
            z = columns.at(i).toDouble(&errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eMetric) &&
                oi::value(this->units, DimensionType::eMetric) !=
                    UnitType::eUnitMeter) {
              z = convertToDefault(
                  z, oi::value(this->units, DimensionType::eMetric));
            }

            if (!errorWhileParsing) {
              columnData.position.setAt(2, z);
            }

            break;
          }
          case ExchangeSimpleAscii::eColumnIgnore:
            break;
          case ExchangeSimpleAscii::eColumnPrimaryI: {
            double value = columns.at(i).toDouble(&errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eAngular) &&
                oi::value(this->units, DimensionType::eAngular) !=
                    UnitType::eUnitDecimalDegree) {
              value = convertToDefault(
                  value, oi::value(this->units, DimensionType::eAngular));
            }

            if (!errorWhileParsing) {
              columnData.direction.setAt(0, value);
            }
            break;
          }
          case ExchangeSimpleAscii::eColumnPrimaryJ: {
            double value = columns.at(i).toDouble(&errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eAngular) &&
                oi::value(this->units, DimensionType::eAngular) !=
                    UnitType::eUnitDecimalDegree) {
              value = convertToDefault(
                  value, oi::value(this->units, DimensionType::eAngular));
            }

            if (!errorWhileParsing) {
              columnData.direction.setAt(1, value);
            }
            break;
          }
          case ExchangeSimpleAscii::eColumnPrimaryK: {
            double value = columns.at(i).toDouble(&errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eAngular) &&
                oi::value(this->units, DimensionType::eAngular) !=
                    UnitType::eUnitDecimalDegree) {
              value = convertToDefault(
                  value, oi::value(this->units, DimensionType::eAngular));
            }

            if (!errorWhileParsing) {
              columnData.direction.setAt(2, value);
            }
            break;
          }
        }

        // if an error occured continue with the next line
        if (errorWhileParsing) {
          numErrors++;
          break;
        }

      }  // loop over colums

      // create geometry and add the imported nominal to OpenIndy
      if (!errorWhileParsing) {
        switch (this->typeOfGeometry) {
          case GeometryTypes::ePointGeometry: {
            oi::Guarded<Point> myNominal = new Point();
            // I use QT property system for transportation, because "common" is
            // not "common" of nominal point but actual point!
            myNominal->setTransientAttribute("OI_FEATURE_COMMONSTATE",
                                             columnData.oiFeatureCommonState);

            myNominal->setFeatureName(columnData.featureName);
            myNominal->setGroupName(columnData.groupName);
            myNominal->setComment(columnData.comment);
            myNominal->setPoint(Position(columnData.position));

            // set group of the geometry
            if (this->groupName.compare("") != 0) {
              myNominal->setGroupName(this->groupName);
            }

            // set nominal system
            myNominal->setNominalSystem(this->nominalSystem);

            oi::Guarded<FeatureWrapper> myGeometry = new FeatureWrapper();
            myGeometry->setPoint(myNominal);
            this->features.push_back(myGeometry);

            break;
          }
          case GeometryTypes::ePlaneGeometry:
          case GeometryTypes::ePlaneLevelGeometry: {
            oi::Guarded<Plane> plane = new Plane();
            // I use QT property system for transportation, because level is a
            // special plane
            plane->setTransientAttribute(
                "OI_FEATURE_PLANE_LEVEL",
                this->typeOfGeometry == GeometryTypes::ePlaneLevelGeometry);

            plane->setFeatureName(columnData.featureName);
            plane->setGroupName(columnData.groupName);
            plane->setComment(columnData.comment);

            // plane->setPoint(Position(columnData.position));
            plane->setPlane(Position(columnData.position),
                            Direction(columnData.direction));

            // set group of the geometry
            if (this->groupName.compare("") != 0) {
              plane->setGroupName(this->groupName);
            }

            // set nominal system
            plane->setNominalSystem(this->nominalSystem);

            oi::Guarded<FeatureWrapper> geometry = new FeatureWrapper();
            geometry->setPlane(plane);
            this->features.push_back(geometry);

            break;
          }
        }  // switch
      }

      // update import progress
      int progress = (int)(((float)readSize / (float)fileSize) * 100.0);
      if (progress == 100) {
        progress = 99;
      }
      numPoints++;
      emit this->updateProgress(progress,
                                QString("%1 nominal(s) loaded").arg(numPoints));
      readSize += 2;
    }

    // emit import finished signal
    emit this->importFinished(true);

  } catch (const exception& e) {
    emit this->sendMessage(e.what(), MessageTypes::eErrorMessage,
                           MessageDestinations::eMessageBoxMessage);
  }
}

/*!
 * \brief OiExchangeAscii::exportOiData
 */
void OiExchangeAscii::exportOiData() {
  try {
    if (this->stream != nullptr) {
      QString outText;
      QTextStream out(&outText);

      for (const oi::Guarded<FeatureWrapper>& fw :
           this->currentJob->getGeometriesList()) {
        if (fw->getGeometry()->getIsSolved() &&
            fw->getGeometry()->getOwnerDescription().has_value()) {
          // write to file
          QString line = "";
          // QString delimiter = this->getDelimiter(this->usedDelimiter);
          for (int i = 0; std::cmp_less(i, this->userDefinedColumns.size());
               i++) {
            if (i > 0) {
              line.append("\t");
            }
            switch (this->userDefinedColumns.at(i)) {
              case eColumnFeatureName:
                line.append(fw->getFeature()->getFeatureName());
                break;
              case eColumnX:
                if (fw->getGeometry()->hasPosition()) {
                  line.append(QString::number(
                      convertFromDefault(
                          fw->getGeometry()->getPosition().getVector().getAt(0),
                          this->units[DimensionType::eMetric]),
                      'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnY:
                if (fw->getGeometry()->hasPosition()) {
                  line.append(QString::number(
                      convertFromDefault(
                          fw->getGeometry()->getPosition().getVector().getAt(1),
                          this->units[DimensionType::eMetric]),
                      'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnZ:
                if (fw->getGeometry()->hasPosition()) {
                  line.append(QString::number(
                      convertFromDefault(
                          fw->getGeometry()->getPosition().getVector().getAt(2),
                          this->units[DimensionType::eMetric]),
                      'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnPrimaryI:
                if (fw->getGeometry()->hasDirection()) {
                  line.append(QString::number(
                      fw->getGeometry()->getDirection().getVector().getAt(0),
                      'f', this->getAngleDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnPrimaryJ:
                if (fw->getGeometry()->hasDirection()) {
                  line.append(QString::number(
                      fw->getGeometry()->getDirection().getVector().getAt(1),
                      'f', this->getAngleDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnPrimaryK:
                if (fw->getGeometry()->hasDirection()) {
                  line.append(QString::number(
                      fw->getGeometry()->getDirection().getVector().getAt(2),
                      'f', this->getAngleDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnRadiusA:
                if (fw->getGeometry()->hasRadius()) {
                  line.append(QString::number(
                      convertFromDefault(
                          fw->getGeometry()->getRadius().getRadius(),
                          this->units[DimensionType::eMetric]),
                      'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                line.append("\n");
                break;
              default:
                break;
            }
          }
          // write to file
          out << line;
        }
      }
      out.flush();
      *this->stream << outText.toStdString();
      this->stream->flush();
      emit this->exportFinished(this->stream->good());

    } else {
      emit this->sendMessage("export stream is not set",
                             MessageTypes::eErrorMessage);
      emit this->exportFinished(false);
    }

  } catch (const exception& e) {
    emit this->sendMessage(e.what(), MessageTypes::eErrorMessage,
                           MessageDestinations::eMessageBoxMessage);
  }
}
