/* DO NOT EDIT THIS FILE - it is machine generated */

#ifndef __R1_H_
#define __R1_H_

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
#include "Hydro.h"

using namespace nablalib::utils;
using namespace nablalib::types;
using namespace nablalib::utils::kokkos;

/******************** Module declaration ********************/

class R1
{
	friend class Hydro;
	friend class R2;

	typedef Kokkos::TeamPolicy<Kokkos::DefaultExecutionSpace::scratch_memory_space>::member_type member_type;
	
public:
	R1(CartesianMesh2D& aMesh);
	~R1();

	void jsonInit(const char* jsonContent);

	inline void setMainModule(Hydro* value)
	{
		mainModule = value,
		mainModule->r1 = this;
	}

	void simulate();
	void rj1(const member_type& teamMember) noexcept;
	void rj2(const member_type& teamMember) noexcept;

private:
	/**
	 * Utility function to get work load for each team of threads
	 * In  : thread and number of element to use for computation
	 * Out : pair of indexes, 1st one for start of chunk, 2nd one for size of chunk
	 */
	const std::pair<size_t, size_t> computeTeamWorkRange(const member_type& thread, const size_t& nb_elmt) noexcept;
	
	// Mesh and mesh variables
	CartesianMesh2D& mesh;
	size_t nbNodes, nbCells;

	// Main module
	Hydro* mainModule;

	// Options and global variables
	Kokkos::View<double*> rv3;

	// Timers
	Timer globalTimer;
	Timer cpuTimer;
	Timer ioTimer;
};

#endif
