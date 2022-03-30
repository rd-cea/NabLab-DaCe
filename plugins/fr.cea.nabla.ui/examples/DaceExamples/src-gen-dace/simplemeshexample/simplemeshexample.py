"""
DO NOT EDIT THIS FILE - it is machine generated
"""
import sys
import json
import numpy as np
from cartesianmesh2d import CartesianMesh2D
from pvdfilewriter2d import PvdFileWriter2D

class SimpleMeshExample:
	stopTime = 1.0
	maxIterations = 1
	deltat = 0.001
	
	# dace symbols for connectivities
	nbNodes = dace.symbol("nbNodes")
	nbCells = dace.symbol("nbCells")

	def __init__(self, mesh):
		self.__mesh = mesh
		self.__nbNodes = mesh.nbNodes
		self.__nbCells = mesh.nbCells

	def jsonInit(self, jsonContent):
		self.__outputPath = jsonContent["outputPath"]
		self.__writer = PvdFileWriter2D("SimpleMeshExample", self.__outputPath)
		self.outputPeriod = jsonContent["outputPeriod"]
		self.lastDump = -sys.maxsize - 1
		self.n = 0
		self.X = np.empty((self.__nbNodes, 2), dtype=np.double)
		self.nodes_sum = np.empty((self.__nbCells), dtype=np.double)
		self.cst = np.empty((self.__nbNodes), dtype=np.double)

		# Copy node coordinates
		gNodes = mesh.geometry.nodes
		for rNodes in range(self.__nbNodes):
			self.X[rNodes] = gNodes[rNodes]

	"""
	 Job computeCst called @1.0 in simulate method.
	 In variables: 
	 Out variables: cst
	"""
	@dace.program
	def _computeCst(cst: dace.float64[nbNodes]):
		for rNodes in range(self.__nbNodes):
			self.cst[rNodes] = 1.0

	"""
	 Job computeTn called @1.0 in executeTimeLoopN method.
	 In variables: deltat, t_n
	 Out variables: t_nplus1
	"""
	@dace.program
	def _computeTn(t_n: dace.scalar(dace.float64), deltat: dace.scalar(dace.float64), t_nplus1: dace.scalar(dace.float64)):
		self.t_nplus1 = self.t_n + self.deltat

	"""
	 Job initTime called @1.0 in simulate method.
	 In variables: 
	 Out variables: t_n0
	"""
	@dace.program
	def _initTime(t_n0: dace.scalar(dace.float64)):
		self.t_n0 = 0.0

	"""
	 Job computeSum called @2.0 in simulate method.
	 In variables: cst
	 Out variables: nodes_sum
	"""
	@dace.program
	def _computeSum(cst: dace.float64[nbNodes], nodes_sum: dace.float64[nbCells]):
		for jCells in range(self.__nbCells):
			jId = jCells
			reduction0 = 0.0
			nodesOfCellJ = mesh.getNodesOfCell(jId)
			nbNodesOfCellJ = len(nodesOfCellJ)
			for rNodesOfCellJ in range(nbNodesOfCellJ):
				rId = nodesOfCellJ[rNodesOfCellJ]
				rNodes = rId
				reduction0 = self.__sumR0(reduction0, self.cst[rNodes] * jCells)
			self.nodes_sum[jCells] = reduction0

	"""
	 Job setUpTimeLoopN called @2.0 in simulate method.
	 In variables: t_n0
	 Out variables: t_n
	"""
	@dace.program
	def _setUpTimeLoopN(t_n0: dace.scalar(dace.float64), t_n: dace.scalar(dace.float64)):
		self.t_n = self.t_n0

	"""
	 Job assertSum called @3.0 in simulate method.
	 In variables: nodes_sum
	 Out variables: 
	"""
	@dace.program
	def _assertSum(nodes_sum: dace.float64[nbCells]):
		for jCells in range(self.__nbCells):
			b = self.__assertEquals1(4.0 * jCells, self.nodes_sum[jCells])

	"""
	 Job executeTimeLoopN called @3.0 in simulate method.
	 In variables: lastDump, maxIterations, n, outputPeriod, t_n
	 Out variables: t_nplus1
	"""
	@dace.program
	def _executeTimeLoopN(t_n: dace.scalar(dace.float64), n: dace.scalar(dace.int64), maxIterations: dace.scalar(dace.int64), lastDump: dace.scalar(dace.int64), outputPeriod: dace.scalar(dace.int64), t_nplus1: dace.scalar(dace.float64)):
		self.n = 0
		self.t_n = 0.0
		continueLoop = True
		while continueLoop:
			self.n += 1
			print("START ITERATION n: %5d - t: %5.5f - deltat: %5.5f\n" % (self.n, self.t_n, self.deltat))
			if (self.n >= self.lastDump + self.outputPeriod):
				self.__dumpVariables(self.n)
		
			self._computeTn() # @1.0
		
			# Evaluate loop condition with variables at time n
			continueLoop = (self.n < self.maxIterations)
		
			self.t_n = self.t_nplus1
		
		print("FINAL TIME: %5.5f - deltat: %5.5f\n" % (self.t_n, self.deltat))
		self.__dumpVariables(self.n+1);

	@dace.program
	def __assertEquals1(expected: dace.scalar(dace.float64), actual: dace.scalar(dace.float64)):
		ret = (expected == actual)
		if not ret:
			raise Exception("** Assertion failed");
		return ret

	@dace.program
	def __sumR0(a: dace.scalar(dace.float64), b: dace.scalar(dace.float64)):
		return a + b

	def simulate(self):
		print("Start execution of simpleMeshExample")
		self._computeCst() # @1.0
		self._initTime() # @1.0
		self._computeSum() # @2.0
		self._setUpTimeLoopN() # @2.0
		self._assertSum() # @3.0
		self._executeTimeLoopN() # @3.0
		print("End of execution of simpleMeshExample")

	def __dumpVariables(self, iteration):
		if not self.__writer.disabled:
			quads = mesh.geometry.quads
			self.__writer.startVtpFile(iteration, self.t_n, self.X, quads)
			self.__writer.openNodeData()
			self.__writer.closeNodeData()
			self.__writer.openCellData()
			self.__writer.openCellArray("sum", 0);
			for i in range(self.__nbCells):
				self.__writer.write(self.nodes_sum[i])
			self.__writer.closeCellArray()
			self.__writer.closeCellData()
			self.__writer.closeVtpFile()
			self.lastDump = self.n

if __name__ == '__main__':
	args = sys.argv[1:]
	
	if len(args) == 1:
		f = open(args[0])
		data = json.load(f)
		f.close()

		# Mesh instanciation
		mesh = CartesianMesh2D()
		mesh.jsonInit(data["mesh"])

		# Module instanciation
		simpleMeshExample = SimpleMeshExample(mesh)
		simpleMeshExample.jsonInit(data["simpleMeshExample"])

		# Start simulation
		simpleMeshExample.simulate()
	else:
		print("[ERROR] Wrong number of arguments: expected 1, actual " + str(len(args)), file=sys.stderr)
		print("        Expecting user data file name, for example SimpleMeshExample.json", file=sys.stderr)
		exit(1)
