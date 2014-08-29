/**
 * \file   Calculation of a minimum bounding sphere for a vector of points
 * \author Karsten Rink
 * \date   2014-07-11
 * \brief  Implementation of the BoundingSphere class.
 *
 * \copyright
 * Copyright (c) 2013, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "BoundingSphere.h"

// ThirdParty/logog
#include "logog/include/logog.hpp"

#include "MathTools.h"
#include "AnalyticalGeometry.h"

namespace GeoLib {

BoundingSphere::BoundingSphere()
: _center(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()), _radius(-1)
{	
}

BoundingSphere::BoundingSphere(const BoundingSphere &sphere)
: _center(sphere.getCenter()), _radius(sphere.getRadius())
{
}

BoundingSphere::BoundingSphere(const GeoLib::Point &p)
: _center(p), _radius(std::numeric_limits<double>::epsilon())
{
}

BoundingSphere::BoundingSphere(const GeoLib::Point &p, double radius)
: _center(p), _radius(radius)
{
}

BoundingSphere::BoundingSphere(const GeoLib::Point &p, const GeoLib::Point &q)
: _center(p), _radius(std::numeric_limits<double>::epsilon())
{
	const MathLib::Vector3 a(p, q);

    if (a.getLength() > 0)
    {
	    const MathLib::Vector3 o(0.5*a);
	    _radius = o.getLength() + std::numeric_limits<double>::epsilon();
	    _center = MathLib::Vector3(p) + o;
    }
}

BoundingSphere::BoundingSphere(const GeoLib::Point &p, const GeoLib::Point &q,  const GeoLib::Point &r)
{
	const MathLib::Vector3 a(p,r);
	const MathLib::Vector3 b(p,q);

	const MathLib::Vector3 cross_ab(crossProduct(a,b));

    if (cross_ab.getLength() > 0)
    {
	    const double denom = 2.0 * scalarProduct(cross_ab,cross_ab);
	    const MathLib::Vector3 o = (scalarProduct(b,b) * crossProduct(cross_ab, a) 
	 	                          + scalarProduct(a,a) * crossProduct(b, cross_ab))
		                          * (1.0 / denom);
	    _radius = o.getLength() + std::numeric_limits<double>::epsilon();
	    _center = MathLib::Vector3(p) + o;
    }
    else
    {
        BoundingSphere two_pnts_sphere;
        if (a.getLength() > b.getLength())
            two_pnts_sphere = BoundingSphere(p,r);
        else
            two_pnts_sphere = BoundingSphere(p,q);
        _radius = two_pnts_sphere.getRadius();
	    _center = two_pnts_sphere.getCenter();
    }
}

BoundingSphere::BoundingSphere(const GeoLib::Point &p, const GeoLib::Point &q, const GeoLib::Point &r, const GeoLib::Point &s)
{
	const MathLib::Vector3 a(p, q);
	const MathLib::Vector3 b(p, r);
	const MathLib::Vector3 c(p, s);

    if (!GeoLib::isCoplanar(p, q, r, s))
    {
    	// det of matrix [a^T, b^T, c^T]^T
	    const double denom = 2.0 * (a[0] * (b[1] * c[2] - c[1] * b[2])
							      - b[0] * (a[1] * c[2] - c[1] * a[2])
							      + c[0] * (a[1] * b[2] - b[1] * a[2]));
	    const MathLib::Vector3 o = (scalarProduct(c,c) * crossProduct(a,b) 
							      + scalarProduct(b,b) * crossProduct(c,a)
							      + scalarProduct(a,a) * crossProduct(b,c)) 
							    * (1.0 / denom);

	    _radius = o.getLength() + std::numeric_limits<double>::epsilon();
	    _center = MathLib::Vector3(p) + o;
    }
    else
    {
        BoundingSphere pqr(p, q , r);
        BoundingSphere pqs(p, q , s);
        BoundingSphere prs(p, r , s);
        BoundingSphere qrs(q, r , s);
        _radius = pqr.getRadius();
        _center = pqr.getCenter();
        if (_radius < pqs.getRadius())
        {
            _radius = pqs.getRadius();
            _center = pqs.getCenter();
        }
        if (_radius < prs.getRadius())
        {
            _radius = prs.getRadius();
            _center = prs.getCenter();
        }
        if (_radius < qrs.getRadius())
        {
            _radius = qrs.getRadius();
            _center = qrs.getCenter();
        }
    }
}

BoundingSphere::BoundingSphere(const std::vector<GeoLib::Point*> &points)
: _center(0,0,0), _radius(-1)
{
	const std::size_t n_points (points.size());
	GeoLib::Point **sphere_points = new GeoLib::Point*[n_points];
	for(unsigned int i = 0; i < n_points; i++)
		sphere_points[i] = points[i];

	const BoundingSphere bounding_sphere = recurseCalculation(points, 0, points.size(), 0);
	delete[] sphere_points;
	
	this->_center = bounding_sphere.getCenter();
	this->_radius = bounding_sphere.getRadius();
}

BoundingSphere BoundingSphere::recurseCalculation(std::vector<GeoLib::Point*> sphere_points, std::size_t current_index, std::size_t n_points, std::size_t n_boundary_points)
{
	BoundingSphere sphere;
	switch(n_boundary_points)
	{
	case 0:
		sphere = BoundingSphere();
		break;
	case 1:
		sphere = BoundingSphere(*sphere_points[current_index-1]);
		break;
	case 2:
		sphere = BoundingSphere(*sphere_points[current_index-1], *sphere_points[current_index-2]);
		break;
	case 3:
		sphere = BoundingSphere(*sphere_points[current_index-1], *sphere_points[current_index-2], *sphere_points[current_index-3]);
		break;
	case 4:
		sphere = BoundingSphere(*sphere_points[current_index-1], *sphere_points[current_index-2], *sphere_points[current_index-3], *sphere_points[current_index-4]);
		return sphere;
	}

	for(std::size_t i=current_index; i<n_points; ++i)
	{
        // current point is located outside of sphere
		if(sphere.sqrPointDist(*sphere_points[i]) > 0)
		{
			GeoLib::Point* tmp = sphere_points[i];
            std::copy(sphere_points.begin(), sphere_points.begin() + i, sphere_points.begin() + 1);
            sphere_points[0] = tmp;

    		sphere = recurseCalculation(sphere_points, current_index+1, i, n_boundary_points+1);
		}
	}
	return sphere;
}

double BoundingSphere::sqrPointDist(const GeoLib::Point pnt) const
{
	return MathLib::sqrDist(_center.getCoords(), pnt.getCoords())-(_radius*_radius);
}

std::vector<GeoLib::Point*>* BoundingSphere::getRandomSpherePoints(std::size_t n_points) const
{
	std::vector<GeoLib::Point*> *pnts = new std::vector<GeoLib::Point*>;
	pnts->reserve(n_points);
	srand ( static_cast<unsigned>(time(NULL)) );

	for (std::size_t k(0); k<n_points; ++k) 
	{
		MathLib::Vector3 vec (0,0,0);
		double sum (0);
		for (unsigned i=0; i<3; ++i)
		{
			vec[i] = (double)rand()-(RAND_MAX/2.0);
			sum+=(vec[i]*vec[i]);
		}
		double fac (this->_radius/sqrt(sum));
		pnts->push_back(new GeoLib::Point(_center[0]+vec[0]*fac, _center[1]+vec[1]*fac, _center[2]+vec[2]*fac));
	}
	return pnts;
}

}