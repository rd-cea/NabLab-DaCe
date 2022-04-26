/* DO NOT EDIT THIS FILE - it is machine generated */

#ifndef __GLACE2D_H_
#define __GLACE2D_H_

#include <fstream>
#include <iomanip>
#include <type_traits>
#include <limits>
#include <utility>
#include <cmath>
#include <rapidjson/document.h>
#include <Kokkos_Core.hpp>
#include <Kokkos_hwloc.hpp>
#include "nablalib/utils/Utils.h"
#include "nablalib/utils/Timer.h"
#include "nablalib/types/Types.h"
#include "nablalib/utils/kokkos/Parallel.h"
#include "CartesianMesh2D.h"
#include "PvdFileWriter2D.h"

using namespace nablalib::utils;
using namespace nablalib::types;
using namespace nablalib::utils::kokkos;


/******************** Free functions declarations ********************/

namespace glace2dfreefuncs
{
double det(RealArray2D<2,2> a);
RealArray1D<2> perp(RealArray1D<2> a);
template<size_t x>
double dot(RealArray1D<x> a, RealArray1D<x> b);
template<size_t x>
double norm(RealArray1D<x> a);
template<size_t l>
RealArray2D<l,l> tensProduct(RealArray1D<l> a, RealArray1D<l> b);
template<size_t x, size_t y>
RealArray1D<x> matVectProduct(RealArray2D<x,y> a, RealArray1D<y> b);
template<size_t l>
double trace(RealArray2D<l,l> a);
RealArray2D<2,2> inverse(RealArray2D<2,2> a);
template<size_t x>
RealArray1D<x> sumR1(RealArray1D<x> a, RealArray1D<x> b);
double sumR0(double a, double b);
template<size_t x>
RealArray2D<x,x> sumR2(RealArray2D<x,x> a, RealArray2D<x,x> b);
double minR0(double a, double b);
template<size_t x0>
RealArray1D<x0> operatorAdd(RealArray1D<x0> a, RealArray1D<x0> b);
template<size_t x0, size_t x1>
RealArray2D<x0,x1> operatorAdd(RealArray2D<x0,x1> a, RealArray2D<x0,x1> b);
template<size_t x0>
RealArray1D<x0> operatorMult(double a, RealArray1D<x0> b);
template<size_t x0>
RealArray1D<x0> operatorSub(RealArray1D<x0> a, RealArray1D<x0> b);
template<size_t x0, size_t x1>
RealArray2D<x0,x1> operatorMult(double a, RealArray2D<x0,x1> b);
template<size_t x0, size_t x1>
RealArray2D<x0,x1> operatorSub(RealArray2D<x0,x1> a, RealArray2D<x0,x1> b);
template<size_t x0, size_t x1>
RealArray2D<x0,x1> operatorMult(RealArray2D<x0,x1> a, RealArray2D<x0,x1> b);
template<size_t x0, size_t x1>
RealArray2D<x0,x1> operatorMult(RealArray2D<x0,x1> a, double b);
}

/******************** Module declaration ********************/

class Glace2d
{

private:
	void dumpVariables(int iteration, bool useTimer=true);

	// Mesh and mesh variables
	CartesianMesh2D& mesh;
	size_t nbNodes;
	size_t nbCells;
	size_t nbTopNodes;
	size_t nbBottomNodes;
	size_t nbLeftNodes;
	size_t nbRightNodes;
	size_t nbInnerNodes;

	PvdFileWriter2D* writer;
	std::string outputPath;

	// Timers
	Timer globalTimer;
	Timer cpuTimer;
	Timer ioTimer;
	

public:
	Glace2d(CartesianMesh2D& aMesh);
	~Glace2d();

	void jsonInit(const char* jsonContent);

	void simulate();
	void computeCjr() noexcept;
	void computeInternalEnergy() noexcept;
	void iniCjrIc() noexcept;
	void iniTime() noexcept;
	void computeLjr() noexcept;
	void computeV() noexcept;
	void initialize() noexcept;
	void setUpTimeLoopN() noexcept;
	void computeDensity() noexcept;
	void executeTimeLoopN() noexcept;
	void computeEOSp() noexcept;
	void computeEOSc() noexcept;
	void computeAjr() noexcept;
	void computedeltatj() noexcept;
	void computeAr() noexcept;
	void computeBr() noexcept;
	void computeDt() noexcept;
	void computeBoundaryConditions() noexcept;
	void computeBt() noexcept;
	void computeMt() noexcept;
	void computeTn() noexcept;
	void computeU() noexcept;
	void computeFjr() noexcept;
	void computeXn() noexcept;
	void computeEn() noexcept;
	void computeUn() noexcept;

	// Options and global variables.
	// Module variables are public members of the class to be accessible from Python.
	int outputPeriod;
	int lastDump;
	int n;
	double stopTime;
	int maxIterations;
	static constexpr double gamma = 1.4;
	static constexpr double xInterface = 0.5;
	static constexpr double deltatCfl = 0.4;
	static constexpr double rhoIniZg = 1.0;
	static constexpr double rhoIniZd = 0.125;
	static constexpr double pIniZg = 1.0;
	static constexpr double pIniZd = 0.1;
	double t_n;
	double t_nplus1;
	double t_n0;
	double deltat;
	Kokkos::View<RealArray1D<2>*> X_n;
	Kokkos::View<RealArray1D<2>*> X_nplus1;
	Kokkos::View<RealArray1D<2>*> X_n0;
	Kokkos::View<RealArray1D<2>*> b;
	Kokkos::View<RealArray1D<2>*> bt;
	Kokkos::View<RealArray2D<2,2>*> Ar;
	Kokkos::View<RealArray2D<2,2>*> Mt;
	Kokkos::View<RealArray1D<2>*> ur;
	Kokkos::View<double*> c;
	Kokkos::View<double*> m;
	Kokkos::View<double*> p;
	Kokkos::View<double*> rho;
	Kokkos::View<double*> e;
	Kokkos::View<double*> E_n;
	Kokkos::View<double*> E_nplus1;
	Kokkos::View<double*> V;
	Kokkos::View<double*> deltatj;
	Kokkos::View<RealArray1D<2>*> uj_n;
	Kokkos::View<RealArray1D<2>*> uj_nplus1;
	Kokkos::View<double**> l;
	Kokkos::View<RealArray1D<2>**> Cjr_ic;
	Kokkos::View<RealArray1D<2>**> C;
	Kokkos::View<RealArray1D<2>**> F;
	Kokkos::View<RealArray2D<2,2>**> Ajr;
};

#endif
