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

using namespace nablalib;

class ExplicitHeatEquation
{
public:
	struct Options
	{
		double X_LENGTH = 2.0;
		double Y_LENGTH = 2.0;
		double u0 = 1.0;
		Real2 vectOne = Real2(1.0);
		int X_EDGE_ELEMS = 40;
		int Y_EDGE_ELEMS = 40;
		int Z_EDGE_ELEMS = 1;
		double X_EDGE_LENGTH = as_const(X_LENGTH) / as_const(X_EDGE_ELEMS);
		double Y_EDGE_LENGTH = as_const(Y_LENGTH) / as_const(Y_EDGE_ELEMS);
		double option_stoptime = 1.0;
		int option_max_iterations = 500000000;
	};
	Options* options;

private:
	NumericMesh2D* mesh;
	VtkFileWriter2D writer;
	int nbNodes, nbCells, nbFaces, nbNodesOfCell, nbNodesOfFace, nbCellsOfFace, nbNeighbourCells;

	// Global Variables
	double t, deltat, t_nplus1;

	// Array Variables
	Kokkos::View<Real2*> X;
	Kokkos::View<Real2*> Xc;
	Kokkos::View<double*> xc;
	Kokkos::View<double*> yc;
	Kokkos::View<double*> u;
	Kokkos::View<double*> V;
	Kokkos::View<double*> D;
	Kokkos::View<double*> faceLength;
	Kokkos::View<double*> faceConductivity;
	Kokkos::View<double**> alpha;
	Kokkos::View<double*> u_nplus1;
	
