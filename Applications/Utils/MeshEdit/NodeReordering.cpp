/**
 * \file DataExplorer5NodeReordering.cpp
 * 2013/13/06 KR Initial implementation
 *
 * @copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/LICENSE.txt
 */

#include <array>
#include <algorithm>
#include <vector>

#include "logog/include/logog.hpp"
#include "LogogSimpleFormatter.h"

// TCLAP
#include "tclap/CmdLine.h"

// FileIO
#include "readMeshFromFile.h"
#include "FileIO/VtkIO/VtuInterface.h"

// MeshLib
#include "Mesh.h"
#include "Elements/Element.h"

/// Re-ordering mesh elements to correct Data Explorer 5 meshes to work with Data Explorer 6.
void reorderNodes(std::vector<MeshLib::Element*> &elements)
{
	std::size_t nElements (elements.size());
	for (std::size_t i=0; i<nElements; ++i)
	{
		const unsigned nElemNodes (elements[i]->getNBaseNodes());
		std::vector<MeshLib::Node*> nodes(elements[i]->getNodes(), elements[i]->getNodes() + nElemNodes);

		switch (elements[i]->getGeomType())
		{
			case MeshLib::MeshElemType::TETRAHEDRON:
				for(size_t j = 0; j < 4; ++j)
					elements[i]->setNode(j, nodes[(j+1)%4]);
				break;
			case MeshLib::MeshElemType::PYRAMID:
				for(size_t j = 0; j < 5; ++j)
					elements[i]->setNode(j, nodes[(j+1)%5]);
				break;
			case MeshLib::MeshElemType::PRISM:
				for(size_t j = 0; j < 3; ++j)
				{
					elements[i]->setNode(j, nodes[j+3]);
					elements[i]->setNode(j+3, nodes[j]);
				}
				break;
			case MeshLib::MeshElemType::HEXAHEDRON:
				for(size_t j = 0; j < 4; ++j)
				{
					elements[i]->setNode(j, nodes[j+4]);
					elements[i]->setNode(j+4, nodes[j]);
				}
				break;
			default:
				for(size_t j = 0; j < nElemNodes; ++j)
					elements[i]->setNode(j, nodes[nElemNodes - j - 1]);
		}
	}
}

/// Re-ordering prism elements to correct OGS6 meshes with and without InSitu-Lib
void reorderNodes2(std::vector<MeshLib::Element*> &elements)
{
	std::size_t nElements (elements.size());
	for (std::size_t i=0; i<nElements; ++i)
	{
		const unsigned nElemNodes (elements[i]->getNBaseNodes());
		std::vector<MeshLib::Node*> nodes(elements[i]->getNodes(), elements[i]->getNodes() + nElemNodes);

		for(size_t j = 0; j < nElemNodes; ++j)
			if (elements[i]->getGeomType() == MeshLib::MeshElemType::PRISM)
			{
				for(size_t j = 0; j < 3; ++j)
				{
					elements[i]->setNode(j, nodes[j+3]);
					elements[i]->setNode(j+3, nodes[j]);
				}
				break;
			}
	}
}

int main (int argc, char* argv[])
{
	LOGOG_INITIALIZE();
	logog::Cout* logogCout = new logog::Cout;
	BaseLib::LogogSimpleFormatter* formatter = new BaseLib::LogogSimpleFormatter;
	logogCout->SetFormatter(*formatter);

	TCLAP::CmdLine cmd("Reordering of mesh nodes to make OGS Data Explorer 5 meshes compatible with OGS6.\n" \
	                   "Method 1 is the re-ordering between DataExplorer 5 and DataExplorer 6 meshes,\n" \
	                   "Method 2 is the re-ordering with and without InSitu-Lib in OGS6.",
	                   ' ', "0.1");
	TCLAP::UnlabeledValueArg<std::string> input_mesh_arg("input_mesh",
	                                                     "the name of the input mesh file",
	                                                     true, "", "oldmesh.msh");
	cmd.add(input_mesh_arg);
	TCLAP::UnlabeledValueArg<std::string> output_mesh_arg("output_mesh",
	                                                      "the name of the output mesh file",
	                                                      true, "", "newmesh.vtu");
	cmd.add(output_mesh_arg);
	TCLAP::ValueArg<int> method_arg("m", "method", "reordering method selection", false,  1, "value");

	cmd.add(method_arg);
	cmd.parse(argc, argv);

	MeshLib::Mesh* mesh (FileIO::readMeshFromFile(input_mesh_arg.getValue().c_str()));

	INFO("Reordering nodes... ");
	if (!method_arg.isSet() || method_arg.getValue() == 1)
		reorderNodes(const_cast<std::vector<MeshLib::Element*>&>(mesh->getElements()));
	else if (method_arg.getValue() == 2)
		reorderNodes2(const_cast<std::vector<MeshLib::Element*>&>(mesh->getElements()));
	else
	{
		ERR ("Unknown re-ordering method. Exit program...");
		return 1;
	}

	FileIO::VtuInterface writer(mesh);
	writer.writeToFile(output_mesh_arg.getValue().c_str());

	INFO("VTU file written.");

	delete formatter;
	delete logogCout;
	LOGOG_SHUTDOWN();

	return 0;
}



