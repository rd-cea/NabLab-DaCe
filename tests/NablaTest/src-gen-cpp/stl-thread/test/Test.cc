#include "test/Test.h"

using namespace nablalib;


/******************** Options definition ********************/

Test::Options::Options(const std::string& fileName)
{
	ifstream ifs(fileName);
	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document d;
	d.ParseStream(isw);
	assert(d.IsObject());
	// maxTime
	assert(d.HasMember("maxTime"));
	const rapidjson::Value& valueof_maxTime = d["maxTime"];
	assert(valueof_maxTime.IsDouble());
	maxTime = valueof_maxTime.GetDouble();
	// maxIter
	assert(d.HasMember("maxIter"));
	const rapidjson::Value& valueof_maxIter = d["maxIter"];
	assert(valueof_maxIter.IsInt());
	maxIter = valueof_maxIter.GetInt();
	// X_EDGE_LENGTH
	assert(d.HasMember("X_EDGE_LENGTH"));
	const rapidjson::Value& valueof_X_EDGE_LENGTH = d["X_EDGE_LENGTH"];
	assert(valueof_X_EDGE_LENGTH.IsDouble());
	X_EDGE_LENGTH = valueof_X_EDGE_LENGTH.GetDouble();
	// Y_EDGE_LENGTH
	assert(d.HasMember("Y_EDGE_LENGTH"));
	const rapidjson::Value& valueof_Y_EDGE_LENGTH = d["Y_EDGE_LENGTH"];
	assert(valueof_Y_EDGE_LENGTH.IsDouble());
	Y_EDGE_LENGTH = valueof_Y_EDGE_LENGTH.GetDouble();
	// X_EDGE_ELEMS
	assert(d.HasMember("X_EDGE_ELEMS"));
	const rapidjson::Value& valueof_X_EDGE_ELEMS = d["X_EDGE_ELEMS"];
	assert(valueof_X_EDGE_ELEMS.IsInt());
	X_EDGE_ELEMS = valueof_X_EDGE_ELEMS.GetInt();
	// Y_EDGE_ELEMS
	assert(d.HasMember("Y_EDGE_ELEMS"));
	const rapidjson::Value& valueof_Y_EDGE_ELEMS = d["Y_EDGE_ELEMS"];
	assert(valueof_Y_EDGE_ELEMS.IsInt());
	Y_EDGE_ELEMS = valueof_Y_EDGE_ELEMS.GetInt();
	// deltat
	assert(d.HasMember("deltat"));
	const rapidjson::Value& valueof_deltat = d["deltat"];
	assert(valueof_deltat.IsDouble());
	deltat = valueof_deltat.GetDouble();
}

/******************** Module definition ********************/

Test::Test(const Options& aOptions)
: options(aOptions)
, t_n(0.0)
, t_nplus1(0.0)
, mesh(CartesianMesh2DGenerator::generate(options.X_EDGE_ELEMS, options.Y_EDGE_ELEMS, options.X_EDGE_LENGTH, options.Y_EDGE_LENGTH))
, nbNodes(mesh->getNbNodes())
, nbCells(mesh->getNbCells())
, X(nbNodes)
, e1(nbCells)
, e2_n(nbCells)
, e2_nplus1(nbCells)
, e2_nplus1_k(nbCells)
, e2_nplus1_kplus1(nbCells)
, e2_nplus1_k0(nbCells)
, e_n(nbCells)
, e_nplus1(nbCells)
, e_n0(nbCells)
{
	// Copy node coordinates
	const auto& gNodes = mesh->getGeometry()->getNodes();
	for (size_t rNodes=0; rNodes<nbNodes; rNodes++)
	{
		X[rNodes][0] = gNodes[rNodes][0];
		X[rNodes][1] = gNodes[rNodes][1];
	}
}

Test::~Test()
{
	delete mesh;
}

/**
 * Job computeE1 called @1.0 in executeTimeLoopN method.
 * In variables: e_n
 * Out variables: e1
 */
