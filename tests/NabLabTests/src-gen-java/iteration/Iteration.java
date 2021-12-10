/* DO NOT EDIT THIS FILE - it is machine generated */

package iteration;

import java.io.FileReader;
import java.io.IOException;
import java.util.stream.IntStream;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

import fr.cea.nabla.javalib.mesh.*;

public final class Iteration
{
	// Mesh and mesh variables
	private final CartesianMesh2D mesh;
	@SuppressWarnings("unused")
	private final int nbNodes, nbCells;

	// Options and global variables
	int n;
	int k;
	int l;
	static final double maxTime = 0.1;
	static final double deltat = 1.0;
	double t_n;
	double t_nplus1;
	double t_n0;
	double[][] X;
	static final int maxIterN = 10;
	static final int maxIterK = 6;
	static final int maxIterL = 7;
	double[] vn_n;
	double[] vn_nplus1;
	double[] vn_n0;
	double[] vk_n;
	double[] vk_nplus1;
	double[] vk_nplus1_k;
	double[] vk_nplus1_kplus1;
	double[] vk_nplus1_k0;
	double[] vl_n;
	double[] vl_nplus1;
	double[] vl_nplus1_l;
	double[] vl_nplus1_lplus1;
	double[] vl_nplus1_l0;

	public Iteration(CartesianMesh2D aMesh)
	{
		// Mesh and mesh variables initialization
		mesh = aMesh;
		nbNodes = mesh.getNbNodes();
		nbCells = mesh.getNbCells();
	}

	public void jsonInit(final String jsonContent)
	{
		final Gson gson = new Gson();
		final JsonObject options = gson.fromJson(jsonContent, JsonObject.class);
		X = new double[nbNodes][2];
		vn_n = new double[nbCells];
		vn_nplus1 = new double[nbCells];
		vn_n0 = new double[nbCells];
		vk_n = new double[nbCells];
		vk_nplus1 = new double[nbCells];
		vk_nplus1_k = new double[nbCells];
		vk_nplus1_kplus1 = new double[nbCells];
		vk_nplus1_k0 = new double[nbCells];
		vl_n = new double[nbCells];
		vl_nplus1 = new double[nbCells];
		vl_nplus1_l = new double[nbCells];
		vl_nplus1_lplus1 = new double[nbCells];
		vl_nplus1_l0 = new double[nbCells];

		// Copy node coordinates
		double[][] gNodes = mesh.getGeometry().getNodes();
		IntStream.range(0, nbNodes).parallel().forEach(rNodes ->
		{
			X[rNodes][0] = gNodes[rNodes][0];
			X[rNodes][1] = gNodes[rNodes][1];
		});
	}

	/**
	 * Job computeTn called @1.0 in executeTimeLoopN method.
	 * In variables: deltat, t_n
	 * Out variables: t_nplus1
	 */
	protected void computeTn()
	{
		t_nplus1 = t_n + deltat;
	}

	/**
	 * Job iniTime called @1.0 in simulate method.
	 * In variables: 
	 * Out variables: t_n0
	 */
	protected void iniTime()
	{
		t_n0 = 0.0;
	}

