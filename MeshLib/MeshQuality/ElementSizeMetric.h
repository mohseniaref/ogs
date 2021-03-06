/**
 * \file   ElementSizeMetric.h
 * \author Karsten Rink
 * \date   2011-03-17
 * \brief  Implementation of the AreaMetric class.
 *
 * \copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#ifndef ELEMENTSIZEMETRIC_H_
#define ELEMENTSIZEMETRIC_H_

#include "ElementQualityMetric.h"

namespace MeshLib
{

/** 
 * Calculates the quality of mesh elements based on length/area/volume
 */
class ElementSizeMetric : public ElementQualityMetric
{
public:
	ElementSizeMetric(Mesh const& mesh);
	virtual ~ElementSizeMetric() {}

	virtual void calculateQuality ();

private:
	std::size_t calc1dQuality();
	std::size_t calc2dQuality();
	std::size_t calc3dQuality();
};
}

#endif /* ELEMENTSIZEMETRIC_H_ */
