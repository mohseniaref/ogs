/**
 * \file
 * \author Lars Bilke
 * \date   2014-08-12
 * \brief  Unit tests for In-Situ mesh source
 *
 * \copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "BaseLib/BuildInfo.h"

#include <numeric>

#include "FileIO/VtkIO/VtuInterface.h"

#include "InSituLib/VtkMappedMesh.h"
#include "InSituLib/VtkMappedMeshSource.h"

#include "MeshLib/Elements/Element.h"
#include "MeshLib/Mesh.h"
#include "MeshLib/MeshGenerators/MeshGenerator.h"
#include "MeshLib/MeshGenerators/VtkMeshConverter.h"

#include "gtest/gtest.h"

#include <vtkNew.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

// Creates a mesh with double and int point and cell properties
class InSituMesh : public ::testing::Test
{
	public:
	InSituMesh()
	 : mesh(nullptr)
	{
		mesh = MeshLib::MeshGenerator::generateRegularHexMesh(this->length, this->subdivisions);

		std::string const point_prop_name("PointDoubleProperty");
		boost::optional<MeshLib::PropertyVector<double> &> point_double_properties(
			mesh->getProperties().createNewPropertyVector<double>(point_prop_name,
				MeshLib::MeshItemType::Node)
		);
		(*point_double_properties).resize(mesh->getNNodes());
		std::iota((*point_double_properties).begin(), (*point_double_properties).end(), 1);

		std::string const cell_prop_name("CellDoubleProperty");
		boost::optional<MeshLib::PropertyVector<double> &> cell_double_properties(
			mesh->getProperties().createNewPropertyVector<double>(cell_prop_name,
				MeshLib::MeshItemType::Cell)
		);
		(*cell_double_properties).resize(mesh->getNElements());
		std::iota((*cell_double_properties).begin(), (*cell_double_properties).end(), 1);

		std::string const point_int_prop_name("PointIntProperty");
		boost::optional<MeshLib::PropertyVector<int> &> point_int_properties(
			mesh->getProperties().createNewPropertyVector<int>(point_int_prop_name,
				MeshLib::MeshItemType::Node)
		);
		(*point_int_properties).resize(mesh->getNNodes());
		std::iota((*point_int_properties).begin(), (*point_int_properties).end(), 1);

		std::string const cell_int_prop_name("CellIntProperty");
		boost::optional<MeshLib::PropertyVector<int> &> cell_int_properties(
			mesh->getProperties().createNewPropertyVector<int>(cell_int_prop_name,
				MeshLib::MeshItemType::Cell)
		);
		(*cell_int_properties).resize(mesh->getNElements());
		std::iota((*cell_int_properties).begin(), (*cell_int_properties).end(), 1);

		std::string const point_unsigned_prop_name("PointUnsignedProperty");
		boost::optional<MeshLib::PropertyVector<unsigned> &> point_unsigned_properties(
			mesh->getProperties().createNewPropertyVector<unsigned>(point_unsigned_prop_name,
				MeshLib::MeshItemType::Node)
		);
		(*point_unsigned_properties).resize(mesh->getNNodes());
		std::iota((*point_unsigned_properties).begin(), (*point_unsigned_properties).end(), 1);

		std::string const cell_unsigned_prop_name("CellUnsignedProperty");
		boost::optional<MeshLib::PropertyVector<unsigned> &> cell_unsigned_properties(
			mesh->getProperties().createNewPropertyVector<unsigned>(cell_unsigned_prop_name,
				MeshLib::MeshItemType::Cell)
		);
		(*cell_unsigned_properties).resize(mesh->getNElements());
		std::iota((*cell_unsigned_properties).begin(), (*cell_unsigned_properties).end(), 1);

		std::string const material_ids_name("MaterialIDs");
		boost::optional<MeshLib::PropertyVector<int> &> material_id_properties(
			mesh->getProperties().createNewPropertyVector<int>(material_ids_name,
				MeshLib::MeshItemType::Node)
		);
		(*material_id_properties).resize(mesh->getNNodes());
		std::iota((*material_id_properties).begin(), (*material_id_properties).end(), 1);
	}



	~InSituMesh()
	{
		delete mesh;
	}

	MeshLib::Mesh * mesh;
	const std::size_t subdivisions = 5;
	const double length = 1.0;
	const double dx = length / subdivisions;
};

TEST_F(InSituMesh, Construction)
{
	ASSERT_TRUE(mesh != nullptr);
	ASSERT_EQ((subdivisions+1)*(subdivisions+1)*(subdivisions+1), mesh->getNNodes());
}

// Maps the mesh into a vtkUnstructuredGrid-equivalent
TEST_F(InSituMesh, MappedMesh)
{
	ASSERT_TRUE(mesh != nullptr);

	vtkNew<InSituLib::VtkMappedMesh> vtkMesh;
	vtkMesh->GetImplementation()->SetNodes(mesh->getNodes());
	vtkMesh->GetImplementation()->SetElements(mesh->getElements());

	ASSERT_EQ(subdivisions*subdivisions*subdivisions, vtkMesh->GetNumberOfCells());
	ASSERT_EQ(VTK_HEXAHEDRON, vtkMesh->GetCellType(0));
	ASSERT_EQ(VTK_HEXAHEDRON, vtkMesh->GetCellType(vtkMesh->GetNumberOfCells()-1));
	ASSERT_EQ(1, vtkMesh->IsHomogeneous());
	ASSERT_EQ(8, vtkMesh->GetMaxCellSize());


	ASSERT_EQ(0, vtkMesh->GetNumberOfPoints()); // No points are defined
}

// Writes the mesh into a vtk file, reads the file back and converts it into a
// OGS mesh
TEST_F(InSituMesh, MappedMeshSourceRoundtrip)
{
	// TODO Add more comparison criteria

	ASSERT_TRUE(mesh != nullptr);
	std::string test_data_file(BaseLib::BuildInfo::tests_tmp_path + "/MappedMeshSourceRoundtrip.vtu");

	// -- Test VtkMappedMeshSource, i.e. OGS mesh to VTK mesh
	vtkNew<InSituLib::VtkMappedMeshSource> vtkSource;
	vtkSource->SetMesh(mesh);
	vtkSource->Update();
	vtkUnstructuredGrid* output = vtkSource->GetOutput();

	// Point and cell numbers
	ASSERT_EQ((subdivisions+1)*(subdivisions+1)*(subdivisions+1), output->GetNumberOfPoints());
	ASSERT_EQ(subdivisions*subdivisions*subdivisions, output->GetNumberOfCells());

	// Point data arrays
	vtkDataArray* pointDoubleArray = output->GetPointData()->GetScalars("PointDoubleProperty");
	ASSERT_EQ(pointDoubleArray->GetSize(), mesh->getNNodes());
	ASSERT_EQ(pointDoubleArray->GetComponent(0, 0), 1.0);
	double* range = pointDoubleArray->GetRange(0);
	ASSERT_EQ(range[0], 1.0);
	ASSERT_EQ(range[1], 1.0 + mesh->getNNodes() - 1.0);

	vtkDataArray* pointIntArray = output->GetPointData()->GetScalars("PointIntProperty");
	ASSERT_EQ(pointIntArray->GetSize(), mesh->getNNodes());
	ASSERT_EQ(pointIntArray->GetComponent(0, 0), 1.0);
	range = pointIntArray->GetRange(0);
	ASSERT_EQ(range[0], 1.0);
	ASSERT_EQ(range[1], 1 + mesh->getNNodes() - 1);

	vtkDataArray* pointUnsignedArray = output->GetPointData()->GetScalars("PointUnsignedProperty");
	ASSERT_EQ(pointUnsignedArray->GetSize(), mesh->getNNodes());
	ASSERT_EQ(pointUnsignedArray->GetComponent(0, 0), 1.0);
	range = pointUnsignedArray->GetRange(0);
	ASSERT_EQ(range[0], 1.0);
	ASSERT_EQ(range[1], 1 + mesh->getNNodes() - 1);

	// Cell data arrays
	vtkDataArray* cellDoubleArray = output->GetCellData()->GetScalars("CellDoubleProperty");
	ASSERT_EQ(cellDoubleArray->GetSize(), mesh->getNElements());
	ASSERT_EQ(cellDoubleArray->GetComponent(0, 0), 1.0);
	range = cellDoubleArray->GetRange(0);
	ASSERT_EQ(range[0], 1.0);
	ASSERT_EQ(range[1], 1.0 + mesh->getNElements() - 1.0);

	vtkDataArray* cellIntArray = output->GetCellData()->GetScalars("CellIntProperty");
	ASSERT_EQ(cellIntArray->GetSize(), mesh->getNElements());
	ASSERT_EQ(cellIntArray->GetComponent(0, 0), 1.0);
	range = cellIntArray->GetRange(0);
	ASSERT_EQ(range[0], 1.0);
	ASSERT_EQ(range[1], 1 + mesh->getNElements() - 1);

	vtkDataArray* cellUnsignedArray = output->GetCellData()->GetScalars("CellUnsignedProperty");
	ASSERT_EQ(cellUnsignedArray->GetSize(), mesh->getNElements());
	ASSERT_EQ(cellUnsignedArray->GetComponent(0, 0), 1.0);
	range = cellUnsignedArray->GetRange(0);
	ASSERT_EQ(range[0], 1.0);
	ASSERT_EQ(range[1], 1 + mesh->getNElements() - 1);

	// -- Write VTK mesh to file (in all combinations of binary, appended and compressed)
	// atm vtkXMLWriter::Appended does not work, see http://www.paraview.org/Bug/view.php?id=13382
	for(int dataMode : { vtkXMLWriter::Ascii, vtkXMLWriter::Binary })
	{
		for(bool compressed : { true, false })
		{
			if(dataMode == vtkXMLWriter::Ascii && compressed)
				continue;
			FileIO::VtuInterface vtuInterface(mesh, dataMode, compressed);
			ASSERT_TRUE(vtuInterface.writeToFile(test_data_file));

			// -- Read back VTK mesh
			vtkSmartPointer<vtkXMLUnstructuredGridReader> reader =
				vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
			reader->SetFileName(test_data_file.c_str());
			reader->Update();
			vtkUnstructuredGrid *vtkMesh = reader->GetOutput();

			// Both VTK meshes should be identical
			ASSERT_EQ(vtkMesh->GetNumberOfPoints(), output->GetNumberOfPoints());
			ASSERT_EQ(vtkMesh->GetNumberOfCells(), output->GetNumberOfCells());
			ASSERT_EQ(vtkMesh->GetPointData()->GetScalars("PointDoubleProperty")->GetNumberOfTuples(),
				pointDoubleArray->GetNumberOfTuples());
			ASSERT_EQ(vtkMesh->GetPointData()->GetScalars("PointIntProperty")->GetNumberOfTuples(),
				pointIntArray->GetNumberOfTuples());
			ASSERT_EQ(vtkMesh->GetPointData()->GetScalars("PointUnsignedProperty")->GetNumberOfTuples(),
					  pointUnsignedArray->GetNumberOfTuples());
			ASSERT_EQ(vtkMesh->GetCellData()->GetScalars("CellDoubleProperty")->GetNumberOfTuples(),
				cellDoubleArray->GetNumberOfTuples());
			ASSERT_EQ(vtkMesh->GetCellData()->GetScalars("CellIntProperty")->GetNumberOfTuples(),
				cellIntArray->GetNumberOfTuples());
			ASSERT_EQ(vtkMesh->GetCellData()->GetScalars("CellUnsignedProperty")->GetNumberOfTuples(),
					  cellUnsignedArray->GetNumberOfTuples());

			// Both OGS meshes should be identical
			MeshLib::Mesh *newMesh = MeshLib::VtkMeshConverter::convertUnstructuredGrid(vtkMesh);
			ASSERT_EQ(mesh->getNNodes(), newMesh->getNNodes());
			ASSERT_EQ(mesh->getNElements(), newMesh->getNElements());

			// Both properties should be identical
			auto meshProperties = mesh->getProperties();
			auto newMeshProperties = newMesh->getProperties();
			ASSERT_EQ(newMeshProperties.hasPropertyVector("PointDoubleProperty"),
				meshProperties.hasPropertyVector("PointDoubleProperty"));
			ASSERT_EQ(newMeshProperties.hasPropertyVector("PointIntProperty"),
				meshProperties.hasPropertyVector("PointIntProperty"));
			ASSERT_EQ(newMeshProperties.hasPropertyVector("PointUnsignedProperty"),
					  meshProperties.hasPropertyVector("PointUnsignedProperty"));
			ASSERT_EQ(newMeshProperties.hasPropertyVector("CellDoubleProperty"),
				meshProperties.hasPropertyVector("CellDoubleProperty"));
			ASSERT_EQ(newMeshProperties.hasPropertyVector("CellIntProperty"),
				meshProperties.hasPropertyVector("CellIntProperty"));
			ASSERT_EQ(newMeshProperties.hasPropertyVector("CellUnsignedProperty"),
					  meshProperties.hasPropertyVector("CellUnsignedProperty"));
			ASSERT_EQ(newMeshProperties.hasPropertyVector("MaterialIDs"),
				meshProperties.hasPropertyVector("MaterialIDs"));

			// Check some properties on equality
			auto doubleProps = meshProperties.getPropertyVector<double>("PointDoubleProperty");
			auto newDoubleProps = newMeshProperties.getPropertyVector<double>("PointDoubleProperty");
			ASSERT_EQ((*newDoubleProps).getTupleSize(), (*doubleProps).getTupleSize());
			ASSERT_EQ((*newDoubleProps).getNumberOfTuples(), (*doubleProps).getNumberOfTuples());
			ASSERT_EQ((*newDoubleProps).size(), (*doubleProps).size());
			for(std::size_t i = 0; i < (*doubleProps).size(); i++)
				ASSERT_EQ((*newDoubleProps)[i], (*doubleProps)[i]);

			auto unsignedProps = meshProperties.getPropertyVector<unsigned>("CellUnsignedProperty");
			auto newUnsignedIds = newMeshProperties.getPropertyVector<unsigned>("CellUnsignedProperty");

			ASSERT_EQ((*newUnsignedIds).getTupleSize(), (*unsignedProps).getTupleSize());
			ASSERT_EQ((*newUnsignedIds).getNumberOfTuples(), (*unsignedProps).getNumberOfTuples());
			ASSERT_EQ((*newUnsignedIds).size(), (*unsignedProps).size());
			for(std::size_t i = 0; i < (*unsignedProps).size(); i++)
				ASSERT_EQ((*newUnsignedIds)[i], (*unsignedProps)[i]);

			auto materialIds = meshProperties.getPropertyVector<int>("MaterialIDs");
			auto newMaterialIds = newMeshProperties.getPropertyVector<int>("MaterialIDs");
			ASSERT_EQ((*newMaterialIds).getTupleSize(), (*materialIds).getTupleSize());
			ASSERT_EQ((*newMaterialIds).getNumberOfTuples(), (*materialIds).getNumberOfTuples());
			ASSERT_EQ((*newMaterialIds).size(), (*materialIds).size());
			for(std::size_t i = 0; i < (*materialIds).size(); i++)
				ASSERT_EQ((*newMaterialIds)[i], (*materialIds)[i]);
		}
	}
}
