#include "p_best_fit_circle.h"

#include <math.h>

#include <QtCore>
#include <format>
#include <vector>

namespace {
constexpr int kMaxIterations = 50;
constexpr double kInitialMaxAbsDu = 1.0E308;
constexpr double kConvergenceLimit = 1.0E-8;
constexpr int kUnknowns =
    8;  // circle unknowns plus the offset of the helper plane
constexpr int kUnknownPlaneOffset = 7;  // column of the helper plane offset
constexpr int kConstrainedSize = 10;    // kUnknowns plus the two constraints
constexpr int kFittedParameters =
    6;  // degrees of freedom removed by the circle fit
constexpr double kSquareDerivative = 2.0;  // d(x^2)/dx
}  // namespace

/*!
 * \brief BestFitCircle::init
 */
void BestFitCircle::init() {
  // set plugin meta data
  this->metaData.name = "BestFitCircle";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "Michael Loesler - derletztekick.com";
  this->metaData.description = std::format(
      "{} {} {}",
      "This function estimates a spatial circle usnig a general least "
      "squares algorithm.",
      "Insert at least three non-collinear points, to get the best fit "
      "circle.",
      "The algorithm was taken from the OpenSource FormFittingToolbox "
      "(c) M.Loesler (GNU-GPL).");
  this->metaData.iid = FitFunction_iidd;

  // D12/Stage 7c-ii: plain BestFitCircle had no Points-based twin before
  // this stage - it is converted in place to the same tag-resolved,
  // Point-consuming shape as every other fit here.
  this->neededElements.clear();
  NeededElement param1;
  param1.description =
      "Select at least three non-collinear points to calculate the best fit "
      "circle.";
  param1.infinite = true;
  param1.typeOfElement = ElementTypes::ePointElement;
  param1.roleName = "default";
  this->neededElements.push_back(param1);

  // set spplicable for
  this->applicableFor.push_back(FeatureTypes::eCircleFeature);
}

/*!
 * \brief BestFitCircle::exec
 * \return
 */
std::expected<Circle, std::string> BestFitCircle::exec() {
  Circle circle;
  if (!this->setUpResult(circle)) {
    return std::unexpected(std::format("{} failed", this->getMetaData().name));
  }
  return circle;
}

/*!
 * \brief BestFitCircle::setUpResult
 * \param circle
 * \return
 */
