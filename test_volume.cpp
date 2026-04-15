#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkGenericCell.h>
#include <vtkIntegrateAttributes.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkQuadraticHexahedron.h>
#include <vtkQuadraticTetra.h>
#include <iostream>

const std::string root = PROJECT_ROOT;

double vtk_volume(vtkSmartPointer<vtkUnstructuredGrid> grid)
{
  vtkNew<vtkIntegrateAttributes> integrator;
  integrator->SetInputData(grid);
  integrator->Update();

  vtkUnstructuredGrid* output = integrator->GetOutput();

  double volume = output->GetCellData()->GetArray("Volume")->GetComponent(0, 0);
  return volume;
}

// Check if vtk only implementation yields wrong results.
bool test_quadratic_hex()
{
 // We want to reproduce the following (wrong) result in vtk only
 // The correct value from cgx would be 8666.666
 std::cout << "Test Computing Quadratic Hexahedron volumes" << std::endl;
 std::cout << "===========================================" << std::endl;

 std::map<int, std::array<double, 3>> nodes = {
     {486, {80.0, 80.0, 80.0}},    {491, {80.0, 100.0, 80.0}},
     {492, {80.0, 90.0, 80.0}},    {508, {100.0, 80.0, 80.0}},
     {511, {90.0, 80.0, 80.0}},    {512, {110.0, 100.0, 80.0}},
     {513, {100.0, 90.0, 80.0}},   {515, {90.0, 100.0, 80.0}},
     {592, {80.0, 80.0, 100.0}},   {596, {80.0, 80.0, 90.0}},
     {599, {80.0, 100.0, 100.0}},  {601, {80.0, 90.0, 100.0}},
     {603, {80.0, 100.0, 90.0}},   {713, {100.0, 80.0, 100.0}},
     {715, {100.0, 80.0, 90.0}},   {716, {90.0, 80.0, 100.0}},
     {717, {110.0, 100.0, 100.0}}, {718, {100.0, 90.0, 100.0}},
     {719, {110.0, 100.0, 90.0}},  {720, {90.0, 100.0, 100.0}}};

 std::vector<int> elem = {
     486, 592, 713, 508, 491, 599, 717, 512, // 8 corners
     596, 716, 715, 511, 603, 720, 719,      // mids
     515, 492, 601, 718, 513                 // mids
 };

 auto points = vtkSmartPointer<vtkPoints>::New();
 std::map<int, vtkIdType> idMap;

 vtkIdType vtkId = 0;
 for (auto &p : nodes) {
   points->InsertNextPoint(p.second[0], p.second[1], p.second[2]);
   idMap[p.first] = vtkId++;
 }

  auto hex = vtkSmartPointer<vtkQuadraticHexahedron>::New();

  for (unsigned int i = 0; i < elem.size(); ++i) {
    hex->GetPointIds()->SetId(i, idMap[elem[i]]);
  }

  auto ug = vtkSmartPointer<vtkUnstructuredGrid>::New();
  ug->SetPoints(points);
  ug->InsertNextCell(hex->GetCellType(), hex->GetPointIds());

  double volume = vtk_volume(ug);

  std::cout << "Computed volume using vtkIntegrate attributes" << std::endl
            <<  volume<< std::endl;
  std::cout << "Correct volume (computed in cgx and by hand)" << std::endl
            << 8.666667e+03 << std::endl;
  std::cout << "Absolute deviation: " << volume - 8.666667e+03
            << std::endl;
  std::cout << "Relative deviation: " << (volume - 8.666667e+03) / volume
  << std::endl;
  return true;
}

bool test_quadratic_tet()
{ // CGX gives out the volume 2.298627e+04 for the following tetrahedron: abq_quadratic_tet.inp

 std::cout << "Test Computing Quadratic Hexahedron volumes" << std::endl;
 std::cout << "===========================================" << std::endl;

  // Construct the vtkUnstructuredGrid by hand and verify deviation
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

  double coords[10][3] = {
    {0, 0, 0},  // 0
    {100, 0, 0},  // 1
    {0, 100, 0},  // 2
    {0, 0, 100},  // 3
    {50, 25, 0},  // 4  (0-1)
    {50, 50, 0},  // 5 (1-2)
    {0, 50, 0},   // 6 (0-2)
    {0, 0, 50},   // 7
    {50, 0, 50},  // 8
    {0, 50, 50}   // 9
  };

  std::cout << coords[4][1]<< std::endl;
  for (int i = 0; i < 10; ++i)
    points->InsertNextPoint(coords[i][0], coords[i][1], coords[i][2]);

  vtkSmartPointer<vtkQuadraticTetra> qtet =
    vtkSmartPointer<vtkQuadraticTetra>::New();

  for (int i = 0; i < 10; ++i)
    qtet->GetPointIds()->SetId(i, i);

  vtkSmartPointer<vtkUnstructuredGrid> ug =
    vtkSmartPointer<vtkUnstructuredGrid>::New();

  ug->SetPoints(points);
  ug->InsertNextCell(qtet->GetCellType(), qtet->GetPointIds());

  double volume = vtk_volume(ug);

  std::cout << "Computed volume using vtkIntegrate attributes" << std::endl
            << volume << std::endl;
  std::cout << "Correct volume (computed in cgx and by hand)" << std::endl
            <<  2.298627e+04<< std::endl;
  std::cout << "Absolute deviation: " << std::abs(volume - 2.298627e+04)
            << std::endl;
  std::cout << "Relative deviation: " << std::abs(volume - 2.298627e+04) / volume
  << std::endl;
  return true;
}

int main() {
  bool success = test_quadratic_hex();
  success = success && test_quadratic_tet();
  return success ? 0 : 1;
}