void Test::computeE1() noexcept
{
	parallel::parallel_exec(nbCells, [&](const size_t& cCells)
	{
		e1[cCells] = e_n[cCells] + 3.0;
	});
}

/**
 * Job computeE2 called @1.0 in executeTimeLoopK method.
 * In variables: e2_nplus1_k
 * Out variables: e2_nplus1_kplus1
 */
void Test::computeE2() noexcept
{
	parallel::parallel_exec(nbCells, [&](const size_t& cCells)
	{
		e2_nplus1_kplus1[cCells] = e2_nplus1_k[cCells] + 0.4;
	});
}

/**
 * Job initE called @1.0 in simulate method.
 * In variables: 
 * Out variables: e_n0
 */
void Test::initE() noexcept
{
	parallel::parallel_exec(nbCells, [&](const size_t& cCells)
	{
		e_n0[cCells] = 0.0;
	});
}

/**
 * Job updateT called @1.0 in executeTimeLoopN method.
 * In variables: deltat, t_n
 * Out variables: t_nplus1
 */
void Test::updateT() noexcept
{
	t_nplus1 = t_n + options.deltat;
}

/**
 * Job InitE2 called @2.0 in executeTimeLoopN method.
 * In variables: e1
 * Out variables: e2_nplus1_k0
 */
void Test::initE2() noexcept
{
	parallel::parallel_exec(nbCells, [&](const size_t& cCells)
	{
		e2_nplus1_k0[cCells] = e1[cCells];
	});
}

/**
 * Job SetUpTimeLoopN called @2.0 in simulate method.
 * In variables: e_n0
 * Out variables: e_n
 */
void Test::setUpTimeLoopN() noexcept
{
	for (size_t i1(0) ; i1<e_n.size() ; i1++)
		e_n[i1] = e_n0[i1];
}

/**
 * Job ExecuteTimeLoopN called @3.0 in simulate method.
 * In variables: deltat, e1, e2_nplus1, e2_nplus1_k, e2_nplus1_k0, e2_nplus1_kplus1, e_n, t_n
 * Out variables: e1, e2_nplus1, e2_nplus1_k, e2_nplus1_k0, e2_nplus1_kplus1, e_nplus1, t_nplus1
 */
void Test::executeTimeLoopN() noexcept
{
	n = 0;
	bool continueLoop = true;
	do
	{
		globalTimer.start();
		cpuTimer.start();
		n++;
		if (n!=1)
			std::cout << "[" << __CYAN__ << __BOLD__ << setw(3) << n << __RESET__ "] t = " << __BOLD__
				<< setiosflags(std::ios::scientific) << setprecision(8) << setw(16) << t_n << __RESET__;
	
		computeE1(); // @1.0
		updateT(); // @1.0
		initE2(); // @2.0
		setUpTimeLoopK(); // @3.0
		executeTimeLoopK(); // @4.0
		tearDownTimeLoopK(); // @5.0
		updateE(); // @6.0
		
	
		// Evaluate loop condition with variables at time n
		continueLoop = (n + 1 < options.maxIter && t_nplus1 < options.maxTime);
	
		if (continueLoop)
		{
			// Switch variables to prepare next iteration
			std::swap(t_nplus1, t_n);
			std::swap(e2_nplus1, e2_n);
			std::swap(e_nplus1, e_n);
		}
	
		cpuTimer.stop();
		globalTimer.stop();
	
		// Timers display
			std::cout << " {CPU: " << __BLUE__ << cpuTimer.print(true) << __RESET__ ", IO: " << __RED__ << "none" << __RESET__ << "} ";
		
		// Progress
		std::cout << utils::progress_bar(n, options.maxIter, t_n, options.maxTime, 25);
		std::cout << __BOLD__ << __CYAN__ << utils::Timer::print(
			utils::eta(n, options.maxIter, t_n, options.maxTime, options.deltat, globalTimer), true)
			<< __RESET__ << "\r";
		std::cout.flush();
	
		cpuTimer.reset();
		ioTimer.reset();
	} while (continueLoop);
}

