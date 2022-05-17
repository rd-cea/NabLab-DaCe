/* DO NOT EDIT THIS FILE - it is machine generated */

#ifndef __VARIABLES_H_
#define __VARIABLES_H_

#include <fstream>
#include <iomanip>
#include <type_traits>
#include <limits>
#include <utility>
#include <cmath>
#include <rapidjson/document.h>
#include <omp.h>
#include "nablalib/utils/Utils.h"
#include "nablalib/utils/Timer.h"
#include "nablalib/types/Types.h"
#include "CartesianMesh2D.h"

using namespace nablalib::utils;
using namespace nablalib::types;


/******************** Free functions declarations ********************/

namespace variablesfreefuncs
{
bool assertEquals(int expected, int actual);
template<size_t x>
bool assertEquals(RealArray1D<x> expected, RealArray1D<x> actual);
bool assertEquals(double expected, double actual);
template<size_t x0>
RealArray1D<x0> operatorAdd(RealArray1D<x0> a, RealArray1D<x0> b);
}

/******************** Module declaration ********************/

class Variables
{

private:
	// Mesh and mesh variables
	CartesianMesh2D& mesh;
	size_t nbNodes;


	// Timers
	Timer globalTimer;
	Timer cpuTimer;
	Timer ioTimer;
	

public:
	Variables(CartesianMesh2D& aMesh);
	~Variables();

	void jsonInit(const char* jsonContent);

	void simulate();
	void dynamicVecInitialization() noexcept;
	void varVecInitialization() noexcept;
	void oracle() noexcept;

	// Options and global variables.
	// Module variables are public members of the class to be accessible from Python.
	static constexpr double maxTime = 0.1;
	static constexpr int maxIter = 500;
	static constexpr double deltat = 1.0;
	static constexpr double t = 0.0;
	std::vector<RealArray1D<2>> X;
	int optDim;
	RealArray1D<2> optVect1;
	RealArray1D<2> optVect2;
	RealArray1D<2> optVect3;
	static constexpr int constexprDim = 2;
	static constexpr RealArray1D<constexprDim> constexprVec = {1.1, 1.1};
	RealArray1D<constexprDim> varVec;
	int checkDynamicDim;
	RealArray1D<0> dynamicVec;
};

#endif
