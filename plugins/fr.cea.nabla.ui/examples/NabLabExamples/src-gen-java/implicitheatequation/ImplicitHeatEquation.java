/* DO NOT EDIT THIS FILE - it is machine generated */

package implicitheatequation;

import java.io.FileReader;
import java.io.IOException;
import java.util.stream.IntStream;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonElement;

import fr.cea.nabla.javalib.mesh.*;

public final class ImplicitHeatEquation
{
	// Mesh and mesh variables
	private final CartesianMesh2D mesh;
	private final int nbNodes;
	private final int nbCells;
	private final int nbFaces;
	private final int nbInnerFaces;
	// Options and global variables
	private PvdFileWriter2D writer;
	private String outputPath;
	private linearalgebrajava.LinearAlgebra linearAlgebra;
	int outputPeriod;
	int lastDump;
	int n;
	double stopTime;
	int maxIterations;
	static final double u0 = 1.0;
	static final double[] vectOne = new double[] {1.0, 1.0};
	double deltat;
	double t_n;
	double t_nplus1;
	double t_n0;
	double[][] X;
	double[][] Xc;
	linearalgebrajava.Vector u_n;
	linearalgebrajava.Vector u_nplus1;
	double[] V;
	double[] D;
	double[] faceLength;
	double[] faceConductivity;
	double[] alphaExtraDiag;
	linearalgebrajava.Matrix alpha;

	public ImplicitHeatEquation(CartesianMesh2D aMesh)
	{
		// Mesh and mesh variables initialization
		mesh = aMesh;
		nbNodes = mesh.getNbNodes();
		nbCells = mesh.getNbCells();
		nbFaces = mesh.getNbFaces();
		nbInnerFaces = mesh.getGroup("InnerFaces").length;
	}

	public void jsonInit(final String jsonContent)
	{
		final Gson gson = new Gson();
		final JsonObject options = gson.fromJson(jsonContent, JsonObject.class);
		assert(options.has("outputPath"));
		final JsonElement valueof_outputPath = options.get("outputPath");
		outputPath = valueof_outputPath.getAsJsonPrimitive().getAsString();
		writer = new PvdFileWriter2D("ImplicitHeatEquation", outputPath);
		assert(options.has("outputPeriod"));
		final JsonElement valueof_outputPeriod = options.get("outputPeriod");
		assert(valueof_outputPeriod.isJsonPrimitive());
		outputPeriod = valueof_outputPeriod.getAsJsonPrimitive().getAsInt();
		lastDump = Integer.MIN_VALUE;
		n = 0;
		assert(options.has("stopTime"));
		final JsonElement valueof_stopTime = options.get("stopTime");
		assert(valueof_stopTime.isJsonPrimitive());
		stopTime = valueof_stopTime.getAsJsonPrimitive().getAsDouble();
		assert(options.has("maxIterations"));
		final JsonElement valueof_maxIterations = options.get("maxIterations");
		assert(valueof_maxIterations.isJsonPrimitive());
		maxIterations = valueof_maxIterations.getAsJsonPrimitive().getAsInt();
		deltat = 0.001;
		X = new double[nbNodes][2];
		Xc = new double[nbCells][2];
		u_n = new linearalgebrajava.Vector("u_n", nbCells);
		u_nplus1 = new linearalgebrajava.Vector("u_nplus1", nbCells);
		V = new double[nbCells];
		D = new double[nbCells];
		faceLength = new double[nbFaces];
		faceConductivity = new double[nbFaces];
		alphaExtraDiag = new double[nbFaces];
		alpha = new linearalgebrajava.Matrix("alpha", nbCells, nbCells);
		// linearAlgebra
		linearAlgebra = new linearalgebrajava.LinearAlgebra();
		if (options.has("linearAlgebra"))
			linearAlgebra.jsonInit(options.get("linearAlgebra").toString());

		// Copy node coordinates
		double[][] gNodes = mesh.getGeometry().getNodes();
		IntStream.range(0, nbNodes).parallel().forEach(rNodes ->
		{
			X[rNodes][0] = gNodes[rNodes][0];
			X[rNodes][1] = gNodes[rNodes][1];
		});
	}