bool BestFitCircle::setUpResult(Circle& circle) {
  // get and check input points
  if (!this->inputElements.contains(0) || this->inputElements[0].empty()) {
    this->sendMessage(
        std::format("No points tagged for circle {}", circle.getFeatureName()),
        MessageTypes::eWarningMessage);
    return false;
  }
  std::vector<oi::Guarded<Point> > inputPoints;
  for (const InputElement& element : this->inputElements[0]) {
    if (!element.point.isNull() && element.point->getIsSolved() &&
        element.shouldBeUsed) {
      inputPoints.push_back(element.point);
      this->setIsUsed(0, element.id, true);
      continue;
    }
    this->setIsUsed(0, element.id, false);
  }
  if (inputPoints.size() < 3) {
    this->sendMessage(std::format("Not enough valid points to fit the point {}",
                                  circle.getFeatureName()),
                      MessageTypes::eWarningMessage);
    return false;
  }

  int obsCount = inputPoints.size();

  // fill x,y,z arrays
  std::vector<double> x(obsCount);
  std::vector<double> y(obsCount);
  std::vector<double> z(obsCount);

  int k = 0;

  double xc = 0, yc = 0, zc = 0, rc = 0;

  for (const oi::Guarded<Point>& p : inputPoints) {
    const OiVec pos = p->getPosition().getVector();
    x[k] = pos.getAt(0);
    y[k] = pos.getAt(1);
    z[k] = pos.getAt(2);

    xc += x[k];
    yc += y[k];
    zc += z[k];

    k++;
  }

  xc /= obsCount;
  yc /= obsCount;
  zc /= obsCount;

  for (int i = 0; i < obsCount; i++)
    rc += sqrt((x[i] - xc) * (x[i] - xc) + (y[i] - yc) * (y[i] - yc) +
               (z[i] - zc) * (z[i] - zc));
  rc /= obsCount;

  OiVec plane = this->getApproximatedPlane(obsCount, x, y, z, xc, yc, zc);

  int maxItr = kMaxIterations;
  double maxAbsDu = kInitialMaxAbsDu;
  double nx = plane.getAt(0);
  double ny = plane.getAt(1);
  double nz = plane.getAt(2);
  double d = plane.getAt(3);

  OiVec v(3 * obsCount);
  OiMat Qxx(kUnknowns, kUnknowns);

  OiMat A(2 * obsCount, kUnknowns);
  OiMat B(2 * obsCount, 3 * obsCount);
  OiVec w(2 * obsCount);
  OiMat Pww(2 * obsCount, 2 * obsCount);
  OiMat NGL(kUnknowns, kUnknowns);
  OiVec ngl(kUnknowns);
  OiMat NGLR(kConstrainedSize, kConstrainedSize);
  OiMat QxxR(kConstrainedSize, kConstrainedSize);
  OiVec nglr(kConstrainedSize);
  OiVec du(kUnknowns);
  OiVec duk(kConstrainedSize);

  int iterations = 0;
  while (true) {
    for (int i = 0; i < obsCount; i++) {
      double xP = x[i] - v.getAt(3 * i);
      double yP = y[i] - v.getAt(3 * i + 1);
      double zP = z[i] - v.getAt(3 * i + 2);

      // Kugelparameter
      A.setAt(i, Circle::unknownCenterX, -kSquareDerivative * (xP - xc));
      A.setAt(i, Circle::unknownCenterY, -kSquareDerivative * (yP - yc));
      A.setAt(i, Circle::unknownCenterZ, -kSquareDerivative * (zP - zc));
      A.setAt(i, Circle::unknownRadius, -kSquareDerivative * rc);
      // A.setAt(i, Circle::unknownNormalI, 0.0);
      // A.setAt(i, Circle::unknownNormalJ, 0.0);
      // A.setAt(i, Circle::unknownNormalK, 0.0);
      // A.setAt(i, 7, 0.0);

      B.setAt(i, i * 3, kSquareDerivative * (xP - xc));
      B.setAt(i, i * 3 + 1, kSquareDerivative * (yP - yc));
      B.setAt(i, i * 3 + 2, kSquareDerivative * (zP - zc));

      w.setAt(i, rc * rc - ((xP - xc) * (xP - xc) + (yP - yc) * (yP - yc) +
                            (zP - zc) * (zP - zc)));

      // Ebene in Normalform
      // A.setAt(obsCount+i, 0.0);
      // A.setAt(obsCount+i, 0.0);
      // A.setAt(obsCount+i, 0.0);
      // A.setAt(obsCount+i, 0.0);
      A.setAt(obsCount + i, Circle::unknownNormalI, xP);
      A.setAt(obsCount + i, Circle::unknownNormalJ, yP);
      A.setAt(obsCount + i, Circle::unknownNormalK, zP);
      A.setAt(obsCount + i, kUnknownPlaneOffset, -1.0);

      B.setAt(obsCount + i, i * 3, nx);
      B.setAt(obsCount + i, i * 3 + 1, ny);
      B.setAt(obsCount + i, i * 3 + 2, nz);

      w.setAt(obsCount + i, d - (nx * xP + ny * yP + nz * zP));
    }

    w = -1 * B * v + w;

    Pww = (B * B.t()).inv();
    NGL = A.t() * Pww * A;
    ngl = A.t() * Pww * w;

    for (int i = 0; i < NGL.getRowCount(); i++) {
      nglr.setAt(i, ngl.getAt(i));
      for (int j = 0; j < NGL.getColCount(); j++)
        NGLR.setAt(i, j, NGL.getAt(i, j));
    }

    NGLR.setAt(NGL.getRowCount(), Circle::unknownCenterX, nx);
    NGLR.setAt(NGL.getRowCount(), Circle::unknownCenterY, ny);
    NGLR.setAt(NGL.getRowCount(), Circle::unknownCenterZ, nz);
    NGLR.setAt(NGL.getRowCount(), Circle::unknownNormalI, xc);
    NGLR.setAt(NGL.getRowCount(), Circle::unknownNormalJ, yc);
    NGLR.setAt(NGL.getRowCount(), Circle::unknownNormalK, zc);
    NGLR.setAt(NGL.getRowCount(), kUnknownPlaneOffset, -1.0);
    nglr.setAt(NGL.getRowCount(), d - (nx * xc + ny * yc + nz * zc));

    NGLR.setAt(NGL.getRowCount() + 1, Circle::unknownNormalI,
               kSquareDerivative * nx);
    NGLR.setAt(NGL.getRowCount() + 1, Circle::unknownNormalJ,
               kSquareDerivative * ny);
    NGLR.setAt(NGL.getRowCount() + 1, Circle::unknownNormalK,
               kSquareDerivative * nz);
    nglr.setAt(NGL.getRowCount() + 1, 1.0 - (nx * nx + ny * ny + nz * nz));

    NGLR.setAt(Circle::unknownCenterX, NGL.getRowCount(), nx);
    NGLR.setAt(Circle::unknownCenterY, NGL.getRowCount(), ny);
    NGLR.setAt(Circle::unknownCenterZ, NGL.getRowCount(), nz);
    NGLR.setAt(Circle::unknownNormalI, NGL.getRowCount(), xc);
    NGLR.setAt(Circle::unknownNormalJ, NGL.getRowCount(), yc);
    NGLR.setAt(Circle::unknownNormalK, NGL.getRowCount(), zc);
    NGLR.setAt(kUnknownPlaneOffset, NGL.getRowCount(), -1.0);

    NGLR.setAt(Circle::unknownNormalI, NGL.getRowCount() + 1,
               kSquareDerivative * nx);
    NGLR.setAt(Circle::unknownNormalJ, NGL.getRowCount() + 1,
               kSquareDerivative * ny);
    NGLR.setAt(Circle::unknownNormalK, NGL.getRowCount() + 1,
               kSquareDerivative * nz);

    QxxR = NGLR.inv();
    duk = QxxR * nglr;

    xc += duk.getAt(Circle::unknownCenterX);
    yc += duk.getAt(Circle::unknownCenterY);
    zc += duk.getAt(Circle::unknownCenterZ);
    rc += duk.getAt(Circle::unknownRadius);

    nx += duk.getAt(Circle::unknownNormalI);
    ny += duk.getAt(Circle::unknownNormalJ);
    nz += duk.getAt(Circle::unknownNormalK);
    d += duk.getAt(kUnknownPlaneOffset);

    maxAbsDu = duk.getAt(0);
    for (int i = 0; i < du.getSize(); i++) {
      du.setAt(i, duk.getAt(i));
      maxAbsDu = max(maxAbsDu, fabs(duk.getAt(i)));

      // Dies waere nur am Ende der letzten Iteration notwendig
      for (int j = 0; j < du.getSize(); j++) Qxx.setAt(i, j, QxxR.getAt(i, j));
    }

    v = B.t() * Pww * (A * du - w);

    if (iterations < maxItr && maxAbsDu < kConvergenceLimit) {
      qDebug() << "DEBUG: Found Solution!";
      break;
    } else if (iterations > maxItr) {
      qDebug() << "DEBUG: Estimation failed!";
      return false;
    }
    iterations++;
  }

  // Statistik
  Statistic myStats;
  myStats.setS0APriori(1.0);
  double vv = NAN;
  OiVec::dot(vv, v, v);
  myStats.setS0APosteriori((obsCount - kFittedParameters) > 0
                               ? sqrt(vv / (obsCount - kFittedParameters))
                               : 1.0);

  Position circlePosition;
  OiVec xyz(3);
  xyz.setAt(0, xc);
  xyz.setAt(1, yc);
  xyz.setAt(2, zc);
  circlePosition.setVector(xyz);

  Direction circleDirection;
  OiVec ijk(3);
  ijk.setAt(0, nx);
  ijk.setAt(1, ny);
  ijk.setAt(2, nz);
  circleDirection.setVector(ijk);

  Radius circleRadius;
  circleRadius.setRadius(rc);

  circle.setCircle(circlePosition, circleDirection, circleRadius);

  myStats.setV(v);
  myStats.setQxx(Qxx);

  myStats.setIsValid(true);
  circle.setStatistic(myStats);
  this->statistic = circle.getStatistic();

  // per-point display residuals go to the function's own statistic: the copy
  // above (like every Statistic copy) drops them, so they must be added to
  // this->statistic afterwards, as the other fit functions do
  for (int i = 0; i < obsCount; i++) {
    const double vx = v.getAt(3 * i);
    const double vy = v.getAt(3 * i + 1);
    const double vz = v.getAt(3 * i + 2);
    this->addDisplayResidual(inputPoints.at(i)->getId(), vx, vy, vz,
                             std::sqrt(vx * vx + vy * vy + vz * vz));
  }

  return true;
}