	/**
	 * Job iniVk called @1.0 in simulate method.
	 * In variables: 
	 * Out variables: vk_nplus1_k0
	 */
	protected void iniVk()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			vk_nplus1_k0[jCells] = 0.0;
		});
	}

	/**
	 * Job iniVn called @1.0 in simulate method.
	 * In variables: 
	 * Out variables: vn_n0
	 */
	protected void iniVn()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			vn_n0[jCells] = 0.0;
		});
	}

	/**
	 * Job setUpTimeLoopK called @1.0 in executeTimeLoopN method.
	 * In variables: vk_nplus1_k0
	 * Out variables: vk_nplus1_k
	 */
	protected void setUpTimeLoopK()
	{
		IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
		{
			vk_nplus1_k[i1Cells] = vk_nplus1_k0[i1Cells];
		});
	}

	/**
	 * Job updateVk called @1.0 in executeTimeLoopK method.
	 * In variables: vk_nplus1_k
	 * Out variables: vk_nplus1_kplus1
	 */
	protected void updateVk()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			vk_nplus1_kplus1[jCells] = vk_nplus1_k[jCells] + 2;
		});
	}

	/**
	 * Job updateVl called @1.0 in executeTimeLoopL method.
	 * In variables: vl_nplus1_l
	 * Out variables: vl_nplus1_lplus1
	 */
	protected void updateVl()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			vl_nplus1_lplus1[jCells] = vl_nplus1_l[jCells] + 1;
		});
	}

	/**
	 * Job executeTimeLoopK called @2.0 in executeTimeLoopN method.
	 * In variables: k, maxIterK, vk_nplus1_k
	 * Out variables: vk_nplus1_kplus1
	 */
	protected void executeTimeLoopK()
	{
		k = 0;
		boolean continueLoop = true;
		do
		{
			k++;
			System.out.printf("Start iteration k: %5d\n", k);
		
			updateVk(); // @1.0
		
			// Evaluate loop condition with variables at time n
			continueLoop = (k < maxIterK);
		
			IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
			{
				vk_nplus1_k[i1Cells] = vk_nplus1_kplus1[i1Cells];
			});
		} while (continueLoop);
	}

	/**
	 * Job setUpTimeLoopN called @2.0 in simulate method.
	 * In variables: t_n0, vn_n0
	 * Out variables: t_n, vn_n
	 */
	protected void setUpTimeLoopN()
	{
		t_n = t_n0;
		IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
		{
			vn_n[i1Cells] = vn_n0[i1Cells];
		});
	}

	/**
	 * Job executeTimeLoopN called @3.0 in simulate method.
	 * In variables: maxIterN, n, t_n, vk_n, vl_n, vn_n
	 * Out variables: t_nplus1, vk_nplus1, vl_nplus1, vn_nplus1
	 */
	protected void executeTimeLoopN()
	{
		n = 0;
		boolean continueLoop = true;
		do
		{
			n++;
			System.out.printf("START ITERATION n: %5d - t: %5.5f - deltat: %5.5f\n", n, t_n, deltat);
		
			computeTn(); // @1.0
			setUpTimeLoopK(); // @1.0
			executeTimeLoopK(); // @2.0
			tearDownTimeLoopK(); // @3.0
			iniVl(); // @4.0
			oracleVk(); // @4.0
			setUpTimeLoopL(); // @5.0
			executeTimeLoopL(); // @6.0
			tearDownTimeLoopL(); // @7.0
			oracleVl(); // @8.0
			updateVn(); // @8.0
			oracleVn(); // @9.0
		
			// Evaluate loop condition with variables at time n
			continueLoop = (n < maxIterN);
		
			t_n = t_nplus1;
			IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
			{
				vn_n[i1Cells] = vn_nplus1[i1Cells];
			});
			IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
			{
				vk_n[i1Cells] = vk_nplus1[i1Cells];
			});
			IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
			{
				vl_n[i1Cells] = vl_nplus1[i1Cells];
			});
		} while (continueLoop);
		
		System.out.printf("FINAL TIME: %5.5f - deltat: %5.5f\n", t_n, deltat);
	}

	/**
	 * Job tearDownTimeLoopK called @3.0 in executeTimeLoopN method.
	 * In variables: vk_nplus1_kplus1
	 * Out variables: vk_nplus1
	 */
	protected void tearDownTimeLoopK()
	{
		IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
		{
			vk_nplus1[i1Cells] = vk_nplus1_kplus1[i1Cells];
		});
	}

	/**
	 * Job iniVl called @4.0 in executeTimeLoopN method.
	 * In variables: vk_nplus1
	 * Out variables: vl_nplus1_l0
	 */
	protected void iniVl()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			vl_nplus1_l0[jCells] = vk_nplus1[jCells] + 8;
		});
	}

	/**
	 * Job oracleVk called @4.0 in executeTimeLoopN method.
	 * In variables: vk_nplus1
	 * Out variables: 
	 */
	protected void oracleVk()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			final boolean testVk = assertEquals(12.0, vk_nplus1[jCells]);
		});
	}

	/**
	 * Job setUpTimeLoopL called @5.0 in executeTimeLoopN method.
	 * In variables: vl_nplus1_l0
	 * Out variables: vl_nplus1_l
	 */
	protected void setUpTimeLoopL()
	{
		IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
		{
			vl_nplus1_l[i1Cells] = vl_nplus1_l0[i1Cells];
		});
	}

	/**
	 * Job executeTimeLoopL called @6.0 in executeTimeLoopN method.
	 * In variables: l, maxIterL, vl_nplus1_l
	 * Out variables: vl_nplus1_lplus1
	 */
	protected void executeTimeLoopL()
	{
		l = 0;
		boolean continueLoop = true;
		do
		{
			l++;
			System.out.printf("Start iteration l: %5d\n", l);
		
			updateVl(); // @1.0
		
			// Evaluate loop condition with variables at time n
			continueLoop = (l < maxIterL);
		
			IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
			{
				vl_nplus1_l[i1Cells] = vl_nplus1_lplus1[i1Cells];
			});
		} while (continueLoop);
	}

	/**
	 * Job tearDownTimeLoopL called @7.0 in executeTimeLoopN method.
	 * In variables: vl_nplus1_lplus1
	 * Out variables: vl_nplus1
	 */
	protected void tearDownTimeLoopL()
	{
		IntStream.range(0, nbCells).parallel().forEach(i1Cells -> 
		{
			vl_nplus1[i1Cells] = vl_nplus1_lplus1[i1Cells];
		});
	}

	/**
	 * Job oracleVl called @8.0 in executeTimeLoopN method.
	 * In variables: vl_nplus1
	 * Out variables: 
	 */
	protected void oracleVl()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			final boolean testVl = assertEquals(27.0, vl_nplus1[jCells]);
		});
	}

	/**
	 * Job updateVn called @8.0 in executeTimeLoopN method.
	 * In variables: vl_nplus1, vn_n
	 * Out variables: vn_nplus1
	 */
	protected void updateVn()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			vn_nplus1[jCells] = vn_n[jCells] + vl_nplus1[jCells] * 2;
		});
	}

	/**
	 * Job oracleVn called @9.0 in executeTimeLoopN method.
	 * In variables: n, vn_nplus1
	 * Out variables: 
	 */
	protected void oracleVn()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			final boolean testVn = assertEquals(54.0 * n, vn_nplus1[jCells]);
		});
	}

	private static boolean assertEquals(double expected, double actual)
	{
		final boolean ret = (expected == actual);
		if (!ret)
			throw new RuntimeException("** Assertion failed");
		return ret;
	}

	public void simulate()
	{
		System.out.println("Start execution of iteration");
		iniTime(); // @1.0
		iniVk(); // @1.0
		iniVn(); // @1.0
		setUpTimeLoopN(); // @2.0
		executeTimeLoopN(); // @3.0
		System.out.println("End of execution of iteration");
	}

	public static void main(String[] args) throws IOException
	{
		if (args.length == 1)
		{
			final String dataFileName = args[0];
			final Gson gson = new Gson();
			final JsonObject o = gson.fromJson(new FileReader(dataFileName), JsonObject.class);

			// Mesh instanciation
			assert(o.has("mesh"));
			CartesianMesh2D mesh = new CartesianMesh2D();
			mesh.jsonInit(o.get("mesh").toString());

			// Module instanciation(s)
			Iteration iteration = new Iteration(mesh);
			if (o.has("iteration")) iteration.jsonInit(o.get("iteration").toString());

			// Start simulation
			iteration.simulate();
		}
		else
		{
			System.err.println("[ERROR] Wrong number of arguments: expected 1, actual " + args.length);
			System.err.println("        Expecting user data file name, for example Iteration.json");
			System.exit(1);
		}
	}
};