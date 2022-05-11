/* DO NOT EDIT THIS FILE - it is machine generated */

#ifndef __ITERATIVEHEATEQUATIONMODULE_H_
#define __ITERATIVEHEATEQUATIONMODULE_H_

#include <arcane/utils/NumArray.h>
#include <arcane/datatype/RealArrayVariant.h>
#include <arcane/datatype/RealArray2Variant.h>
#include <arcane/accelerator/core/IAcceleratorMng.h>
#include <arcane/accelerator/Reduce.h>
#include <arcane/accelerator/Accelerator.h>
#include <arcane/accelerator/RunCommandEnumerate.h>
#include "IterativeHeatEquation_axl.h"
#include "CartesianMesh2D.h"

namespace ax = Arcane::Accelerator;
using namespace Arcane;

/*** Free functions **********************************************************/

namespace iterativeheatequationfreefuncs
{
	const bool check(const bool a);
	const Real norm(RealArrayVariant a);
	const Real dot(RealArrayVariant a, RealArrayVariant b);
	const Real det(RealArrayVariant a, RealArrayVariant b);
	RealArrayVariant sumR1(RealArrayVariant a, RealArrayVariant b);
	const Real minR0(const Real a, const Real b);
	const Real sumR0(const Real a, const Real b);
	const Real prodR0(const Real a, const Real b);
	const Real maxR0(const Real a, const Real b);
	RealArrayVariant operatorAdd(RealArrayVariant a, RealArrayVariant b);
	RealArrayVariant operatorMult(const Real a, RealArrayVariant b);
	RealArrayVariant operatorSub(RealArrayVariant a, RealArrayVariant b);
}

/*** Module/Service **********************************************************/

class IterativeHeatEquationModule
: public ArcaneIterativeHeatEquationObject
{
public:
	IterativeHeatEquationModule(const ModuleBuildInfo& mbi);
	~IterativeHeatEquationModule() {}

	// entry points
	virtual void init() override;
	virtual void executeTimeLoopN() override;

	VersionInfo versionInfo() const override { return VersionInfo(1, 0, 0); }

public:
	// jobs
	virtual void computeFaceLength();
	virtual void computeTn();
	virtual void computeV();
	virtual void initD();
	virtual void initTime();
	virtual void initXc();
	virtual void setUpTimeLoopK();
	virtual void updateU();
	virtual void computeDeltaTn();
	virtual void computeFaceConductivity();
	virtual void computeResidual();
	virtual void executeTimeLoopK();
	virtual void initU();
	virtual void setUpTimeLoopN();
	virtual void computeAlphaCoeff();
	virtual void tearDownTimeLoopK();

private:
	// mesh attributes
	CartesianMesh2D* m_mesh;

	// other attributes
	Int32 m_lastDump;
	Int32 m_n;
	Int32 m_k;
	static constexpr Real m_u0 = 1.0;
	static constexpr Real2 m_vectOne = {1.0, 1.0};
	static constexpr Int32 m_maxIterationsK = 1000;
	static constexpr Real m_epsilon = 1.0E-8;
	Real m_deltat;
	Real m_t_n;
	Real m_t_nplus1;
	Real m_t_n0;
	Real m_residual;

	// accelerator queue
	ax::RunQueue* m_default_queue = nullptr;
};

#endif
