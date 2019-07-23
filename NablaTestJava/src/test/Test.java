package test;

import java.util.HashMap;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.stream.IntStream;

import fr.cea.nabla.javalib.Utils;
import fr.cea.nabla.javalib.types.*;
import fr.cea.nabla.javalib.mesh.*;

@SuppressWarnings("all")
public final class Test
{
	public final static class Options
	{
		public final double X_EDGE_LENGTH = 1.0;
		public final double Y_EDGE_LENGTH = X_EDGE_LENGTH;
		public final int X_EDGE_ELEMS = 2;
		public final int Y_EDGE_ELEMS = 2;
		public final int Z_EDGE_ELEMS = 1;
		public final double option_stoptime = 0.1;
		public final int option_max_iterations = 500;
	}
	
	private final Options options;

	// Mesh
	private final NumericMesh2D mesh;
	private final int nbNodes, nbCells, nbNodesOfCell;
	private final VtkFileWriter2D writer;

	// Global Variables
	private double total;
	private double[] a, b;

	// Array Variables
	private double[] X[];
	private double v[];
	private double u[];
	private double r[];
	private double Cjr[][];
	private double M[][];
	
	public Test(Options aOptions, NumericMesh2D aNumericMesh2D)
	{
		options = aOptions;
		mesh = aNumericMesh2D;
		writer = new VtkFileWriter2D("Test");

		nbNodes = mesh.getNbNodes();
		nbCells = mesh.getNbCells();
		nbNodesOfCell = NumericMesh2D.MaxNbNodesOfCell;


		// Arrays allocation
		X = new double[nbNodes][2];
		v = new double[nbNodes];
		u = new double[nbCells];
		r = new double[nbCells];
		Cjr = new double[nbCells][nbNodesOfCell];
		M = new double[nbCells][nbCells];
		a = new double[2];
		b = new double[2];

		// Copy node coordinates
		ArrayList<double[]> gNodes = mesh.getGeometricMesh().getNodes();
		IntStream.range(0, nbNodes).parallel().forEach(rNodes -> X[rNodes] = gNodes.get(rNodes));
	}
	
	/**
	 * Job TestMatrix @-1.0
	 * In variables: M, u
	 * Out variables: r
	 */
	private void testMatrix() 
	{
		r = LinearAlgebraFunctions.solveLinearSystem(M, u, u);
	}		

	public void simulate()
	{
		System.out.println("Début de l'exécution du module Test");
		testMatrix(); // @-1.0
		System.out.println("Fin de l'exécution du module Test");
	}

	public static void main(String[] args)
	{
		Test.Options o = new Test.Options();
		Mesh<double[]> gm = CartesianMesh2DGenerator.generate(o.X_EDGE_ELEMS, o.Y_EDGE_ELEMS, o.X_EDGE_LENGTH, o.Y_EDGE_LENGTH);
		NumericMesh2D nm = new NumericMesh2D(gm);
		Test i = new Test(o, nm);
		i.simulate();
	}
};
