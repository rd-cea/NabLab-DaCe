#include <fstream>
#include <iomanip>
#include <type_traits>
#include <limits>
#include <utility>
#include <cmath>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <Kokkos_Core.hpp>
#include <Kokkos_hwloc.hpp>
#include "mesh/CartesianMesh2DGenerator.h"
#include "mesh/CartesianMesh2D.h"
#include "mesh/PvdFileWriter2D.h"
#include "utils/Utils.h"
#include "utils/Timer.h"
#include "types/Types.h"
#include "types/MathFunctions.h"
#include "utils/kokkos/Parallel.h"

using namespace nablalib;

/******************** Free functions declarations ********************/

KOKKOS_INLINE_FUNCTION
RealArray1D<2> perp(RealArray1D<2> a);
template<size_t l>
KOKKOS_INLINE_FUNCTION
RealArray2D<l,l> tensProduct(RealArray1D<l> a, RealArray1D<l> b);
template<size_t l>
KOKKOS_INLINE_FUNCTION
double trace(RealArray2D<l,l> a);
KOKKOS_INLINE_FUNCTION
RealArray2D<2,2> inverse(RealArray2D<2,2> a);
template<size_t x>
KOKKOS_INLINE_FUNCTION
RealArray1D<x> sumR1(RealArray1D<x> a, RealArray1D<x> b);
KOKKOS_INLINE_FUNCTION
double sumR0(double a, double b);
template<size_t x>
KOKKOS_INLINE_FUNCTION
RealArray2D<x,x> sumR2(RealArray2D<x,x> a, RealArray2D<x,x> b);
KOKKOS_INLINE_FUNCTION
double minR0(double a, double b);


/******************** Module declaration ********************/

class Glace2d
{
public:
	struct Options
	{
		double X_EDGE_LENGTH;
		double Y_EDGE_LENGTH;
		size_t X_EDGE_ELEMS;
		size_t Y_EDGE_ELEMS;
		double option_stoptime;
		size_t option_max_iterations;
		double gamma;
		double option_x_interface;
		double option_deltat_ini;
		double option_deltat_cfl;
		double option_rho_ini_zg;
		double option_rho_ini_zd;
		double option_p_ini_zg;
		double option_p_ini_zd;

		Options(const std::string& fileName);
	};

	Options* options;

	Glace2d(Options* aOptions, CartesianMesh2D* aCartesianMesh2D, string output);

private:
	CartesianMesh2D* mesh;
	PvdFileWriter2D writer;
	size_t nbNodes, nbCells, nbNodesOfCell, nbCellsOfNode, nbInnerNodes, nbOuterFaces, nbNodesOfFace;
	int n;
	double t_n;
	double t_nplus1;
	double deltat_n;
	double deltat_nplus1;
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
	int lastDump;
	utils::Timer globalTimer;
	utils::Timer cpuTimer;
	utils::Timer ioTimer;
	typedef Kokkos::TeamPolicy<Kokkos::DefaultExecutionSpace::scratch_memory_space>::member_type member_type;

	KOKKOS_INLINE_FUNCTION
	void computeCjr(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeInternalEnergy(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void iniCjrIc(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void setUpTimeLoopN() noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeLjr(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeV(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void initialize(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeDensity(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void executeTimeLoopN() noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeEOSp(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeEOSc(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeAjr(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computedeltatj(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeAr(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeBr(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeDt(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeBoundaryConditions(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeBt(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeMt(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeTn() noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeU(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeFjr(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeXn(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeEn(const member_type& teamMember) noexcept;
	
	KOKKOS_INLINE_FUNCTION
	void computeUn(const member_type& teamMember) noexcept;

	void dumpVariables(int iteration);

public:
	void simulate();
};