	const size_t maxHardThread = Kokkos::DefaultExecutionSpace::max_hardware_threads();

public:
	ExplicitHeatEquation(Options* aOptions, NumericMesh2D* aNumericMesh2D, string output)
	: options(aOptions)
	, mesh(aNumericMesh2D)
	, writer("ExplicitHeatEquation", output)
	, nbNodes(mesh->getNbNodes())
	, nbCells(mesh->getNbCells())
	, nbFaces(mesh->getNbFaces())
	, nbNodesOfCell(NumericMesh2D::MaxNbNodesOfCell)
	, nbNodesOfFace(NumericMesh2D::MaxNbNodesOfFace)
	, nbCellsOfFace(NumericMesh2D::MaxNbCellsOfFace)
	, nbNeighbourCells(NumericMesh2D::MaxNbNeighbourCells)
	, t(0.0)
	, deltat(0.001)
	, t_nplus1(0.0)
	, X("X", nbNodes)
	, Xc("Xc", nbCells)
	, xc("xc", nbCells)
	, yc("yc", nbCells)
	, u("u", nbCells)
	, V("V", nbCells)
	, D("D", nbCells)
	, faceLength("faceLength", nbFaces)
	, faceConductivity("faceConductivity", nbFaces)
	, alpha("alpha", nbCells, nbCells)
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
	 * Job InitXc @-3.0
	 * In variables: X
	 * Out variables: Xc
	 */
	KOKKOS_INLINE_FUNCTION
	void initXc() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& cCells)
		{
			int cId(cCells);
			Real2 reduceSum9574393(0.0);
			{
				auto nodesOfCellC(mesh->getNodesOfCell(cId));
				for (int pNodesOfCellC=0; pNodesOfCellC<nodesOfCellC.size(); pNodesOfCellC++)
				{
					int pId(nodesOfCellC[pNodesOfCellC]);
					int pNodes(pId);
					reduceSum9574393 = reduceSum9574393 + (X(pNodes));
				}
			}
			Xc(cCells) = 0.25 * reduceSum9574393;
		});
	}
	
	/**
	 * Job InitD @-3.0
	 * In variables: 
	 * Out variables: D
	 */
	KOKKOS_INLINE_FUNCTION
	void initD() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& cCells)
		{
			D(cCells) = 1.0;
		});
	}
	
	/**
	 * Job ComputeV @-3.0
	 * In variables: X
	 * Out variables: V
	 */
	KOKKOS_INLINE_FUNCTION
	void computeV() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& jCells)
		{
			int jId(jCells);
			double reduceSum_236982947 = 0.0;
			{
				auto nodesOfCellJ(mesh->getNodesOfCell(jId));
				for (int pNodesOfCellJ=0; pNodesOfCellJ<nodesOfCellJ.size(); pNodesOfCellJ++)
				{
					int pId(nodesOfCellJ[pNodesOfCellJ]);
					int pPlus1Id(nodesOfCellJ[(pNodesOfCellJ+1+nbNodesOfCell)%nbNodesOfCell]);
					int pNodes(pId);
					int pPlus1Nodes(pPlus1Id);
					reduceSum_236982947 = reduceSum_236982947 + (MathFunctions::det(X(pNodes), X(pPlus1Nodes)));
				}
			}
			V(jCells) = 0.5 * reduceSum_236982947;
		});
	}
	
	/**
	 * Job ComputeFaceLength @-3.0
	 * In variables: X
	 * Out variables: faceLength
	 */
	KOKKOS_INLINE_FUNCTION
	void computeFaceLength() noexcept
	{
		Kokkos::parallel_for(nbFaces, KOKKOS_LAMBDA(const int& fFaces)
		{
			int fId(fFaces);
			double reduceSum505189429 = 0.0;
			{
				auto nodesOfFaceF(mesh->getNodesOfFace(fId));
				for (int pNodesOfFaceF=0; pNodesOfFaceF<nodesOfFaceF.size(); pNodesOfFaceF++)
				{
					int pId(nodesOfFaceF[pNodesOfFaceF]);
					int pPlus1Id(nodesOfFaceF[(pNodesOfFaceF+1+nbNodesOfFace)%nbNodesOfFace]);
					int pNodes(pId);
					int pPlus1Nodes(pPlus1Id);
					reduceSum505189429 = reduceSum505189429 + (MathFunctions::norm(X(pNodes) - X(pPlus1Nodes)));
				}
			}
			faceLength(fFaces) = 0.5 * reduceSum505189429;
		});
	}
	
	/**
	 * Job InitXcAndYc @-2.0
	 * In variables: Xc
	 * Out variables: xc, yc
	 */
	KOKKOS_INLINE_FUNCTION
	void initXcAndYc() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& cCells)
		{
			xc(cCells) = Xc(cCells).x;
			yc(cCells) = Xc(cCells).y;
		});
	}
	
	/**
	 * Job InitU @-2.0
	 * In variables: Xc, vectOne, u0
	 * Out variables: u
	 */
	KOKKOS_INLINE_FUNCTION
	void initU() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& cCells)
		{
			if (MathFunctions::norm(Xc(cCells) - as_const(options->vectOne)) < 0.5) 
				u(cCells) = as_const(options->u0);
			else 
				u(cCells) = 0.0;
		});
	}
	
	/**
	 * Job computeDeltaTn @-2.0
	 * In variables: X_EDGE_LENGTH, Y_EDGE_LENGTH, D
	 * Out variables: deltat
	 */
	KOKKOS_INLINE_FUNCTION
	void computeDeltaTn() noexcept
	{
		double reduceMin_1282422837(numeric_limits<double>::max());
		{
			Kokkos::Min<double> reducer(reduceMin_1282422837);
			Kokkos::parallel_reduce("ReductionreduceMin_1282422837", nbCells, KOKKOS_LAMBDA(const int& cCells, double& x)
			{
				reducer.join(x, as_const(options->X_EDGE_LENGTH) * as_const(options->Y_EDGE_LENGTH) / D(cCells));
			}, reducer);
		}
		deltat = reduceMin_1282422837 * 0.24;
	}
	
	/**
	 * Job ComputeFaceConductivity @-2.0
	 * In variables: D
	 * Out variables: faceConductivity
	 */
	KOKKOS_INLINE_FUNCTION
	void computeFaceConductivity() noexcept
	{
		Kokkos::parallel_for(nbFaces, KOKKOS_LAMBDA(const int& fFaces)
		{
			int fId(fFaces);
			double reduceProd_1688659495 = 1.0;
			{
				auto cellsOfFaceF(mesh->getCellsOfFace(fId));
				for (int c1CellsOfFaceF=0; c1CellsOfFaceF<cellsOfFaceF.size(); c1CellsOfFaceF++)
				{
					int c1Id(cellsOfFaceF[c1CellsOfFaceF]);
					int c1Cells(c1Id);
					reduceProd_1688659495 = reduceProd_1688659495 * (D(c1Cells));
				}
			}
			double reduceSum1410943609 = 0.0;
			{
				auto cellsOfFaceF(mesh->getCellsOfFace(fId));
				for (int c2CellsOfFaceF=0; c2CellsOfFaceF<cellsOfFaceF.size(); c2CellsOfFaceF++)
				{
					int c2Id(cellsOfFaceF[c2CellsOfFaceF]);
					int c2Cells(c2Id);
					reduceSum1410943609 = reduceSum1410943609 + (D(c2Cells));
				}
			}
			faceConductivity(fFaces) = 2.0 * reduceProd_1688659495 / reduceSum1410943609;
		});
	}
	
	/**
	 * Job computeAlphaCoeff @-1.0
	 * In variables: deltat, V, faceLength, faceConductivity, Xc
	 * Out variables: alpha
	 */
	KOKKOS_INLINE_FUNCTION
	void computeAlphaCoeff() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& cCells)
		{
			int cId(cCells);
			double alphaDiag = 0.0;
			{
				auto neighbourCellsC(mesh->getNeighbourCells(cId));
				for (int dNeighbourCellsC=0; dNeighbourCellsC<neighbourCellsC.size(); dNeighbourCellsC++)
				{
					int dId(neighbourCellsC[dNeighbourCellsC]);
					int dCells(dId);
					int fId(mesh->getCommonFace(cId, dId));
					int fFaces(fId);
					double alphaExtraDiag = as_const(deltat) / V(cCells) * (faceLength(fFaces) * faceConductivity(fFaces)) / MathFunctions::norm(Xc(cCells) - Xc(dCells));
					alpha(cCells,dCells) = alphaExtraDiag;
					alphaDiag = alphaDiag + alphaExtraDiag;
				}
			}
			alpha(cCells,cCells) = 1 - alphaDiag;
		});
	}
	
	/**
	 * Job UpdateU @1.0
	 * In variables: alpha, u
	 * Out variables: u_nplus1
	 */
	KOKKOS_INLINE_FUNCTION
	void updateU() noexcept
	{
		Kokkos::parallel_for(nbCells, KOKKOS_LAMBDA(const int& cCells)
		{
			int cId(cCells);
			double reduceSum_745417239 = 0.0;
			{
				auto neighbourCellsC(mesh->getNeighbourCells(cId));
				for (int dNeighbourCellsC=0; dNeighbourCellsC<neighbourCellsC.size(); dNeighbourCellsC++)
				{
					int dId(neighbourCellsC[dNeighbourCellsC]);
					int dCells(dId);
					reduceSum_745417239 = reduceSum_745417239 + (alpha(cCells,dCells) * u(dCells));
				}
			}
			u_nplus1(cCells) = alpha(cCells,cCells) * u(cCells) + reduceSum_745417239;
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
	 * Job Copy_u_nplus1_to_u @2.0
	 * In variables: u_nplus1
	 * Out variables: u
	 */
	KOKKOS_INLINE_FUNCTION
	void copy_u_nplus1_to_u() noexcept
	{
		std::swap(u_nplus1, u);
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

	void dumpVariables(const int iteration)
	{
		std::map<string, double*> cellVariables;
		std::map<string, double*> nodeVariables;
		cellVariables.insert(pair<string,double*>("Temperature", u.data()));
		auto quads = mesh->getGeometricMesh()->getQuads();
		writer.writeFile(iteration, nbNodes, X.data(), nbCells, quads.data(), cellVariables, nodeVariables);
	}


public:
	void simulate()
	{
		std::cout << "\n" << __BLUE_BKG__ << __YELLOW__ << __BOLD__ <<"\tStarting ExplicitHeatEquation ..." << __RESET__ << "\n\n";

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

		initXc(); // @-3.0
		initD(); // @-3.0
		computeV(); // @-3.0
		computeFaceLength(); // @-3.0
		initXcAndYc(); // @-2.0
		initU(); // @-2.0
		computeDeltaTn(); // @-2.0
		computeFaceConductivity(); // @-2.0
		computeAlphaCoeff(); // @-1.0
		timer.stop();
		int iteration = 0;
		while (t < options->option_stoptime && iteration < options->option_max_iterations)
		{
			timer.start();
			utils::Timer compute_timer(true);
			iteration++;
			if (iteration!=1)
				std::cout << "[" << __CYAN__ << __BOLD__ << setw(3) << iteration << __RESET__ "] t = " << __BOLD__
					<< setiosflags(std::ios::scientific) << setprecision(8) << setw(16) << t << __RESET__;

			updateU(); // @1.0
			computeTn(); // @1.0
			copy_u_nplus1_to_u(); // @2.0
			copy_t_nplus1_to_t(); // @2.0
			compute_timer.stop();

			if (!writer.isDisabled()) {
				utils::Timer io_timer(true);
				dumpVariables(iteration);
				io_timer.stop();
				std::cout << " {CPU: " << __BLUE__ << compute_timer.print(true) << __RESET__ ", IO: " << __BLUE__ << io_timer.print(true) << __RESET__ "} ";
			} else {
				std::cout << " {CPU: " << __BLUE__ << compute_timer.print(true) << __RESET__ ", IO: " << __RED__ << "none" << __RESET__ << "} ";
			}
			// Progress
			std::cout << utils::progress_bar(iteration, options->option_max_iterations, t, options->option_stoptime);
			timer.stop();
			std::cout << __BOLD__ << __CYAN__ << utils::Timer::print(
				utils::eta(iteration, options->option_max_iterations, t, options->option_stoptime, deltat, timer), true)
				<< __RESET__ << "\r";
			std::cout.flush();
		}
		dumpVariables(iteration);
		std::cout << __YELLOW__ << "\n\tDone ! Took " << __MAGENTA__ << __BOLD__ << timer.print() << __RESET__ << std::endl;
	}
};	

int main(int argc, char* argv[]) 
{
	Kokkos::initialize(argc, argv);
	auto o = new ExplicitHeatEquation::Options();
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
	auto c = new ExplicitHeatEquation(o, nm, output);
	c->simulate();
	delete c;
	delete nm;
	delete gm;
	delete o;
	Kokkos::finalize();
	return 0;
}