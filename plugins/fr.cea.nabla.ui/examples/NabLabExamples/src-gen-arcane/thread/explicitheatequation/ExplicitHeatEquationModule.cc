/* DO NOT EDIT THIS FILE - it is machine generated */

#include "ExplicitHeatEquationModule.h"
#include <arcane/Concurrency.h>
#include <arcane/ITimeLoopMng.h>

using namespace Arcane;

/*** Free functions **********************************************************/

namespace explicitheatequationfreefuncs
{
	Real norm(RealArrayVariant a)
	{
		return std::sqrt(explicitheatequationfreefuncs::dot(a, a));
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
	
	Real det(RealArrayVariant a, RealArrayVariant b)
	{
		return (a[0] * b[1] - a[1] * b[0]);
	}
	
	RealArrayVariant sumR1(RealArrayVariant a, RealArrayVariant b)
	{
		return explicitheatequationfreefuncs::operatorAdd(a, b);
	}
	
	Real minR0(Real a, Real b)
	{
		return std::min(a, b);
	}
	
	Real sumR0(Real a, Real b)
	{
		return a + b;
	}
	
	Real prodR0(Real a, Real b)
	{
		return a * b;
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

ExplicitHeatEquationModule::ExplicitHeatEquationModule(const ModuleBuildInfo& bi)
: ArcaneExplicitHeatEquationObject(bi)
{}

void ExplicitHeatEquationModule::init()
{
	// initialization of mesh attributes
	m_mesh = CartesianMesh2D::createInstance(mesh());

	// initialization of other attributes
	m_lastDump = numeric_limits<int>::min();
	m_n = 0;
	m_delta_t = 0.001;
	m_alpha.resize(nbCell());

	// calling jobs
	computeFaceLength(); // @1.0
	computeV(); // @1.0
	initD(); // @1.0
	initTime(); // @1.0
	initXc(); // @1.0
	computeDeltaTn(); // @2.0
	computeFaceConductivity(); // @2.0
	initU(); // @2.0
	setUpTimeLoopN(); // @2.0
	computeAlphaCoeff(); // @3.0
}

/**
 * Job computeFaceLength called @1.0 in simulate method.
 * In variables: X
 * Out variables: faceLength
 */
void ExplicitHeatEquationModule::computeFaceLength()
{
	arcaneParallelForeach(allFaces(), [&](FaceVectorView view)
	{
		ENUMERATE_FACE(fFaces, view)
		{
			const auto fId(fFaces.asItemLocalId());
			Real reduction0(0.0);
			{
				const auto nodesOfFaceF(m_mesh->getNodesOfFace(fId));
				const Int32 nbNodesOfFaceF(nodesOfFaceF.size());
				for (Int32 pNodesOfFaceF=0; pNodesOfFaceF<nbNodesOfFaceF; pNodesOfFaceF++)
				{
					const auto pId(nodesOfFaceF[pNodesOfFaceF]);
					const auto pPlus1Id(nodesOfFaceF[(pNodesOfFaceF+1+nbNodesOfFaceF)%nbNodesOfFaceF]);
					const auto pNodes(pId);
					const auto pPlus1Nodes(pPlus1Id);
					reduction0 = explicitheatequationfreefuncs::sumR0(reduction0, explicitheatequationfreefuncs::norm(explicitheatequationfreefuncs::operatorSub(m_X[pNodes], m_X[pPlus1Nodes])));
				}
			}
			m_faceLength[fFaces] = 0.5 * reduction0;
		}
	});
}

/**
 * Job computeTn called @1.0 in executeTimeLoopN method.
 * In variables: delta_t, t_n
 * Out variables: t_nplus1
 */
void ExplicitHeatEquationModule::computeTn()
{
	m_t_nplus1 = m_t_n + m_delta_t;
}

/**
 * Job computeV called @1.0 in simulate method.
 * In variables: X
 * Out variables: V
 */
void ExplicitHeatEquationModule::computeV()
{
	arcaneParallelForeach(allCells(), [&](CellVectorView view)
	{
		ENUMERATE_CELL(cCells, view)
		{
			const auto cId(cCells.asItemLocalId());
			Real reduction0(0.0);
			{
				const auto nodesOfCellC(m_mesh->getNodesOfCell(cId));
				const Int32 nbNodesOfCellC(nodesOfCellC.size());
				for (Int32 pNodesOfCellC=0; pNodesOfCellC<nbNodesOfCellC; pNodesOfCellC++)
				{
					const auto pId(nodesOfCellC[pNodesOfCellC]);
					const auto pPlus1Id(nodesOfCellC[(pNodesOfCellC+1+nbNodesOfCellC)%nbNodesOfCellC]);
					const auto pNodes(pId);
					const auto pPlus1Nodes(pPlus1Id);
					reduction0 = explicitheatequationfreefuncs::sumR0(reduction0, explicitheatequationfreefuncs::det(m_X[pNodes], m_X[pPlus1Nodes]));
				}
			}
			m_V[cCells] = 0.5 * reduction0;
		}
	});
}

/**
 * Job initD called @1.0 in simulate method.
 * In variables: 
 * Out variables: D
 */
void ExplicitHeatEquationModule::initD()
{
	arcaneParallelForeach(allCells(), [&](CellVectorView view)
	{
		ENUMERATE_CELL(cCells, view)
		{
			m_D[cCells] = 1.0;
		}
	});
}

/**
 * Job initTime called @1.0 in simulate method.
 * In variables: 
 * Out variables: t_n0
 */
void ExplicitHeatEquationModule::initTime()
{
	m_t_n0 = 0.0;
}

/**
 * Job initXc called @1.0 in simulate method.
 * In variables: X
 * Out variables: Xc
 */
void ExplicitHeatEquationModule::initXc()
{
	arcaneParallelForeach(allCells(), [&](CellVectorView view)
	{
		ENUMERATE_CELL(cCells, view)
		{
			const auto cId(cCells.asItemLocalId());
			Real2 reduction0{0.0, 0.0};
			{
				const auto nodesOfCellC(m_mesh->getNodesOfCell(cId));
				const Int32 nbNodesOfCellC(nodesOfCellC.size());
				for (Int32 pNodesOfCellC=0; pNodesOfCellC<nbNodesOfCellC; pNodesOfCellC++)
				{
					const auto pId(nodesOfCellC[pNodesOfCellC]);
					const auto pNodes(pId);
					reduction0 = explicitheatequationfreefuncs::sumR1(reduction0, m_X[pNodes]);
				}
			}
			m_Xc[cCells] = explicitheatequationfreefuncs::operatorMult(0.25, reduction0);
		}
	});
}

/**
 * Job updateU called @1.0 in executeTimeLoopN method.
 * In variables: alpha, u_n
 * Out variables: u_nplus1
 */
void ExplicitHeatEquationModule::updateU()
{
	arcaneParallelForeach(allCells(), [&](CellVectorView view)
	{
		ENUMERATE_CELL(cCells, view)
		{
			const auto cId(cCells.asItemLocalId());
			Real reduction0(0.0);
			{
				const auto neighbourCellsC(m_mesh->getNeighbourCells(cId));
				const Int32 nbNeighbourCellsC(neighbourCellsC.size());
				for (Int32 dNeighbourCellsC=0; dNeighbourCellsC<nbNeighbourCellsC; dNeighbourCellsC++)
				{
					const auto dId(neighbourCellsC[dNeighbourCellsC]);
					const auto dCells(dId);
					reduction0 = explicitheatequationfreefuncs::sumR0(reduction0, m_alpha[cCells][dCells] * m_u_n[dCells]);
				}
			}
			m_u_nplus1[cCells] = m_alpha[cCells][cCells.localId()] * m_u_n[cCells] + reduction0;
		}
	});
}

/**
 * Job computeDeltaTn called @2.0 in simulate method.
 * In variables: D, V
 * Out variables: delta_t
 */
void ExplicitHeatEquationModule::computeDeltaTn()
{
	Real reduction0(numeric_limits<double>::max());
	ENUMERATE_CELL(cCells, allCells())
	{
		reduction0 = explicitheatequationfreefuncs::minR0(reduction0, m_V[cCells] / m_D[cCells]);
	}
	m_delta_t = reduction0 * 0.24;
	m_global_deltat = m_delta_t;
}

/**
 * Job computeFaceConductivity called @2.0 in simulate method.
 * In variables: D
 * Out variables: faceConductivity
 */
void ExplicitHeatEquationModule::computeFaceConductivity()
{
	arcaneParallelForeach(allFaces(), [&](FaceVectorView view)
	{
		ENUMERATE_FACE(fFaces, view)
		{
			const auto fId(fFaces.asItemLocalId());
			Real reduction0(1.0);
			{
				const auto cellsOfFaceF(m_mesh->getCellsOfFace(fId));
				const Int32 nbCellsOfFaceF(cellsOfFaceF.size());
				for (Int32 c1CellsOfFaceF=0; c1CellsOfFaceF<nbCellsOfFaceF; c1CellsOfFaceF++)
				{
					const auto c1Id(cellsOfFaceF[c1CellsOfFaceF]);
					const auto c1Cells(c1Id);
					reduction0 = explicitheatequationfreefuncs::prodR0(reduction0, m_D[c1Cells]);
				}
			}
			Real reduction1(0.0);
			{
				const auto cellsOfFaceF(m_mesh->getCellsOfFace(fId));
				const Int32 nbCellsOfFaceF(cellsOfFaceF.size());
				for (Int32 c2CellsOfFaceF=0; c2CellsOfFaceF<nbCellsOfFaceF; c2CellsOfFaceF++)
				{
					const auto c2Id(cellsOfFaceF[c2CellsOfFaceF]);
					const auto c2Cells(c2Id);
					reduction1 = explicitheatequationfreefuncs::sumR0(reduction1, m_D[c2Cells]);
				}
			}
			m_faceConductivity[fFaces] = 2.0 * reduction0 / reduction1;
		}
	});
}

/**
 * Job initU called @2.0 in simulate method.
 * In variables: Xc, u0, vectOne
 * Out variables: u_n
 */
void ExplicitHeatEquationModule::initU()
{
	arcaneParallelForeach(allCells(), [&](CellVectorView view)
	{
		ENUMERATE_CELL(cCells, view)
		{
			if (explicitheatequationfreefuncs::norm(explicitheatequationfreefuncs::operatorSub(m_Xc[cCells], m_vectOne)) < 0.5) 
				m_u_n[cCells] = m_u0;
			else
				m_u_n[cCells] = 0.0;
		}
	});
}

/**
 * Job setUpTimeLoopN called @2.0 in simulate method.
 * In variables: t_n0
 * Out variables: t_n
 */
void ExplicitHeatEquationModule::setUpTimeLoopN()
{
	m_t_n = m_t_n0;
}

/**
 * Job computeAlphaCoeff called @3.0 in simulate method.
 * In variables: V, Xc, delta_t, faceConductivity, faceLength
 * Out variables: alpha
 */
void ExplicitHeatEquationModule::computeAlphaCoeff()
{
	arcaneParallelForeach(allCells(), [&](CellVectorView view)
	{
		ENUMERATE_CELL(cCells, view)
		{
			const auto cId(cCells.asItemLocalId());
			Real alpha_Diag(0.0);
			{
				const auto neighbourCellsC(m_mesh->getNeighbourCells(cId));
				const Int32 nbNeighbourCellsC(neighbourCellsC.size());
				for (Int32 dNeighbourCellsC=0; dNeighbourCellsC<nbNeighbourCellsC; dNeighbourCellsC++)
				{
					const auto dId(neighbourCellsC[dNeighbourCellsC]);
					const auto dCells(dId);
					const auto fId(m_mesh->getCommonFace(cId, dId));
					const auto fFaces(fId);
					const Real alpha_ExtraDiag(m_delta_t / m_V[cCells] * (m_faceLength[fFaces] * m_faceConductivity[fFaces]) / explicitheatequationfreefuncs::norm(explicitheatequationfreefuncs::operatorSub(m_Xc[cCells], m_Xc[dCells])));
					m_alpha[cCells][dCells] = alpha_ExtraDiag;
					alpha_Diag = alpha_Diag + alpha_ExtraDiag;
				}
			}
			m_alpha[cCells][cCells.localId()] = 1 - alpha_Diag;
		}
	});
}

/**
 * Job executeTimeLoopN called @4.0 in simulate method.
 * In variables: lastDump, maxIterations, n, outputPeriod, stopTime, t_n, t_nplus1, u_n
 * Out variables: t_nplus1, u_nplus1
 */
void ExplicitHeatEquationModule::executeTimeLoopN()
{
	m_n++;
	computeTn(); // @1.0
	updateU(); // @1.0
	
	// Evaluate loop condition with variables at time n
	bool continueLoop = (m_t_nplus1 < options()->stopTime() && m_n + 1 < options()->maxIterations());
	
	m_t_n = m_t_nplus1;
	arcaneParallelForeach(allCells(), [&](CellVectorView view)
	{
		ENUMERATE_CELL(i1Cells, view)
		{
			m_u_n[i1Cells] = m_u_nplus1[i1Cells];
		}
	});
	
	if (!continueLoop)
		subDomain()->timeLoopMng()->stopComputeLoop(true);
}

ARCANE_REGISTER_MODULE_EXPLICITHEATEQUATION(ExplicitHeatEquationModule);
