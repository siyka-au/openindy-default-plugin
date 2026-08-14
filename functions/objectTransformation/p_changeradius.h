#ifndef P_CHANGERADIUS_H
#define P_CHANGERADIUS_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The ChangeRadius class
 */
class ChangeRadius : public ObjectTransformation, public Transform<Sphere>, public Transform<Circle>, public Transform<Cylinder>
{
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init();

    //############
    //exec methods
    //############

    std::expected<Sphere, QString> exec(const Sphere &input) override;
    std::expected<Circle, QString> exec(const Circle &input) override;
    std::expected<Cylinder, QString> exec(const Cylinder &input) override;

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Sphere &sphere);
    bool setUpResult(Circle &circle);
    bool setUpResult(Cylinder &cylinder);

};

#endif // P_CHANGERADIUS_H
