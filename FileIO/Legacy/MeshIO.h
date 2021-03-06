/**
 * \file
 * \author Karsten Rink
 * \date   2012-05-08
 * \brief  Definition of the MeshIO class.
 *
 * \copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */


#ifndef MESHIO_H_
#define MESHIO_H_

#include <iostream>
#include <string>
#include <vector>
#include <boost/optional.hpp>

#include "FileIO/Writer.h"
#include "MeshLib/MeshEnums.h"
#include "MeshLib/PropertyVector.h"

namespace MeshLib
{
	class Mesh;
	class Node;
	class Element;
	enum class MeshElemType;
}


namespace FileIO
{
namespace Legacy {

/// Interface for handling mesh files from OGS-5 and below. (*.msh files)
class MeshIO : public Writer
{
public:
	/// Constructor.
	MeshIO();

	virtual ~MeshIO() {}

	/// Read mesh from file.
	MeshLib::Mesh* loadMeshFromFile(const std::string& fileName);

	/// Set mesh for writing.
	void setMesh(const MeshLib::Mesh*  mesh);

protected:
	/// Write mesh to stream.
	bool write();

private:
	void writeElements(std::vector<MeshLib::Element*> const& ele_vec,
		boost::optional<MeshLib::PropertyVector<int> const&> material_ids,
		std::ostream &out) const;
	std::size_t readMaterialID(std::istream & in) const;
	MeshLib::Element* readElement(std::istream& line, const std::vector<MeshLib::Node*> &nodes) const;
	std::string ElemType2StringOutput(const MeshLib::MeshElemType t) const;

	const MeshLib::Mesh* _mesh;

};  /* class */

}
} /* namespace FileIO */

#endif /* MESHIO_H_ */

