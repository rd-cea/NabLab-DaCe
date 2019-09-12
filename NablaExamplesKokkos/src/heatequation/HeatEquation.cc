#include <iostream>
#include <iomanip>
#include <type_traits>
#include <limits>
#include <utility>
#include <cmath>
#include <cfenv>
#pragma STDC FENV_ACCESS ON

// Kokkos headers
#include <Kokkos_Core.hpp>
#include <Kokkos_hwloc.hpp>

// Project headers
#include "mesh/NumericMesh2D.h"
#include "mesh/CartesianMesh2DGenerator.h"
#include "mesh/VtkFileWriter2D.h"
#include "utils/Utils.h"
#include "utils/Timer.h"
#include "types/Types.h"
#include "types/MathFunctions.h"
#include "types/ArrayOperations.h"

using namespace nablalib;

class HeatEquation
{
public:
	struct Options
	{
		// Should be const but usefull to set them from main args
		double X_EDGE_LENGTH = 0.1;
		double Y_EDGE_LENGTH = as_const(X_EDGE_LENGTH);
		int X_EDGE_ELEMS = 20;
		int Y_EDGE_ELEMS = 20;
		int Z_EDGE_ELEMS = 1;
		double option_stoptime = 0.1;
		int option_max_iterations = 500;
		double PI = 3.1415926;
		double k = 1.0;
	};
	Options* options;

private:
	int iteration;
	NumericMesh2D* mesh;
	VtkFileWriter2D writer;
	int nbNodes, nbCells, nbFaces, nbNodesOfCell, nbNodesOfFace, nbNeighbourCells;

	// Global Variables
	double t, deltat, t_nplus1;

	// Connectivity Variables
	Kokkos::View<RealArray1D<2>*> X;
	Kokkos::View<RealArray1D<2>*> center;
	Kokkos::View<double*> u;
	Kokkos::View<double*> V;
	Kokkos::View<double*> f;
	Kokkos::View<double*> outgoingFlux;
	Kokkos::View<double*> surface;
	Kokkos::View<double*> u_nplus1;
	
