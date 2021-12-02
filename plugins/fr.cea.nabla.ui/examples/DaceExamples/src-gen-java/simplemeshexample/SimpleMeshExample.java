/* DO NOT EDIT THIS FILE - it is machine generated */

package simplemeshexample;

import java.io.FileReader;
import java.io.IOException;
import java.util.stream.IntStream;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

import fr.cea.nabla.javalib.mesh.*;

public final class SimpleMeshExample
{
	// Mesh and mesh variables
	private final CartesianMesh2D mesh;
	@SuppressWarnings("unused")
	private final int nbNodes, nbCells, maxNodesOfCell;

	// Options and global variables
	int n;
	static final double stopTime = 1.0;
	static final int maxIterations = 1;
	static final double deltat = 0.001;
	double t_n;
	double t_nplus1;
	double t_n0;
	double[][] X;
	double[] nodes_sum;
	double[] cst;

	public SimpleMeshExample(CartesianMesh2D aMesh)
	{
		// Mesh and mesh variables initialization
		mesh = aMesh;
		nbNodes = mesh.getNbNodes();
		nbCells = mesh.getNbCells();
		maxNodesOfCell = CartesianMesh2D.MaxNbNodesOfCell;
	}

	public void jsonInit(final String jsonContent)
	{
		final Gson gson = new Gson();
		final JsonObject options = gson.fromJson(jsonContent, JsonObject.class);
		X = new double[nbNodes][2];
		nodes_sum = new double[nbCells];
		cst = new double[nbNodes];

		// Copy node coordinates
		double[][] gNodes = mesh.getGeometry().getNodes();
		IntStream.range(0, nbNodes).parallel().forEach(rNodes ->
		{
			X[rNodes][0] = gNodes[rNodes][0];
			X[rNodes][1] = gNodes[rNodes][1];
		});
	}

	/**
	 * Job computeCst called @1.0 in simulate method.
	 * In variables: 
	 * Out variables: cst
	 */
	protected void computeCst()
	{
		IntStream.range(0, nbNodes).parallel().forEach(rNodes -> 
		{
			cst[rNodes] = 1.0;
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
	 * Job initTime called @1.0 in simulate method.
	 * In variables: 
	 * Out variables: t_n0
	 */
	protected void initTime()
	{
		t_n0 = 0.0;
	}

	/**
	 * Job computeSum called @2.0 in simulate method.
	 * In variables: cst
	 * Out variables: nodes_sum
	 */
	protected void computeSum()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			final int jId = jCells;
			double reduction0 = 0.0;
			{
				final int[] nodesOfCellJ = mesh.getNodesOfCell(jId);
				final int nbNodesOfCellJ = nodesOfCellJ.length;
				for (int rNodesOfCellJ=0; rNodesOfCellJ<nbNodesOfCellJ; rNodesOfCellJ++)
				{
					final int rId = nodesOfCellJ[rNodesOfCellJ];
					final int rNodes = rId;
					reduction0 = sumR0(reduction0, cst[rNodes] * jCells);
				}
			}
			nodes_sum[jCells] = reduction0;
		});
	}

	/**
	 * Job setUpTimeLoopN called @2.0 in simulate method.
	 * In variables: t_n0
	 * Out variables: t_n
	 */
	protected void setUpTimeLoopN()
	{
		t_n = t_n0;
	}

	/**
	 * Job assertSum called @3.0 in simulate method.
	 * In variables: nodes_sum
	 * Out variables: 
	 */
	protected void assertSum()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			final boolean b = assertEquals(4.0 * jCells, nodes_sum[jCells]);
		});
	}

	/**
	 * Job executeTimeLoopN called @3.0 in simulate method.
	 * In variables: maxIterations, n, t_n
	 * Out variables: t_nplus1
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
		
			// Evaluate loop condition with variables at time n
			continueLoop = (n < maxIterations);
		
			t_n = t_nplus1;
		} while (continueLoop);
		
		System.out.printf("FINAL TIME: %5.5f - deltat: %5.5f\n", t_n, deltat);
	}

	private static boolean assertEquals(double expected, double actual)
	{
		final boolean ret = (expected == actual);
		if (!ret)
			throw new RuntimeException("** Assertion failed");
		return ret;
	}

	private static double sumR0(double a, double b)
	{
		return a + b;
	}

	public void simulate()
	{
		System.out.println("Start execution of simpleMeshExample");
		computeCst(); // @1.0
		initTime(); // @1.0
		computeSum(); // @2.0
		setUpTimeLoopN(); // @2.0
		assertSum(); // @3.0
		executeTimeLoopN(); // @3.0
		System.out.println("End of execution of simpleMeshExample");
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
			SimpleMeshExample simpleMeshExample = new SimpleMeshExample(mesh);
			if (o.has("simpleMeshExample")) simpleMeshExample.jsonInit(o.get("simpleMeshExample").toString());

			// Start simulation
			simpleMeshExample.simulate();
		}
		else
		{
			System.err.println("[ERROR] Wrong number of arguments: expected 1, actual " + args.length);
			System.err.println("        Expecting user data file name, for example SimpleMeshExample.json");
			System.exit(1);
		}
	}
};
