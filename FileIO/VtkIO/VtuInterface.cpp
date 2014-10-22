/**
 * \file
 * \author Lars Bilke
 * \date   2014-09-25
 * \brief  Implementation of the VtuInterface class.
 *
 * \copyright
 * Copyright (c) 2012-2014, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 */

#include "VtuInterface.h"

#include "logog/include/logog.hpp"

#include <vtkNew.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkSmartPointer.h>

#include "InSituLib/VtkMappedMeshSource.h"
#include "Mesh.h"
#include "MeshGenerators/VtkMeshConverter.h"

namespace FileIO
{

VtuInterface::VtuInterface(const MeshLib::Mesh* mesh, bool compress) :
	_mesh(mesh), _use_compressor(compress)
{
}

VtuInterface::~VtuInterface()
{}

MeshLib::Mesh* VtuInterface::readVTUFile(std::string const &file_name)
{
	vtkSmartPointer<vtkXMLUnstructuredGridReader> reader =
		vtkXMLUnstructuredGridReader::New();

	reader->SetFileName(file_name.c_str());
	reader->Update();

	vtkUnstructuredGrid* vtkGrid = reader->GetOutput();

	return MeshLib::VtkMeshConverter::convertUnstructuredGrid(vtkGrid);
}

bool VtuInterface::writeToFile(std::string const &file_name)
{
	if(!_mesh)
	{
		ERR("VtuInterface::write(): No mesh specified.");
		return false;
	}

	vtkNew<InSituLib::VtkMappedMeshSource> vtkSource;
	vtkSource->SetMesh(_mesh);

	vtkSmartPointer<vtkXMLUnstructuredGridWriter> vtuWriter =
		vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
	vtuWriter->SetInputConnection(vtkSource->GetOutputPort());
	if(_use_compressor)
		vtuWriter->SetCompressorTypeToZLib();

	// Setting binary file  mode, otherwise corrupted output due to VTK bug
	// See http://www.paraview.org/Bug/view.php?id=13382
	vtuWriter->SetDataModeToBinary();
	vtuWriter->SetFileName(file_name.c_str());
	return static_cast<bool>(vtuWriter->Write());
}

}