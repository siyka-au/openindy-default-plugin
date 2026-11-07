#ifndef P_POINTCLOUDSEGMENTATION_H
#define P_POINTCLOUDSEGMENTATION_H

#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QThread>
#include <QtGlobal>
#include <format>
#include <string>

#include "cylinder.h"
#include "generatefeaturefunction.h"
#include "oimat.h"
#include "oivec.h"
#include "plane.h"
#include "pluginmetadata.h"
#include "ps_cylindersegment.h"
#include "ps_loadingdialog.h"
#include "ps_planesegment.h"
#include "ps_pointcloud.h"
#include "ps_spheresegment.h"
#include "sphere.h"

class SegmentationProducer : public QObject {
  Q_OBJECT
  QThread workerThread;

 public slots:
  void startSegmentation(PS_PointCloud myCloud, PS_InputParameter param) {
    connect(&myCloud, SIGNAL(updateStatus(std::string, int)), this,
            SLOT(updateStatus(std::string, int)));

    myCloud.setUpOctree(param);
    myCloud.detectShapes(param);

    int i = 1;

    foreach (PS_PlaneSegment* p, myCloud.getDetectedPlanes()) {
      FeatureWrapper* myFeature = new FeatureWrapper();
      Plane* myPlane = new Plane();
      myPlane->setIsSolved(true);
      myPlane->setFeatureName(std::format("plane_{}", i));
      OiVec xyz(3);
      OiVec ijk(3);
      xyz.setAt(0, p->getDistance() * p->getIJK()[0]);
      xyz.setAt(1, p->getDistance() * p->getIJK()[1]);
      xyz.setAt(2, p->getDistance() * p->getIJK()[2]);
      ijk.setAt(0, p->getIJK()[0]);
      ijk.setAt(1, p->getIJK()[1]);
      ijk.setAt(2, p->getIJK()[2]);
      double checkA, checkB;
      OiVec::dot(checkA, xyz, ijk);
      OiVec xyz2 = -1.0 * xyz;
      OiVec::dot(checkB, xyz2, ijk);
      if (std::abs(checkA - p->getDistance()) >
          std::abs(checkB - p->getDistance())) {
        xyz = -1.0 * xyz;
      }
      xyz.add(1.0);
      ijk.add(1.0);
      myPlane->xyz = xyz;
      myPlane->ijk = ijk;
      myFeature->setPlane(myPlane);
      this->addFeature(myFeature);

      i++;
    }

    i = 1;

    foreach (PS_SphereSegment* s, myCloud.getDetectedSpheres()) {
      FeatureWrapper* myFeature = new FeatureWrapper();
      Sphere* mySphere = new Sphere();
      mySphere->setIsSolved(true);
      mySphere->setFeatureName(std::format("sphere_{}", i));
      OiVec xyz(3);
      double r;
      xyz.setAt(0, s->getXYZ()[0]);
      xyz.setAt(1, s->getXYZ()[1]);
      xyz.setAt(2, s->getXYZ()[2]);
      r = s->getRadius();
      xyz.add(1.0);
      mySphere->xyz = xyz;
      mySphere->radius = r;
      myFeature->setSphere(mySphere);
      this->addFeature(myFeature);

      i++;
    }

    i = 1;

    foreach (PS_CylinderSegment* c, myCloud.getDetectedCylinders()) {
      FeatureWrapper* myFeature = new FeatureWrapper();
      Cylinder* myCylinder = new Cylinder();
      myCylinder->setIsSolved(true);
      myCylinder->setFeatureName(std::format("cylinder_{}", i));
      OiVec xyz(3);
      OiVec ijk(3);
      double r;
      float h[3];
      c->getX0(h);
      xyz.setAt(0, h[0]);
      xyz.setAt(1, h[1]);
      xyz.setAt(2, h[2]);
      c->getIJK(h);
      ijk.setAt(0, h[0]);
      ijk.setAt(1, h[1]);
      ijk.setAt(2, h[2]);
      r = c->getRadius();
      xyz.add(1.0);
      ijk.add(1.0);
      myCylinder->xyz = xyz;
      myCylinder->ijk = ijk;
      myCylinder->radius = r;
      myFeature->setCylinder(myCylinder);
      this->addFeature(myFeature);

      i++;
    }

    disconnect(&myCloud, SIGNAL(updateStatus(std::string, int)), this,
               SLOT(updateStatus(std::string, int)));

    this->stopThread();
  }

  void updateStatus(std::string msg, int status) {
    this->changeStatus(msg, status);
  }

 signals:
  void stopThread();
  void changeStatus(std::string, int);
  void addFeature(FeatureWrapper*);
};

class SegmentationConsumer : public QObject {
  Q_OBJECT
  QThread workerThread;

  PS_LoadingDialog myDialog;

  PointCloud& myPointCloud;

 public:
  SegmentationConsumer(PointCloud& myCloud, QObject* parent = NULL)
      : QObject(parent), myPointCloud(myCloud) {
    qRegisterMetaType<PS_PointCloud>("PS_PointCloud");
    qRegisterMetaType<PS_InputParameter>("PS_InputParameter");
    this->mySegmenter = new SegmentationProducer();
    this->mySegmenter->moveToThread(&workerThread);
    connect(this, SIGNAL(startSegmentation(PS_PointCloud, PS_InputParameter)),
            mySegmenter,
            SLOT(startSegmentation(PS_PointCloud, PS_InputParameter)));
    connect(mySegmenter, SIGNAL(stopThread()), this, SLOT(stopThread()));
    connect(mySegmenter, SIGNAL(changeStatus(std::string, int)),
            &this->myDialog, SLOT(setStatus(std::string, int)));
    connect(mySegmenter, SIGNAL(addFeature(FeatureWrapper*)), this,
            SLOT(addFeature(FeatureWrapper*)));

    workerThread.start();

    myDialog.reset();
    myDialog.show();
  }
  /*SegmentationConsumer(const SegmentationConsumer &copy){
      //this->workerThread = copy.workerThread;
      this->mySegmenter = copy.mySegmenter;
  }*/

  void startSegmentationTask(PS_PointCloud myCloud, PS_InputParameter param) {
    this->startSegmentation(myCloud, param);
  }

 signals:
  void startSegmentation(PS_PointCloud myCloud, PS_InputParameter param);

 public slots:
  void stopThread() {
    workerThread.quit();
    workerThread.wait();

    myDialog.close();

    delete this->mySegmenter;
    delete this;
  }

  void addFeature(FeatureWrapper* myFeature) {
    myPointCloud.addSegment(myFeature);
  }

 private:
  SegmentationProducer* mySegmenter;
};

class PointCloudSegmentation : public GenerateFeatureFunction {
 protected:
  // ##############################
  // function initialization method
  // ##############################

  virtual void init();

  // ############
  // exec methods
  // ############

  bool exec(PointCloud& pointCloud);

 private:
  SegmentationConsumer* myHandler;
};

#endif  // P_POINTCLOUDSEGMENTATION_H
