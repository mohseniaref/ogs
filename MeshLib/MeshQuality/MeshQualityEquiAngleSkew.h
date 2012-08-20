/*
 * MeshQualityEquiAngleSkew.h
 *
 *  Created on: Mar 17, 2011
 *      Author: TF
 */

#ifndef MESHQUALITYEQUIANGLESKEW_H_
#define MESHQUALITYEQUIANGLESKEW_H_

#include "MeshQualityChecker.h"

namespace MeshLib
{
class MeshQualityEquiAngleSkew : public MeshLib::MeshQualityChecker
{
public:
	MeshQualityEquiAngleSkew(Mesh const* const mesh);
	virtual ~MeshQualityEquiAngleSkew();

	virtual void check ();

private:
	double checkTriangle(Element const* const elem) const;
	double checkQuad(Element const* const elem) const;
	double checkTetrahedron(Element const* const elem) const;
	double checkHexahedron(Element const* const elem) const;
	double checkPrism (Element const* const elem) const;
	void getMinMaxAngleFromQuad(double const* const n0,
	                            double const* const n1, double const* const n2,
	                            double const* const n3, double &min_angle,
	                            double &max_angle) const;
	void getMinMaxAngleFromTriangle(double const* const n0,
	                                double const* const n1, double const* const n2,
	                                double &min_angle, double &max_angle) const;

	const double M_PI_THIRD;
	const double TWICE_M_PI;
};
}

#endif /* MESHQUALITYEQUIANGLESKEW_H_ */