/**
 * Job SetUpTimeLoopK called @3.0 in executeTimeLoopN method.
 * In variables: e2_nplus1_k0
 * Out variables: e2_nplus1_k
 */
void Test::setUpTimeLoopK() noexcept
{
	for (size_t i1(0) ; i1<e2_nplus1_k.size() ; i1++)
		e2_nplus1_k[i1] = e2_nplus1_k0[i1];
}

/**
 * Job ExecuteTimeLoopK called @4.0 in executeTimeLoopN method.
 * In variables: e2_nplus1_k
 * Out variables: e2_nplus1_kplus1
 */
void Test::executeTimeLoopK() noexcept
{
	k = 0;
	bool continueLoop = true;
	do
	{
		k++;
	
		computeE2(); // @1.0
		
	
		// Evaluate loop condition with variables at time n
		continueLoop = (k + 1 < 10);
	
		if (continueLoop)
		{
			// Switch variables to prepare next iteration
			std::swap(e2_nplus1_kplus1, e2_nplus1_k);
		}
	
	
	} while (continueLoop);
}

/**
 * Job TearDownTimeLoopK called @5.0 in executeTimeLoopN method.
 * In variables: e2_nplus1_kplus1
 * Out variables: e2_nplus1
 */
void Test::tearDownTimeLoopK() noexcept
{
	for (size_t i1(0) ; i1<e2_nplus1.size() ; i1++)
		e2_nplus1[i1] = e2_nplus1_kplus1[i1];
}

/**
 * Job updateE called @6.0 in executeTimeLoopN method.
 * In variables: e2_nplus1
 * Out variables: e_nplus1
 */
void Test::updateE() noexcept
{
	parallel::parallel_exec(nbCells, [&](const size_t& cCells)
	{
		e_nplus1[cCells] = e2_nplus1[cCells] - 3;
	});
}

void Test::simulate()
{
	std::cout << "\n" << __BLUE_BKG__ << __YELLOW__ << __BOLD__ <<"\tStarting Test ..." << __RESET__ << "\n\n";
	
	std::cout << "[" << __GREEN__ << "MESH" << __RESET__ << "]      X=" << __BOLD__ << options.X_EDGE_ELEMS << __RESET__ << ", Y=" << __BOLD__ << options.Y_EDGE_ELEMS
		<< __RESET__ << ", X length=" << __BOLD__ << options.X_EDGE_LENGTH << __RESET__ << ", Y length=" << __BOLD__ << options.Y_EDGE_LENGTH << __RESET__ << std::endl;
	
	std::cout << "[" << __GREEN__ << "TOPOLOGY" << __RESET__ << "]  HWLOC unavailable cannot get topological informations" << std::endl;
	
		std::cout << "[" << __GREEN__ << "OUTPUT" << __RESET__ << "]    " << __BOLD__ << "Disabled" << __RESET__ << std::endl;

	initE(); // @1.0
	setUpTimeLoopN(); // @2.0
	executeTimeLoopN(); // @3.0
	
	std::cout << __YELLOW__ << "\n\tDone ! Took " << __MAGENTA__ << __BOLD__ << globalTimer.print() << __RESET__ << std::endl;
}

/******************** Module definition ********************/

int main(int argc, char* argv[]) 
{
	string dataFile;
	
	if (argc == 2)
	{
		dataFile = argv[1];
	}
	else
	{
		std::cerr << "[ERROR] Wrong number of arguments. Expecting 1 arg: dataFile." << std::endl;
		std::cerr << "(TestDefaultOptions.json)" << std::endl;
		return -1;
	}
	
	Test::Options options(dataFile);
	// simulator must be a pointer if there is a finalize at the end (Kokkos, omp...)
	auto simulator = new Test(options);
	simulator->simulate();
	
	// simulator must be deleted before calling finalize
	delete simulator;
	return 0;
}
