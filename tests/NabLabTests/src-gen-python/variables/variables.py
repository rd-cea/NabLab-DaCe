"""
DO NOT EDIT THIS FILE - it is machine generated
"""
import sys
import json
import numpy as np
from cartesianmesh2d import CartesianMesh2D

class Variables:
	maxTime = 0.1
	maxIter = 500
	deltat = 1.0
	t = 0.0
	constexprDim = 2
	constexprVec = np.array([1.1, 1.1], dtype=np.double)

	def __init__(self, mesh):
		self.__mesh = mesh
		self.__nbNodes = mesh.nbNodes

	def jsonInit(self, jsonContent):
		self.X = np.empty((self.__nbNodes, 2), dtype=np.double)
		self.optDim = jsonContent["optDim"]
		self.optVect1 = np.empty((2), dtype=np.double)
		for i1 in range(2):
			self.optVect1[i1] = jsonContent["optVect1"][i1]
		self.optVect2 = np.empty((2), dtype=np.double)
		for i1 in range(2):
			self.optVect2[i1] = jsonContent["optVect2"][i1]
		self.optVect3 = np.empty((2), dtype=np.double)
		self.optVect3 = self.__operatorAdd(self.optVect1, self.optVect2)
		self.varVec = np.empty((self.constexprDim), dtype=np.double)
		self.varVec = np.array([1.0, 1.0], dtype=np.double)
		self.dynamicVec = np.empty((self.optDim), dtype=np.double)

		# Copy node coordinates
		gNodes = mesh.geometry.nodes
		for rNodes in range(self.__nbNodes):
			self.X[rNodes] = gNodes[rNodes]

	"""
	 Job dynamicVecInitialization called @1.0 in simulate method.
	 In variables: optDim
	 Out variables: checkDynamicDim, dynamicVec
	"""
	def _dynamicVecInitialization(self):
		cpt = 0
		for i in range(self.optDim):
			self.dynamicVec[i] = 3.3
		for i in range(self.optDim):
			cpt = cpt + 1
		self.checkDynamicDim = cpt

	"""
	 Job varVecInitialization called @1.0 in simulate method.
	 In variables: constexprDim
	 Out variables: varVec
	"""
	def _varVecInitialization(self):
		self.varVec = np.array([2.2, 2.2], dtype=np.double)

	"""
	 Job oracle called @2.0 in simulate method.
	 In variables: checkDynamicDim, constexprDim, constexprVec, dynamicVec, optDim, optVect1, optVect2, optVect3, varVec
	 Out variables: 
	"""
	def _oracle(self):
		testOptDim = self.__assertEquals(2, self.optDim)
		testOptVect1 = self.__assertEquals3(np.array([1.0, 1.0], dtype=np.double), self.optVect1)
		testOptVect2 = self.__assertEquals3(np.array([2.0, 2.0], dtype=np.double), self.optVect2)
		testOptVect3 = self.__assertEquals3(np.array([3.0, 3.0], dtype=np.double), self.optVect3)
		testConstexprDim = self.__assertEquals(2, self.constexprDim)
		testConstexprVec = self.__assertEquals3(np.array([1.1, 1.1], dtype=np.double), self.constexprVec)
		testVarVec = self.__assertEquals3(np.array([2.2, 2.2], dtype=np.double), self.varVec)
		testDynamicVecLength = self.__assertEquals(2, self.checkDynamicDim)
		for i in range(self.optDim):
			testDynamicVec = self.__assertEquals1(3.3, self.dynamicVec[i])

	def __assertEquals(self, expected, actual):
		ret = (expected == actual)
		if not ret:
			raise Exception("** Assertion failed");
		return ret

	def __assertEquals3(self, expected, actual):
		for i in range(len(expected)):
			if expected[i] != actual[i]:
				raise Exception("** Assertion failed");
		return True

	def __assertEquals1(self, expected, actual):
		ret = (expected == actual)
		if not ret:
			raise Exception("** Assertion failed");
		return ret

	def __operatorAdd(self, a, b):
		result = np.empty((len(a)), dtype=np.double)
		for ix0 in range(len(a)):
			result[ix0] = a[ix0] + b[ix0]
		return result

	def simulate(self):
		print("Start execution of variables")
		self._dynamicVecInitialization() # @1.0
		self._varVecInitialization() # @1.0
		self._oracle() # @2.0
		print("End of execution of variables")

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
		variables = Variables(mesh)
		variables.jsonInit(data["variables"])

		# Start simulation
		variables.simulate()
	else:
		print("[ERROR] Wrong number of arguments: expected 1, actual " + str(len(args)), file=sys.stderr)
		print("        Expecting user data file name, for example Variables.json", file=sys.stderr)
		exit(1)
