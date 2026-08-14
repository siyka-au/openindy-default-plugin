#ifndef P_REGISTER_H
#define P_REGISTER_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The Register class
 */
class Register : public ObjectTransformation, public FunctionUtil, public Transform<Point>, public Transform<Sphere>, public Transform<Circle>
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

    std::expected<Point, QString> exec(const Point &input) override;
    std::expected<Sphere, QString> exec(const Sphere &input) override;
    std::expected<Circle, QString> exec(const Circle &input) override;

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Point &point);
    bool setUpResult(Sphere &sphere);
    bool setUpResult(Circle &circle);

};

#endif // P_REGISTER_H
