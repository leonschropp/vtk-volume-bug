# Incorrect Volumes for quadratic Elements at VTK

  It appears that VTK still gives out an incorrect volume for [QuadraticHexahedrons](https://vtk.org/doc/nightly/html/classvtkQuadraticHexahedron.html) and [QuadraticTetrahedrons](https://vtk.org/doc/nightly/html/classvtkQuadraticTetra.html) 
This is verified by the github workflow run of this repository, where the current VTK-master is build and the file `test_volume.cpp` is run. 

In this test, I first construct a QuadraticHexahedron using basic VTK methods and calculate its volume using the filter 'vtkIntegrateAttributes'. The result that VTK gives is **9333.333...**.

However, by integrating over the quadratic function for the 'pulled out' node, one easily verifies that the correct volume is **8666.666**!

The same result is obtained by passing this Hexahedron to Calculix (cgx)(https://www.calculix.de/) or Paraview
(https://www.paraview.org/) and using their volume functions. Both **CGX** and **Paraview** give out **8666.666**!

A similar deviation appears for QuadraticTetraHedrons.

## Verification with CGX

Follow this procedure to verify that the volume of this quadratic Hexahedron is **8666.66** with Calculix (cgx).

0. Install cgx if not done already

		sudo apt install calculix-cgx


1. Consider the following .inp file:

		*NODE
			   486,  80.0           ,  80.0           ,  80.0
			   491,  80.0           ,  100.0          ,  80.0
			   492,  80.0           ,  90.0           ,  80.0
			   508,  100.0          ,  80.0           ,  80.0
			   511,  90.0           ,  80.0           ,  80.0
			   512,  110.0          ,  100.0          ,  80.0
			   513,  100.0          ,  90.0           ,  80.0
			   515,  90.0           ,  100.0          ,  80.0
			   592,  80.0           ,  80.0           ,  100.0
			   596,  80.0           ,  80.0           ,  90.0
			   599,  80.0           ,  100.0          ,  100.0
			   601,  80.0           ,  90.0           ,  100.0
			   603,  80.0           ,  100.0          ,  90.0
			   713,  100.0          ,  80.0           ,  100.0
			   715,  100.0          ,  80.0           ,  90.0
			   716,  90.0           ,  80.0           ,  100.0
			   717,  110.0          ,  100.0          ,  100.0
			   718,  100.0          ,  90.0           ,  100.0
			   719,  110.0          ,  100.0          ,  90.0
			   720,  90.0           ,  100.0          ,  100.0
		**HW_COMPONENT     ID=1     COLOR=4     NAME=cube
		*ELEMENT,TYPE=C3D20
			   140,       486,       592,       713,       508,       491,       599,       717,       512,
			   596,       716,       715,       511,       603,       720,       719,
			   515,       492,       601,       718,       513
		**HW_SET     ID=5
		*NSET, NSET=NFIX_Z
		**HW_SET     ID=4
		*NSET, NSET=NFIX_Y
		**HW_SET     ID=3
		*NSET, NSET=NFIX_X
		**HW_SET     ID=2
		*NSET, NSET=NALL
			   486,       491,       492,       508,       511,       512,       513,       515,
			   592,       596,       599,       601,       603,       713,       715,       716,
			   717,       718,       719,       720
		*ELSET, ELSET=cube
		   140,

2. You may save this .inp file as 'hex.inp'.

3. Open the file in cgx: 
		cgx -c 'hex.inp'

4. In the cgx-Viewer left-click and hold to select `Toggle-Commandline`. You should see `:` at the bottom of the window now. This is the cgx-commandline

5. At the cgx-commandline type `volu cube` and hit enter

6. At the window you launched cgx from you should now see the output

			 Elements: 1 Nodes:20 Datasets:0  MinElemNr: 140 MaxElemNr: 140 MinNodNr:486 MaxNodNr:720  read in 0.020798 sec 
			 found elements of type 1: 0
			 found elements of type 2: 0
			 found elements of type 3: 0
			 found elements of type 4: 1
			 found elements of type 5: 0
			 found elements of type 6: 0
			 found elements of type 7: 0
			 found elements of type 8: 0
			 found elements of type 9: 0
			 found elements of type 10: 0
			 found elements of type 11: 0
			 found elements of type 12: 0
			gtol calculated:3.000000e-04
			VOLUME:8.666667e+03 CENTER OF GRAVITY: 9.107692e+01 9.076923e+01 9.000000e+01

The last line indicates that the volume is 8666.666...
