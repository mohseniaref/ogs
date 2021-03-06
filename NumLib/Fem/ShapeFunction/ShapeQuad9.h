/**
 * \copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#ifndef SHAPEQUAD9_H_
#define SHAPEQUAD9_H_

#include "MeshLib/Elements/Quad.h"

namespace NumLib
{

/**
 *  Shape function for a 9-nodes quadrilateral element
 */
class ShapeQuad9
{
public:
    /**
     * Evaluate the shape function at the given point
     *
     * @param [in]  r    point coordinates
     * @param [out] N   a vector of calculated shape function.
     */
    template <class T_X, class T_N>
    static void computeShapeFunction(const T_X &r, T_N &N);

    /**
     * Evaluate derivatives of the shape function at the given point
     *
     * @param [in]  r    point coordinates
     * @param [out] dN  a matrix of the derivatives
     */
    template <class T_X, class T_N>
    static void computeGradShapeFunction(const T_X &r, T_N &dN);

    using MeshElement = MeshLib::Quad9;
    static const std::size_t DIM = MeshElement::dimension;
    static const std::size_t NPOINTS = MeshElement::n_all_nodes;
};

}

#include "ShapeQuad9-impl.h"

#endif //SHAPEQUAD9_H_
