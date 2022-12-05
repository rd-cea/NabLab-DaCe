/* DO NOT EDIT THIS FILE - it is machine generated */

#include "HeatEquationModule.h"
#include <arcane/Concurrency.h>
#include <arcane/ITimeLoopMng.h>

using namespace Arcane;

/*** Free functions **********************************************************/

namespace heatequationfreefuncs
{
	Real det(RealArrayVariant a, RealArrayVariant b)
	{
		return (a[0] * b[1] - a[1] * b[0]);
	}
	
	Real norm(RealArrayVariant a)
	{
		return std::sqrt(heatequationfreefuncs::dot(a, a));
	}
	
	Real dot(RealArrayVariant a, RealArrayVariant b)
	{
		Real result(0.0);
		for (Int32 i=0; i<a.size(); i++)
		{
			result = result + a(i) * b(i);
		}
		return result;
	}
	
	RealArrayVariant sumR1(RealArrayVariant a, RealArrayVariant b)
	{
		return heatequationfreefuncs::operatorAdd(a, b);
	}
	
	Real sumR0(Real a, Real b)
	{
		return a + b;
	}
	
	RealArrayVariant operatorAdd(RealArrayVariant a, RealArrayVariant b)
	{
		NumArray<Real,MDDim1> result(a.size());
		for (Int32 ix0=0; ix0<a.size(); ix0++)
		{
			result(ix0) = a(ix0) + b(ix0);
		}
		return result;
	}
	
	RealArrayVariant operatorMult(Real a, RealArrayVariant b)
	{
		NumArray<Real,MDDim1> result(b.size());
		for (Int32 ix0=0; ix0<b.size(); ix0++)
		{
			result(ix0) = a * b(ix0);
		}
		return result;
	}
	
	RealArrayVariant operatorSub(RealArrayVariant a, RealArrayVariant b)
	{
		NumArray<Real,MDDim1> result(a.size());
		for (Int32 ix0=0; ix0<a.size(); ix0++)
		{
			result(ix0) = a(ix0) - b(ix0);
		}
		return result;
	}
}

/*** Module/Service **********************************************************/

HeatEquationModule::HeatEquationModule(const ModuleBuildInfo& bi)
: ArcaneHeatEquationObject(bi)
, m_default_queue(subDomain()->acceleratorMng()->defaultQueue())
{}

void HeatEquationModule::init()
{
	// initialization of mesh attributes
	m_mesh = CartesianMesh2D::createInstance(mesh());

	// initialization of other attributes
	m_lastDump = numeric_limits<int>::min();
	m_n = 0;

	// constant time step
	m_global_deltat = m_delta_t;

	// calling jobs
	computeSurface(); // @1.0
	computeV(); // @1.0
	iniCenter(); // @1.0
	iniF(); // @1.0
	iniTime(); // @1.0
	iniUn(); // @2.0
	setUpTimeLoopN(); // @2.0
}

/**
 * Job computeOutgoingFlux called @1.0 in executeTimeLoopN method.
 * In variables: V, center, delta_t, surface, u_n
 * Out variables: outgoingFlux
 */
void HeatEquationModule::computeOutgoingFlux()
{
	auto command = makeCommand(m_default_queue);
	auto in_u_n = ax::viewIn(command, m_u_n);
	auto in_center = ax::viewIn(command, m_center);
	auto in_surface = ax::viewIn(command, m_surface);
	auto in_delta_t = m_delta_t;
	auto in_V = ax::viewIn(command, m_V);
	auto out_outgoingFlux = ax::viewOut(command, m_outgoingFlux);
	command << RUNCOMMAND_ENUMERATE(Cell, j1Cells, allCells())
	{
		const auto j1Id(j1Cells);
		Real reduction0(0.0);
		{
			const auto neighbourCellsJ1(m_mesh->getNeighbourCells(j1Id));
			const Int32 nbNeighbourCellsJ1(neighbourCellsJ1.size());
			for (Int32 j2NeighbourCellsJ1=0; j2NeighbourCellsJ1<nbNeighbourCellsJ1; j2NeighbourCellsJ1++)
			{
				const auto j2Id(neighbourCellsJ1[j2NeighbourCellsJ1]);
				const auto j2Cells(j2Id);
				const auto cfId(m_mesh->getCommonFace(j1Id, j2Id));
				const auto cfFaces(cfId);
				Real reduction1((in_u_n[j2Cells] - in_u_n[j1Cells]) / heatequationfreefuncs::norm(heatequationfreefuncs::operatorSub(in_center[j2Cells], in_center[j1Cells])) * in_surface[cfFaces]);
				reduction0 = heatequationfreefuncs::sumR0(reduction0, reduction1);
			}
		}
		out_outgoingFlux[j1Cells] = in_delta_t / in_V[j1Cells] * reduction0;
	};
}

