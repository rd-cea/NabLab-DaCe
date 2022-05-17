/* DO NOT EDIT THIS FILE - it is machine generated */

#ifndef __ITERATION_H_
#define __ITERATION_H_

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

using namespace nablalib::utils;
using namespace nablalib::types;
using namespace nablalib::utils::kokkos;


/******************** Free functions declarations ********************/

namespace iterationfreefuncs
{
bool assertEquals(double expected, double actual);
}

/******************** Module declaration ********************/

class Iteration
{
	typedef Kokkos::TeamPolicy<Kokkos::DefaultExecutionSpace::scratch_memory_space>::member_type member_type;
	

private:
	/**
	 * Utility function to get work load for each team of threads
	 * In  : thread and number of element to use for computation
	 * Out : pair of indexes, 1st one for start of chunk, 2nd one for size of chunk
	 */
	const std::pair<size_t, size_t> computeTeamWorkRange(const member_type& thread, const size_t& nb_elmt) noexcept;
	
	// Mesh and mesh variables
	CartesianMesh2D& mesh;
	size_t nbNodes;
	size_t nbCells;


	// Timers
	Timer globalTimer;
	Timer cpuTimer;
	Timer ioTimer;
	

public:
	Iteration(CartesianMesh2D& aMesh);
	~Iteration();

	void jsonInit(const char* jsonContent);

	void simulate();
	void computeTn() noexcept;
	void iniTime() noexcept;
	void iniVk(const member_type& teamMember) noexcept;
	void iniVn(const member_type& teamMember) noexcept;
	void setUpTimeLoopK(const member_type& teamMember) noexcept;
	void updateVk(const member_type& teamMember) noexcept;
	void updateVl(const member_type& teamMember) noexcept;
	void executeTimeLoopK() noexcept;
	void setUpTimeLoopN(const member_type& teamMember) noexcept;
	void executeTimeLoopN() noexcept;
	void tearDownTimeLoopK(const member_type& teamMember) noexcept;
	void iniVl(const member_type& teamMember) noexcept;
	void oracleVk(const member_type& teamMember) noexcept;
	void setUpTimeLoopL(const member_type& teamMember) noexcept;
	void executeTimeLoopL() noexcept;
	void tearDownTimeLoopL(const member_type& teamMember) noexcept;
	void oracleVl(const member_type& teamMember) noexcept;
	void updateVn(const member_type& teamMember) noexcept;
	void oracleVn(const member_type& teamMember) noexcept;

	// Options and global variables.
	// Module variables are public members of the class to be accessible from Python.
	int n;
	int k;
	int l;
	static constexpr double maxTime = 0.1;
	static constexpr double deltat = 1.0;
	double t_n;
	double t_nplus1;
	double t_n0;
	Kokkos::View<RealArray1D<2>*> X;
	static constexpr int maxIterN = 10;
	static constexpr int maxIterK = 6;
	static constexpr int maxIterL = 7;
	Kokkos::View<double*> vn_n;
	Kokkos::View<double*> vn_nplus1;
	Kokkos::View<double*> vn_n0;
	Kokkos::View<double*> vk_n;
	Kokkos::View<double*> vk_nplus1;
	Kokkos::View<double*> vk_nplus1_k;
	Kokkos::View<double*> vk_nplus1_kplus1;
	Kokkos::View<double*> vk_nplus1_k0;
	Kokkos::View<double*> vl_n;
	Kokkos::View<double*> vl_nplus1;
	Kokkos::View<double*> vl_nplus1_l;
	Kokkos::View<double*> vl_nplus1_lplus1;
	Kokkos::View<double*> vl_nplus1_l0;
};

#endif
