/**
 * \copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#ifndef SHAPEHEX20_H_
#define SHAPEHEX20_H_

#include "MeshLib/Elements/Hex.h"

namespace NumLib
{

/**
 *  Shape function for a 20-nodes hex element in natural coordinates
 *
 */
class ShapeHex20
{
public:
    /**
     * Evaluate the shape function at the given point
     *
     * @param [in]  r   natural coordinates (r,s,t)
     * @param [out] N   a vector of calculated shape functions
     */
    template <class T_X, class T_N>
    static void computeShapeFunction(const T_X &r, T_N &N);

    /**
     * Evaluate derivatives of the shape function at the given point
     *
     * @param [in]  r   natural coordinates (r,s,t)
     * @param [out] dN  a matrix of the derivatives
     */
    template <class T_X, class T_N>
    static void computeGradShapeFunction(const T_X &r, T_N &dN);

    using MeshElement = MeshLib::Hex20;
    static const std::size_t DIM = MeshElement::dimension;
    static const std::size_t NPOINTS = MeshElement::n_all_nodes;
};

}

#include "ShapeHex20-impl.h"

#endif //SHAPEHEX20_H_
