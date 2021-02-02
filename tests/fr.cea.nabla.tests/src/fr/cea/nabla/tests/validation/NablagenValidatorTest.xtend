package fr.cea.nabla.tests.validation

import com.google.inject.Inject
import com.google.inject.Provider
import fr.cea.nabla.NablaStandaloneSetup
import fr.cea.nabla.NablagenStandaloneSetup
import fr.cea.nabla.nabla.NablaModule
import fr.cea.nabla.nablagen.NablagenPackage
import fr.cea.nabla.nablagen.NablagenRoot
import fr.cea.nabla.tests.NablagenInjectorProvider
import fr.cea.nabla.validation.NablagenValidator
import org.eclipse.emf.ecore.EClass
import org.eclipse.emf.ecore.resource.ResourceSet
import org.eclipse.xtext.testing.InjectWith
import org.eclipse.xtext.testing.XtextRunner
import org.eclipse.xtext.testing.util.ParseHelper
import org.eclipse.xtext.testing.validation.ValidationTestHelper
import org.junit.Assert
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(XtextRunner)
@InjectWith(NablagenInjectorProvider)
class NablagenValidatorTest
{
	@Inject Provider<ResourceSet> resourceSetProvider
	@Inject ValidationTestHelper vth

	val nablaHydroModel =
	'''
	module Hydro;

	itemtypes { node, cell }

	connectivity nodes: → {node};
	connectivity cells: → {cell};

	// Simulation options
	option ℝ maxTime = 0.1;
	option ℕ maxIter = 500;

	let ℝ t = 0.0;
	option ℝ δt = 1.0;
	ℝ[2] X{nodes};
	ℝ hv1{cells}, hv2{cells}, hv3{cells}, hv4{cells}, hv5{cells}, hv6{cells}, hv7{cells};

	iterate n while (n+1 < maxIter && t^{n+1} < maxTime);

	hj1: ∀c∈cells(), hv3{c} = hv2{c};
	hj2: ∀c∈cells(), hv5{c} = hv3{c};
	hj3: ∀c∈cells(), hv7{c} = hv4{c} + hv5{c} + hv6{c};		
	'''

	val nablaRemapModel =
	'''
	module Remap;

	itemtypes { node, cell }

	connectivity nodes: → {node};
	connectivity cells: → {cell};

	ℝ[2] X{nodes};
	ℝ rv1{cells}, rv2{cells}, rv3{cells};

	rj1: ∀c∈cells(), rv2{c} = rv1{c};
	rj2: ∀c∈cells(), rv3{c} = rv2{c};
	'''

	val ngenModel = 
	'''
	Application HydroRemap;

	MainModule Hydro h
	{
		meshClassName = "CartesianMesh2D";
		nodeCoord = X;
		time = t;
		timeStep = δt;
	}

	AdditionalModule Remap r1
	{
		r1.X = h.X;
		r1.rv1 = h.hv1;
		r1.rv2 = h.hv4;
	}

	AdditionalModule Remap r2
	{
		r2.X = h.X;
		r2.rv1 = h.hv3;
		r2.rv3 = h.hv6;
	}

	VtkOutput
	{
		periodReferenceVariable = h.t;
		outputVariables = h.hv1 as "HV1", r1.rv1 as "RV1";
	}
	'''

	val nablaSetup = new NablaStandaloneSetup
	val nablaInjector = nablaSetup.createInjectorAndDoEMFRegistration
	val ParseHelper<NablaModule> nablaParseHelper = nablaInjector.getInstance(ParseHelper)

	val nablagenSetup = new NablagenStandaloneSetup
	val nablagenInjector = nablagenSetup.createInjectorAndDoEMFRegistration
	val ParseHelper<NablagenRoot> nablagenParseHelper = nablagenInjector.getInstance(ParseHelper)

	@Test
	def void testCheckName()
	{
		val koNgenModel = ngenModel.replace("Application HydroRemap", "Application hydroRemap")
		assertNgen(koNgenModel, 
			NablagenPackage.eINSTANCE.nablagenRoot,
			NablagenValidator::NGEN_ELEMENT_NAME, 
			NablagenValidator::getNgenElementNameMsg(),
			ngenModel)
	}

	@Test
	def void testNgenModuleName()
	{
		val koNgenModel = ngenModel.replace("MainModule Hydro h", "MainModule Hydro H").replace("h.", "H.")
		assertNgen(koNgenModel,
			NablagenPackage.eINSTANCE.nablagenModule,
			NablagenValidator::NGEN_MODULE_NAME,
			NablagenValidator::getNgenModuleNameMsg(),
			ngenModel)
	}

	@Test
	def void testCppMandatoryVariables()
	{
		val koNgenModel = ngenModel.concat('OpenMP
			{
				outputPath = "/tmp";
				NABLA_CXX_COMPILER = "/usr/bin/g++";
			}')

		val okNgenModel = koNgenModel.replace("timeStep = δt;", "timeStep = δt;
			iterationMax = maxIter;
			timeMax = maxTime;")

		assertNgen(koNgenModel,
			NablagenPackage.eINSTANCE.nablagenRoot,
			NablagenValidator::CPP_MANDATORY_VARIABLES,
			NablagenValidator::getCppMandatoryVariablesMsg(),
			okNgenModel)
	}

	@Test
	def void testConnectivityConsistency()
	{
		val koHydroModel = nablaHydroModel.replace("→ {cell}", "→ {node}")
		val koNgen = readModelsAndGetNgen(koHydroModel, nablaRemapModel, ngenModel)
		vth.assertError(koNgen,
			NablagenPackage.eINSTANCE.nablagenModule,
			NablagenValidator::CONNECTIVITY_CONSISTENCY,
			NablagenValidator::getConnectivityConsistencyMsg("Remap::cells", "Hydro::cells"))
		val okNgen = readModelsAndGetNgen(nablaHydroModel, nablaRemapModel, ngenModel)
		vth.assertNoErrors(okNgen)
	}

	@Test
	def void testVarLinkMainVarType()
	{
		val koNgenModel = ngenModel.replace("r1.X = h.X;", "r1.X = h.t;")
		assertNgen(koNgenModel,
			NablagenPackage.eINSTANCE.varLink,
			NablagenValidator::VAR_LINK_MAIN_VAR_TYPE,
			NablagenValidator::getVarLinkMainVarTypeMsg("ℝ²{nodes}", "ℝ"),
			ngenModel)
		val okNgen = readModelsAndGetNgen(nablaHydroModel, nablaRemapModel, ngenModel)
		vth.assertNoErrors(okNgen)
	}

	private def void assertNgen(String koNgenModel, EClass objectType, String code, String msg, String okNgenModel)
	{
		val koNgen = readModelsAndGetNgen(nablaHydroModel, nablaRemapModel, koNgenModel)
		vth.assertError(koNgen, objectType, code, msg)
		val okNgen = readModelsAndGetNgen(nablaHydroModel, nablaRemapModel, okNgenModel)
		vth.assertNoErrors(okNgen)
	}

	private def NablagenRoot readModelsAndGetNgen(String nablaHydroModel, String nablaRemapModel, String ngenModel)
	{
		val rs = resourceSetProvider.get
		val nablaHydro = nablaParseHelper.parse(nablaHydroModel, rs)
		Assert.assertNotNull(nablaHydro)
		val nablaRemap = nablaParseHelper.parse(nablaRemapModel, rs)
		Assert.assertNotNull(nablaRemap)
		val ngen = nablagenParseHelper.parse(ngenModel, rs)
		Assert.assertNotNull(ngen)
		return ngen
	}
}