	const size_t maxHardThread = Kokkos::DefaultExecutionSpace::max_hardware_threads();

public:
	HeatEquation(Options* aOptions, NumericMesh2D* aNumericMesh2D, string output)
	: options(aOptions)
	, mesh(aNumericMesh2D)
	, writer("HeatEquation", output)
	, nbNodes(mesh->getNbNodes())
	, nbCells(mesh->getNbCells())
	, nbFaces(mesh->getNbFaces())
	, nbNodesOfCell(NumericMesh2D::MaxNbNodesOfCell)
	, nbNodesOfFace(NumericMesh2D::MaxNbNodesOfFace)
	, nbNeighbourCells(NumericMesh2D::MaxNbNeighbourCells)
	, t(0.0)
	, deltat(0.001)
	, t_nplus1(0.0)
	, X("X", nbNodes)
	, center("center", nbCells)
	, u("u", nbCells)
	, V("V", nbCells)
	, f("f", nbCells)
	, outgoingFlux("outgoingFlux", nbCells)
	, surface("surface", nbFaces)
	, u_nplus1("u_nplus1", nbCells)
	{
		// Copy node coordinates
		const auto& gNodes = mesh->getGeometricMesh()->getNodes();
		Kokkos::parallel_for(nbNodes, KOKKOS_LAMBDA(const int& rNodes)
		{
			X(rNodes) = gNodes[rNodes];
		});
	}

private:
	/**
	 * Job IniF @-2.0
	 * In variables: 
	 * Out variables: f
	 */
	KOKKOS_INLINE_FUNCTION
	void iniF() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& jCells)
		{
			f(jCells) = 0.0;
		});
	}
	
	/**
	 * Job IniCenter @-2.0
	 * In variables: X
	 * Out variables: center
	 */
	KOKKOS_INLINE_FUNCTION
	void iniCenter() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& jCells)
		{
			int jId(jCells);
			RealArray1D<2> reduceSum_906155090 = {0.0, 0.0};
			{
				auto nodesOfCellJ(mesh->getNodesOfCell(jId));
				for (int rNodesOfCellJ=0; rNodesOfCellJ<nodesOfCellJ.size(); rNodesOfCellJ++)
				{
					int rId(nodesOfCellJ[rNodesOfCellJ]);
					int rNodes(rId);
					reduceSum_906155090 = ArrayOperations::plus(reduceSum_906155090, (X(rNodes)));
				}
			}
			center(jCells) = ArrayOperations::multiply(0.25, reduceSum_906155090);
		});
	}
	
	/**
	 * Job ComputeV @-2.0
	 * In variables: X
	 * Out variables: V
	 */
	KOKKOS_INLINE_FUNCTION
	void computeV() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& jCells)
		{
			int jId(jCells);
			double reduceSum1062049054 = 0.0;
			{
				auto nodesOfCellJ(mesh->getNodesOfCell(jId));
				for (int rNodesOfCellJ=0; rNodesOfCellJ<nodesOfCellJ.size(); rNodesOfCellJ++)
				{
					int rId(nodesOfCellJ[rNodesOfCellJ]);
					int rPlus1Id(nodesOfCellJ[(rNodesOfCellJ+1+nbNodesOfCell)%nbNodesOfCell]);
					int rNodes(rId);
					int rPlus1Nodes(rPlus1Id);
					reduceSum1062049054 = reduceSum1062049054 + (MathFunctions::det(X(rNodes), X(rPlus1Nodes)));
				}
			}
			V(jCells) = 0.5 * reduceSum1062049054;
		});
	}
	
	/**
	 * Job ComputeSurface @-2.0
	 * In variables: X
	 * Out variables: surface
	 */
	KOKKOS_INLINE_FUNCTION
	void computeSurface() noexcept
	{
		Kokkos::parallel_for(nbFaces, KOKKOS_LAMBDA(const int& fFaces)
		{
			int fId(fFaces);
			double reduceSum898849778 = 0.0;
			{
				auto nodesOfFaceF(mesh->getNodesOfFace(fId));
				for (int rNodesOfFaceF=0; rNodesOfFaceF<nodesOfFaceF.size(); rNodesOfFaceF++)
				{
					int rId(nodesOfFaceF[rNodesOfFaceF]);
					int rPlus1Id(nodesOfFaceF[(rNodesOfFaceF+1+nbNodesOfFace)%nbNodesOfFace]);
					int rNodes(rId);
					int rPlus1Nodes(rPlus1Id);
					reduceSum898849778 = reduceSum898849778 + (MathFunctions::norm(ArrayOperations::minus(X(rNodes), X(rPlus1Nodes))));
				}
			}
			surface(fFaces) = 0.5 * reduceSum898849778;
		});
	}
	
	/**
	 * Job IniUn @-1.0
	 * In variables: PI, k, center
	 * Out variables: u
	 */
	KOKKOS_INLINE_FUNCTION
	void iniUn() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& jCells)
		{
			u(jCells) = MathFunctions::cos(2 * as_const(options->PI) * as_const(options->k) * center(jCells)[0]);
		});
	}
	
	/**
	 * Job ComputeOutgoingFlux @1.0
	 * In variables: u, center, surface, deltat, V
	 * Out variables: outgoingFlux
	 */
	KOKKOS_INLINE_FUNCTION
	void computeOutgoingFlux() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& j1Cells)
		{
			int j1Id(j1Cells);
			double reduceSum1795706568 = 0.0;
			{
				auto neighbourCellsJ1(mesh->getNeighbourCells(j1Id));
				for (int j2NeighbourCellsJ1=0; j2NeighbourCellsJ1<neighbourCellsJ1.size(); j2NeighbourCellsJ1++)
				{
					int j2Id(neighbourCellsJ1[j2NeighbourCellsJ1]);
					int j2Cells(j2Id);
					int cfId(mesh->getCommonFace(j1Id, j2Id));
					int cfFaces(cfId);
					reduceSum1795706568 = reduceSum1795706568 + ((u(j2Cells) - u(j1Cells)) / MathFunctions::norm(ArrayOperations::minus(center(j2Cells), center(j1Cells))) * surface(cfFaces));
				}
			}
			outgoingFlux(j1Cells) = as_const(deltat) / V(j1Cells) * reduceSum1795706568;
		});
	}
	
	/**
	 * Job ComputeTn @1.0
	 * In variables: t, deltat
	 * Out variables: t_nplus1
	 */
	KOKKOS_INLINE_FUNCTION
	void computeTn() noexcept
	{
		t_nplus1 = as_const(t) + as_const(deltat);
	}
	
	/**
	 * Job dumpVariables @1.0
	 * In variables: u
	 * Out variables: 
	 */
	KOKKOS_INLINE_FUNCTION
	void dumpVariables() noexcept
	{
		if (!writer.isDisabled()) {
			std::map<string, double*> cellVariables;
			std::map<string, double*> nodeVariables;
			cellVariables.insert(pair<string,double*>("Temperature", u.data()));
			auto quads = mesh->getGeometricMesh()->getQuads();
			writer.writeFile(iteration, nbNodes, X.data(), nbCells, quads.data(), cellVariables, nodeVariables);
		}
	}
	
	/**
	 * Job Copy_t_nplus1_to_t @2.0
	 * In variables: t_nplus1
	 * Out variables: t
	 */
	KOKKOS_INLINE_FUNCTION
	void copy_t_nplus1_to_t() noexcept
	{
		std::swap(t_nplus1, t);
	}
	
	/**
	 * Job ComputeUn @2.0
	 * In variables: f, deltat, u, outgoingFlux
	 * Out variables: u_nplus1
	 */
	KOKKOS_INLINE_FUNCTION
	void computeUn() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& jCells)
		{
			u_nplus1(jCells) = f(jCells) * as_const(deltat) + u(jCells) + outgoingFlux(jCells);
		});
	}
	
	/**
	 * Job Copy_u_nplus1_to_u @3.0
	 * In variables: u_nplus1
	 * Out variables: u
	 */
	KOKKOS_INLINE_FUNCTION
	void copy_u_nplus1_to_u() noexcept
	{
		std::swap(u_nplus1, u);
	}