/**
 * Job computeSurface called @1.0 in simulate method.
 * In variables: X
 * Out variables: surface
 */
void HeatEquationModule::computeSurface()
{
	auto command = makeCommand(m_default_queue);
	auto in_X = ax::viewIn(command, m_X);
	auto out_surface = ax::viewOut(command, m_surface);
	command << RUNCOMMAND_ENUMERATE(Face, fFaces, allFaces())
	{
		const auto fId(fFaces);
		Real reduction0(0.0);
		{
			const auto nodesOfFaceF(m_mesh->getNodesOfFace(fId));
			const Int32 nbNodesOfFaceF(nodesOfFaceF.size());
			for (Int32 rNodesOfFaceF=0; rNodesOfFaceF<nbNodesOfFaceF; rNodesOfFaceF++)
			{
				const auto rId(nodesOfFaceF[rNodesOfFaceF]);
				const auto rPlus1Id(nodesOfFaceF[(rNodesOfFaceF+1+nbNodesOfFaceF)%nbNodesOfFaceF]);
				const auto rNodes(rId);
				const auto rPlus1Nodes(rPlus1Id);
				reduction0 = heatequationfreefuncs::sumR0(reduction0, heatequationfreefuncs::norm(heatequationfreefuncs::operatorSub(in_X[rNodes], in_X[rPlus1Nodes])));
			}
		}
		out_surface[fFaces] = 0.5 * reduction0;
	};
}

/**
 * Job computeTn called @1.0 in executeTimeLoopN method.
 * In variables: delta_t, t_n
 * Out variables: t_nplus1
 */
void HeatEquationModule::computeTn()
{
	m_t_nplus1 = m_t_n + m_delta_t;
}

/**
 * Job computeV called @1.0 in simulate method.
 * In variables: X
 * Out variables: V
 */
void HeatEquationModule::computeV()
{
	auto command = makeCommand(m_default_queue);
	auto in_X = ax::viewIn(command, m_X);
	auto out_V = ax::viewOut(command, m_V);
	command << RUNCOMMAND_ENUMERATE(Cell, jCells, allCells())
	{
		const auto jId(jCells);
		Real reduction0(0.0);
		{
			const auto nodesOfCellJ(m_mesh->getNodesOfCell(jId));
			const Int32 nbNodesOfCellJ(nodesOfCellJ.size());
			for (Int32 rNodesOfCellJ=0; rNodesOfCellJ<nbNodesOfCellJ; rNodesOfCellJ++)
			{
				const auto rId(nodesOfCellJ[rNodesOfCellJ]);
				const auto rPlus1Id(nodesOfCellJ[(rNodesOfCellJ+1+nbNodesOfCellJ)%nbNodesOfCellJ]);
				const auto rNodes(rId);
				const auto rPlus1Nodes(rPlus1Id);
				reduction0 = heatequationfreefuncs::sumR0(reduction0, heatequationfreefuncs::det(in_X[rNodes], in_X[rPlus1Nodes]));
			}
		}
		out_V[jCells] = 0.5 * reduction0;
	};
}

/**
 * Job iniCenter called @1.0 in simulate method.
 * In variables: X
 * Out variables: center
 */