/**
 * Estimate the approx. Plane. SourceCode is taken from
 * com.derletztekick.geodesy.formFittingToolbox.v2.form.form3d.Circle3D
 * @brief BestFitCircle::getApproximatedPlane
 * @param nop Number of Points
 * @param x X-values
 * @param y Y-values
 * @param z Z-values
 * @param cx X-value of centroid
 * @param cy Y-value of centroid
 * @param cz Z-value of centroid
 * @return plane
 */
OiVec BestFitCircle::getApproximatedPlane(int nop, std::span<const double> x,
                                          std::span<const double> y,
                                          std::span<const double> z, double cx,
                                          double cy, double cz) {
  OiVec X(4);
  OiMat H(3, 3);
  for (int k = 0; k < nop; k++) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        double a = 0, b = 0;
        if (i == 0)
          a = x[k] - cx;
        else if (i == 1)
          a = y[k] - cy;
        else
          a = z[k] - cz;

        if (j == 0)
          b = x[k] - cx;
        else if (j == 1)
          b = y[k] - cy;
        else
          b = z[k] - cz;
        H.setAt(i, j, H.getAt(i, j) + a * b);
      }
    }
  }
  OiMat u(3, 3);
  OiVec d(3);
  OiMat v(3, 3);
  H.svd(u, d, v);
  int indexMinEigVal = 0;
  double minEigVal = d.getAt(indexMinEigVal);
  for (int i = indexMinEigVal + 1; i < d.getSize(); i++) {
    if (minEigVal > d.getAt(i)) {
      minEigVal = d.getAt(i);
      indexMinEigVal = i;
    }
  }
  for (int i = 0; i < 3; i++) X.setAt(i, v.getAt(i, indexMinEigVal));
  X.setAt(3, X.getAt(0) * cx + X.getAt(1) * cy + X.getAt(2) * cz);
  return X;
}