public:
	void simulate()
	{
		std::cout << "\n" << __BLUE_BKG__ << __YELLOW__ << __BOLD__ <<"\tStarting HeatEquation ..." << __RESET__ << "\n\n";

		std::cout << "[" << __GREEN__ << "MESH" << __RESET__ << "]      X=" << __BOLD__ << options->X_EDGE_ELEMS << __RESET__ << ", Y=" << __BOLD__ << options->Y_EDGE_ELEMS
			<< __RESET__ << ", X length=" << __BOLD__ << options->X_EDGE_LENGTH << __RESET__ << ", Y length=" << __BOLD__ << options->Y_EDGE_LENGTH << __RESET__ << std::endl;


		if (Kokkos::hwloc::available()) {
			std::cout << "[" << __GREEN__ << "TOPOLOGY" << __RESET__ << "]  NUMA=" << __BOLD__ << Kokkos::hwloc::get_available_numa_count()
				<< __RESET__ << ", Cores/NUMA=" << __BOLD__ << Kokkos::hwloc::get_available_cores_per_numa()
				<< __RESET__ << ", Threads/Core=" << __BOLD__ << Kokkos::hwloc::get_available_threads_per_core() << __RESET__ << std::endl;
		} else {
			std::cout << "[" << __GREEN__ << "TOPOLOGY" << __RESET__ << "]  HWLOC unavailable cannot get topological informations" << std::endl;
		}

		// std::cout << "[" << __GREEN__ << "KOKKOS" << __RESET__ << "]    " << __BOLD__ << (is_same<MyLayout,Kokkos::LayoutLeft>::value?"Left":"Right")" << __RESET__ << " layout" << std::endl;

		if (!writer.isDisabled())
			std::cout << "[" << __GREEN__ << "OUTPUT" << __RESET__ << "]    VTK files stored in " << __BOLD__ << writer.outputDirectory() << __RESET__ << " directory" << std::endl;
		else
			std::cout << "[" << __GREEN__ << "OUTPUT" << __RESET__ << "]    " << __BOLD__ << "Disabled" << __RESET__ << std::endl;

		utils::Timer timer(true);

		iniF(); // @-2.0
		iniCenter(); // @-2.0
		computeV(); // @-2.0
		computeSurface(); // @-2.0
		iniUn(); // @-1.0
		timer.stop();

		iteration = 0;
		while (t < options->option_stoptime && iteration < options->option_max_iterations)
		{
			timer.start();
			utils::Timer compute_timer(true);
			iteration++;
			if (iteration!=1)
				std::cout << "[" << __CYAN__ << __BOLD__ << setw(3) << iteration << __RESET__ "] t = " << __BOLD__
					<< setiosflags(std::ios::scientific) << setprecision(8) << setw(16) << t << __RESET__;

			computeOutgoingFlux(); // @1.0
			computeTn(); // @1.0
			dumpVariables(); // @1.0
			copy_t_nplus1_to_t(); // @2.0
			computeUn(); // @2.0
			copy_u_nplus1_to_u(); // @3.0
			compute_timer.stop();

			// Progress
			std::cout << utils::progress_bar(iteration, options->option_max_iterations, t, options->option_stoptime, 30);
			timer.stop();
			std::cout << __BOLD__ << __CYAN__ << utils::Timer::print(
				utils::eta(iteration, options->option_max_iterations, t, options->option_stoptime, deltat, timer), true)
				<< __RESET__ << "\r";
			std::cout.flush();
		}
		std::cout << __YELLOW__ << "\n\tDone ! Took " << __MAGENTA__ << __BOLD__ << timer.print() << __RESET__ << std::endl;
	}
};	