	/**
	 * Job computeFaceLength called @1.0 in simulate method.
	 * In variables: X
	 * Out variables: faceLength
	 */
	protected void computeFaceLength()
	{
		IntStream.range(0, nbFaces).parallel().forEach(fFaces -> 
		{
			final int fId = fFaces;
			double reduction0 = 0.0;
			{
				final int[] nodesOfFaceF = mesh.getNodesOfFace(fId);
				final int nbNodesOfFaceF = nodesOfFaceF.length;
				for (int pNodesOfFaceF=0; pNodesOfFaceF<nbNodesOfFaceF; pNodesOfFaceF++)
				{
					final int pId = nodesOfFaceF[pNodesOfFaceF];
					final int pPlus1Id = nodesOfFaceF[(pNodesOfFaceF+1+nbNodesOfFaceF)%nbNodesOfFaceF];
					final int pNodes = pId;
					final int pPlus1Nodes = pPlus1Id;
					reduction0 = sumR0(reduction0, norm(operatorSub(X[pNodes], X[pPlus1Nodes])));
				}
			}
			faceLength[fFaces] = 0.5 * reduction0;
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
	 * Job computeV called @1.0 in simulate method.
	 * In variables: X
	 * Out variables: V
	 */
	protected void computeV()
	{
		IntStream.range(0, nbCells).parallel().forEach(jCells -> 
		{
			final int jId = jCells;
			double reduction0 = 0.0;
			{
				final int[] nodesOfCellJ = mesh.getNodesOfCell(jId);
				final int nbNodesOfCellJ = nodesOfCellJ.length;
				for (int pNodesOfCellJ=0; pNodesOfCellJ<nbNodesOfCellJ; pNodesOfCellJ++)
				{
					final int pId = nodesOfCellJ[pNodesOfCellJ];
					final int pPlus1Id = nodesOfCellJ[(pNodesOfCellJ+1+nbNodesOfCellJ)%nbNodesOfCellJ];
					final int pNodes = pId;
					final int pPlus1Nodes = pPlus1Id;
					reduction0 = sumR0(reduction0, det(X[pNodes], X[pPlus1Nodes]));
				}
			}
			V[jCells] = 0.5 * reduction0;
		});
	}

	/**
	 * Job initD called @1.0 in simulate method.
	 * In variables: 
	 * Out variables: D
	 */
	protected void initD()
	{
		IntStream.range(0, nbCells).parallel().forEach(cCells -> 
		{
			D[cCells] = 1.0;
		});
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
	 * Job initXc called @1.0 in simulate method.
	 * In variables: X
	 * Out variables: Xc
	 */
	protected void initXc()
	{
		IntStream.range(0, nbCells).parallel().forEach(cCells -> 
		{
			final int cId = cCells;
			double[] reduction0 = new double[] {0.0, 0.0};
			{
				final int[] nodesOfCellC = mesh.getNodesOfCell(cId);
				final int nbNodesOfCellC = nodesOfCellC.length;
				for (int pNodesOfCellC=0; pNodesOfCellC<nbNodesOfCellC; pNodesOfCellC++)
				{
					final int pId = nodesOfCellC[pNodesOfCellC];
					final int pNodes = pId;
					reduction0 = sumR1(reduction0, X[pNodes]);
				}
			}
			Xc[cCells] = operatorMult(0.25, reduction0);
		});
	}

	/**
	 * Job updateU called @1.0 in executeTimeLoopN method.
	 * In variables: alpha, u_n
	 * Out variables: u_nplus1
	 */
	protected void updateU()
	{
		u_nplus1 = linearAlgebra.solveLinearSystem(alpha, u_n);
	}

	/**
	 * Job computeDeltaTn called @2.0 in simulate method.
	 * In variables: D, V
	 * Out variables: deltat
	 */
	protected void computeDeltaTn()
	{
		double reduction0 = Double.MAX_VALUE;
		reduction0 = IntStream.range(0, nbCells).boxed().parallel().reduce
		(
			Double.MAX_VALUE,
			(accu, cCells) ->
			{
				return minR0(accu, V[cCells] / D[cCells]);
			},
			(r1, r2) -> minR0(r1, r2)
		);
		deltat = reduction0 * 0.24;
	}

	/**
	 * Job computeFaceConductivity called @2.0 in simulate method.
	 * In variables: D
	 * Out variables: faceConductivity
	 */
	protected void computeFaceConductivity()
	{
		IntStream.range(0, nbFaces).parallel().forEach(fFaces -> 
		{
			final int fId = fFaces;
			double reduction0 = 1.0;
			{
				final int[] cellsOfFaceF = mesh.getCellsOfFace(fId);
				final int nbCellsOfFaceF = cellsOfFaceF.length;
				for (int c1CellsOfFaceF=0; c1CellsOfFaceF<nbCellsOfFaceF; c1CellsOfFaceF++)
				{
					final int c1Id = cellsOfFaceF[c1CellsOfFaceF];
					final int c1Cells = c1Id;
					reduction0 = prodR0(reduction0, D[c1Cells]);
				}
			}
			double reduction1 = 0.0;
			{
				final int[] cellsOfFaceF = mesh.getCellsOfFace(fId);
				final int nbCellsOfFaceF = cellsOfFaceF.length;
				for (int c2CellsOfFaceF=0; c2CellsOfFaceF<nbCellsOfFaceF; c2CellsOfFaceF++)
				{
					final int c2Id = cellsOfFaceF[c2CellsOfFaceF];
					final int c2Cells = c2Id;
					reduction1 = sumR0(reduction1, D[c2Cells]);
				}
			}
			faceConductivity[fFaces] = 2.0 * reduction0 / reduction1;
		});
	}

	/**
	 * Job initU called @2.0 in simulate method.
	 * In variables: Xc, u0, vectOne
	 * Out variables: u_n
	 */
	protected void initU()
	{
		IntStream.range(0, nbCells).parallel().forEach(cCells -> 
		{
			if (norm(operatorSub(Xc[cCells], vectOne)) < 0.5)
				u_n.setValue(cCells, u0);
			else
				u_n.setValue(cCells, 0.0);
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
	 * Job computeAlphaExtraDiag called @3.0 in simulate method.
	 * In variables: V, Xc, deltat, faceConductivity, faceLength
	 * Out variables: alphaExtraDiag
	 */
	protected void computeAlphaExtraDiag()
	{
		{
			final int[] innerFaces = mesh.getGroup("InnerFaces");
			IntStream.range(0, nbInnerFaces).parallel().forEach(fInnerFaces -> 
			{
				final int fId = innerFaces[fInnerFaces];
				final int fFaces = fId;
				final int cId = mesh.getBackCell(fId);
				final int cCells = cId;
				final int dId = mesh.getFrontCell(fId);
				final int dCells = dId;
				alphaExtraDiag[fFaces] = -deltat / V[cCells] * (faceLength[fFaces] * faceConductivity[fFaces]) / norm(operatorSub(Xc[cCells], Xc[dCells]));
			});
		}
	}

	/**
	 * Job assembleAlphaDiag called @4.0 in simulate method.
	 * In variables: alphaExtraDiag
	 * Out variables: alpha
	 */
	protected void assembleAlphaDiag()
	{
		IntStream.range(0, nbCells).parallel().forEach(cCells -> 
		{
			final int cId = cCells;
			double reduction0 = 0.0;
			{
				final int[] facesOfCellC = mesh.getFacesOfCell(cId);
				final int nbFacesOfCellC = facesOfCellC.length;
				for (int fFacesOfCellC=0; fFacesOfCellC<nbFacesOfCellC; fFacesOfCellC++)
				{
					final int fId = facesOfCellC[fFacesOfCellC];
					final int fFaces = fId;
					reduction0 = sumR0(reduction0, alphaExtraDiag[fFaces]);
				}
			}
			alpha.setValue(cCells, cCells, 1 - reduction0);
		});
	}

	/**
	 * Job assembleAlphaExtraDiag called @4.0 in simulate method.
	 * In variables: alphaExtraDiag
	 * Out variables: alpha
	 */
	protected void assembleAlphaExtraDiag()
	{
		{
			final int[] innerFaces = mesh.getGroup("InnerFaces");
			IntStream.range(0, nbInnerFaces).parallel().forEach(fInnerFaces -> 
			{
				final int fId = innerFaces[fInnerFaces];
				final int fFaces = fId;
				final int cId = mesh.getBackCell(fId);
				final int cCells = cId;
				final int dId = mesh.getFrontCell(fId);
				final int dCells = dId;
				alpha.setValue(cCells, dCells, alphaExtraDiag[fFaces]);
				alpha.setValue(dCells, cCells, alphaExtraDiag[fFaces]);
			});
		}
	}

	/**
	 * Job executeTimeLoopN called @5.0 in simulate method.
	 * In variables: lastDump, maxIterations, n, outputPeriod, stopTime, t_n, t_nplus1, u_n
	 * Out variables: t_nplus1, u_nplus1
	 */
	protected void executeTimeLoopN()
	{
		n = 0;
		boolean continueLoop = true;
		do
		{
			n++;
			System.out.printf("START ITERATION n: %5d - t: %5.5f - deltat: %5.5f\n", n, t_n, deltat);
			if (n >= lastDump + outputPeriod)
				dumpVariables(n);
		
			computeTn(); // @1.0
			updateU(); // @1.0
		
			// Evaluate loop condition with variables at time n
			continueLoop = (t_nplus1 < stopTime && n + 1 < maxIterations);
		
			// fr.cea.nabla.ir.ir.impl.AffectationImpl
			// fr.cea.nabla.ir.ir.impl.AffectationImpl
			// instruction content
			t_n = t_nplus1;
			u_n = u_nplus1;
		} while (continueLoop);
		
		System.out.printf("FINAL TIME: %5.5f - deltat: %5.5f\n", t_n, deltat);
		dumpVariables(n+1);
	}

	private static double norm(double[] a)
	{
		return Math.sqrt(dot(a, a));
	}

	private static double dot(double[] a, double[] b)
	{
		double result = 0.0;
		for (int i=0; i<a.length; i++)
		{
			result = result + a[i] * b[i];
		}
		return result;
	}

	private static double det(double[] a, double[] b)
	{
		return (a[0] * b[1] - a[1] * b[0]);
	}

	private static double[] sumR1(double[] a, double[] b)
	{
		return operatorAdd(a, b);
	}

	private static double minR0(double a, double b)
	{
		return Math.min(a, b);
	}

	private static double sumR0(double a, double b)
	{
		return a + b;
	}

	private static double prodR0(double a, double b)
	{
		return a * b;
	}

	private static double[] operatorAdd(double[] a, double[] b)
	{
		double[] result = new double[a.length];
		for (int ix0=0; ix0<a.length; ix0++)
		{
			result[ix0] = a[ix0] + b[ix0];
		}
		return result;
	}

	private static double[] operatorMult(double a, double[] b)
	{
		double[] result = new double[b.length];
		for (int ix0=0; ix0<b.length; ix0++)
		{
			result[ix0] = a * b[ix0];
		}
		return result;
	}

	private static double[] operatorSub(double[] a, double[] b)
	{
		double[] result = new double[a.length];
		for (int ix0=0; ix0<a.length; ix0++)
		{
			result[ix0] = a[ix0] - b[ix0];
		}
		return result;
	}

	public void simulate()
	{
		System.out.println("Start execution of implicitHeatEquation");
		computeFaceLength(); // @1.0
		computeV(); // @1.0
		initD(); // @1.0
		initTime(); // @1.0
		initXc(); // @1.0
		computeDeltaTn(); // @2.0
		computeFaceConductivity(); // @2.0
		initU(); // @2.0
		setUpTimeLoopN(); // @2.0
		computeAlphaExtraDiag(); // @3.0
		assembleAlphaDiag(); // @4.0
		assembleAlphaExtraDiag(); // @4.0
		executeTimeLoopN(); // @5.0
		System.out.println("End of execution of implicitHeatEquation");
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
			ImplicitHeatEquation implicitHeatEquation = new ImplicitHeatEquation(mesh);
			if (o.has("implicitHeatEquation")) implicitHeatEquation.jsonInit(o.get("implicitHeatEquation").toString());

			// Start simulation
			implicitHeatEquation.simulate();
		}
		else
		{
			System.err.println("[ERROR] Wrong number of arguments: expected 1, actual " + args.length);
			System.err.println("        Expecting user data file name, for example ImplicitHeatEquation.json");
			System.exit(1);
		}
	}

	private void dumpVariables(int iteration)
	{
		if (!writer.isDisabled())
		{
			try
			{
				Quad[] quads = mesh.getGeometry().getQuads();
				writer.startVtpFile(iteration, t_n, X, quads);
				writer.openNodeData();
				writer.closeNodeData();
				writer.openCellData();
				writer.openCellArray("Temperature", 1);
				for (int i=0 ; i<nbCells ; ++i)
					writer.write(u_n.getValue(i));
				writer.closeCellArray();
				writer.closeCellData();
				writer.closeVtpFile();
				lastDump = n;
			}
			catch (java.io.FileNotFoundException e)
			{
				System.out.println("* WARNING: no dump of variables. FileNotFoundException: " + e.getMessage());
			}
		}
	}
};