void HeatEquationModule::iniCenter()
{
	auto command = makeCommand(m_default_queue);
	auto in_X = ax::viewIn(command, m_X);
	auto out_center = ax::viewOut(command, m_center);
	command << RUNCOMMAND_ENUMERATE(Cell, jCells, allCells())
	{
		const auto jId(jCells);
		Real2 reduction0{0.0, 0.0};
		{
			const auto nodesOfCellJ(m_mesh->getNodesOfCell(jId));
			const Int32 nbNodesOfCellJ(nodesOfCellJ.size());
			for (Int32 rNodesOfCellJ=0; rNodesOfCellJ<nbNodesOfCellJ; rNodesOfCellJ++)
			{
				const auto rId(nodesOfCellJ[rNodesOfCellJ]);
				const auto rNodes(rId);
				reduction0 = heatequationfreefuncs::sumR1(reduction0, in_X[rNodes]);
			}
		}
		out_center[jCells] = heatequationfreefuncs::operatorMult(0.25, reduction0);
	};
}

/**
 * Job iniF called @1.0 in simulate method.
 * In variables: 
 * Out variables: f
 */
void HeatEquationModule::iniF()
{
	auto command = makeCommand(m_default_queue);
	auto out_f = ax::viewOut(command, m_f);
	command << RUNCOMMAND_ENUMERATE(Cell, jCells, allCells())
	{
		out_f[jCells] = 0.0;
	};
}

/**
 * Job iniTime called @1.0 in simulate method.
 * In variables: 
 * Out variables: t_n0
 */
void HeatEquationModule::iniTime()
{
	m_t_n0 = 0.0;
}

/**
 * Job computeUn called @2.0 in executeTimeLoopN method.
 * In variables: delta_t, f, outgoingFlux, u_n
 * Out variables: u_nplus1
 */
void HeatEquationModule::computeUn()
{
	auto command = makeCommand(m_default_queue);
	auto in_f = ax::viewIn(command, m_f);
	auto in_delta_t = m_delta_t;
	auto in_u_n = ax::viewIn(command, m_u_n);
	auto in_outgoingFlux = ax::viewIn(command, m_outgoingFlux);
	auto out_u_nplus1 = ax::viewOut(command, m_u_nplus1);
	command << RUNCOMMAND_ENUMERATE(Cell, jCells, allCells())
	{
		out_u_nplus1[jCells] = in_f[jCells] * in_delta_t + in_u_n[jCells] + in_outgoingFlux[jCells];
	};
}

/**
 * Job iniUn called @2.0 in simulate method.
 * In variables: PI, alpha, center
 * Out variables: u_n
 */
void HeatEquationModule::iniUn()
{
	auto command = makeCommand(m_default_queue);
	auto in_PI = m_PI;
	auto in_alpha = m_alpha;
	auto in_center = ax::viewIn(command, m_center);
	auto out_u_n = ax::viewOut(command, m_u_n);
	command << RUNCOMMAND_ENUMERATE(Cell, jCells, allCells())
	{
		out_u_n[jCells] = std::cos(2 * in_PI * in_alpha * in_center[jCells][0]);
	};
}

/**
 * Job setUpTimeLoopN called @2.0 in simulate method.
 * In variables: t_n0
 * Out variables: t_n
 */
void HeatEquationModule::setUpTimeLoopN()
{
	m_t_n = m_t_n0;
}

/**
 * Job executeTimeLoopN called @3.0 in simulate method.
 * In variables: lastDump, maxIterations, n, outputPeriod, stopTime, t_n, t_nplus1, u_n
 * Out variables: t_nplus1, u_nplus1
 */
void HeatEquationModule::executeTimeLoopN()
{
	m_n++;
	computeOutgoingFlux(); // @1.0
	computeTn(); // @1.0
	computeUn(); // @2.0
	
	// Evaluate loop condition with variables at time n
	bool continueLoop = (m_t_nplus1 < options()->stopTime() && m_n + 1 < options()->maxIterations());
	
	m_t_n = m_t_nplus1;
	{
		auto command = makeCommand(m_default_queue);
		auto in_u_nplus1 = ax::viewIn(command, m_u_nplus1);
		auto out_u_n = ax::viewOut(command, m_u_n);
		command << RUNCOMMAND_ENUMERATE(Cell, i1Cells, allCells())
		{
			out_u_n[i1Cells] = in_u_nplus1[i1Cells];
		};
	}
	
	if (!continueLoop)
		subDomain()->timeLoopMng()->stopComputeLoop(true);
}

ARCANE_REGISTER_MODULE_HEATEQUATION(HeatEquationModule);