int main(int argc, char* argv[]) 
{
	Kokkos::initialize(argc, argv);
	auto o = new HeatEquation::Options();
	string output;
	if (argc == 5) {
		o->X_EDGE_ELEMS = std::atoi(argv[1]);
		o->Y_EDGE_ELEMS = std::atoi(argv[2]);
		o->X_EDGE_LENGTH = std::atof(argv[3]);
		o->Y_EDGE_LENGTH = std::atof(argv[4]);
	} else if (argc == 6) {
		o->X_EDGE_ELEMS = std::atoi(argv[1]);
		o->Y_EDGE_ELEMS = std::atoi(argv[2]);
		o->X_EDGE_LENGTH = std::atof(argv[3]);
		o->Y_EDGE_LENGTH = std::atof(argv[4]);
		output = argv[5];
	} else if (argc != 1) {
		std::cerr << "[ERROR] Wrong number of arguments. Expecting 4 or 5 args: X Y Xlength Ylength (output)." << std::endl;
		std::cerr << "(X=100, Y=10, Xlength=0.01, Ylength=0.01 output=current directory with no args)" << std::endl;
	}
	auto gm = CartesianMesh2DGenerator::generate(o->X_EDGE_ELEMS, o->Y_EDGE_ELEMS, o->X_EDGE_LENGTH, o->Y_EDGE_LENGTH);
	auto nm = new NumericMesh2D(gm);
	auto c = new HeatEquation(o, nm, output);
	c->simulate();
	delete c;
	delete nm;
	delete gm;
	delete o;
	Kokkos::finalize();
	return 0;
}
