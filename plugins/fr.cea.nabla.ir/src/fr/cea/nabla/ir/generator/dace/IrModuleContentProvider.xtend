/*******************************************************************************
 * Copyright (c) 2022 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.ir.generator.dace

import fr.cea.nabla.ir.IrTypeExtensions
import fr.cea.nabla.ir.IrUtils
import fr.cea.nabla.ir.generator.Utils
import fr.cea.nabla.ir.ir.BaseType
import fr.cea.nabla.ir.ir.ConnectivityType
import fr.cea.nabla.ir.ir.IrModule
import fr.cea.nabla.ir.ir.LinearAlgebraType
import fr.cea.nabla.ir.ir.PrimitiveType
import fr.cea.nabla.ir.ir.Variable

import static fr.cea.nabla.ir.generator.python.JsonContentProvider.*
import static fr.cea.nabla.ir.generator.python.TypeContentProvider.*

import static extension fr.cea.nabla.ir.ExtensionProviderExtensions.*
import static extension fr.cea.nabla.ir.IrModuleExtensions.*
import static extension fr.cea.nabla.ir.IrRootExtensions.*
import static extension fr.cea.nabla.ir.IrTypeExtensions.*
import static extension fr.cea.nabla.ir.generator.python.ExpressionContentProvider.*
import static extension fr.cea.nabla.ir.generator.python.FunctionContentProvider.*
import static extension fr.cea.nabla.ir.generator.python.JobContentProvider.*

class IrModuleContentProvider
{
	static def getFileContent(IrModule it, boolean hasLevelDB)
	'''
	"""
	«Utils.doNotEditWarning»
	"""
	import sys
	import json
	import numpy as np
	«IF providers.exists[x | x.extensionName == "Math"]»import math«ENDIF»
	«IF main»
		«IF hasLevelDB»
			import plyvel
			import struct
			from distutils.dir_util import copy_tree
		«ENDIF»
		«FOR addM : irRoot.modules.filter[x | !x.main]»
			from «addM.name.toLowerCase» import «addM.className»
		«ENDFOR»
	«ENDIF»
	from «irRoot.mesh.className.toLowerCase» import «irRoot.mesh.className»
	«IF irRoot.postProcessing !== null»from pvdfilewriter2d import PvdFileWriter2D«ENDIF»
	«FOR provider : externalProviders»
		from «provider.className.toLowerCase» import «provider.className»
		«IF provider.linearAlgebra»
			from «IrTypeExtensions.VectorClass.toLowerCase» import «IrTypeExtensions.VectorClass»
			from «IrTypeExtensions.MatrixClass.toLowerCase» import «IrTypeExtensions.MatrixClass»
		«ENDIF»
	«ENDFOR»

	class «className»:
		«FOR v : variables.filter[constExpr]»
			«v.name» = «v.defaultValue.content»
		«ENDFOR»

		def __init__(self, mesh):
			self.__mesh = mesh
			«FOR a: neededConnectivityAttributes»
				«PythonGeneratorUtils.getNbElemsVar(a)» = mesh.nb«a.name.toFirstUpper»
			«ENDFOR»
			«FOR a : neededGroupAttributes»
				self.__«PythonGeneratorUtils.getNbElemsVar(a)» = len(mesh.getGroup("«a»"))
			«ENDFOR»

		def jsonInit(self, jsonContent):
			«IF postProcessing !== null»
				«val opName = IrUtils.OutputPathNameAndValue.key»
				self.__«opName» = jsonContent["«opName»"]
				self.__writer = PvdFileWriter2D("«irRoot.name»", self.__«opName»)
			«ENDIF»
			«FOR v : variables.filter[!constExpr]»
				«IF !v.type.scalar»
					«PythonGeneratorUtils.getCodeName(v)»«getPythonAllocation(v.type, v.name)»
				«ENDIF»
				«IF v.option»
					«getJsonContent(v.name, v.type as BaseType)»
				«ELSEIF v.defaultValue !== null»
					«PythonGeneratorUtils.getCodeName(v)» = «getContent(v.defaultValue)»
				«ENDIF»
			«ENDFOR»
			«FOR v : externalProviders»
				«val vName = v.instanceName»
				# «vName»
				self.«vName» = «v.className»()
				if jsonContent["«vName»"]:
					«vName».jsonInit(jsonContent["«vName»"])
			«ENDFOR»
			«val nrName = IrUtils.NonRegressionNameAndValue.key»
			«IF main && hasLevelDB»

				# Non regression
				if "«nrName»" in jsonContent:
					self.«nrName» = jsonContent["«nrName»"]
			«ENDIF»
			«IF main»

				# Copy node coordinates
				gNodes = mesh.geometry.nodes
				for rNodes in range(self.__nbNodes):
					self.«irRoot.initNodeCoordVariable.name»[rNodes] = gNodes[rNodes]
			«ENDIF»
		«FOR j : jobs»

			«j.content»
		«ENDFOR»
		«FOR f : functions»

			«f.content»
		«ENDFOR»

		«IF main»
			def simulate(self):
				print("Start execution of «name»")
				«FOR j : irRoot.main.calls»
					«PythonGeneratorUtils.getCallName(j)»() # @«j.at»
				«ENDFOR»
				print("End of execution of «name»")
			«FOR addM : irRoot.modules.filter[x | !x.main]»

				def set_«addM.name.toLowerCase»(self, value):
					self._«addM.name.toLowerCase» = value
			«ENDFOR»
		«ELSE /* !main */»
			def set_mainModule(self, value):
				self._mainModule = value
				self._mainModule.set_«name»(self)
		«ENDIF»
		«IF main && hasLevelDB»

		def create_db(self, dbName):
			# Destroy if exists
			plyvel.destroy_db(dbName)

			# Create database
			db = plyvel.DB(dbName, create_if_missing=True)	

			with db.write_batch() as b:
				«FOR v : irRoot.variables.filter[!option]»
					«IF v.type.scalar»
						b.put(b"«Utils.getDbKey(v)»", struct.pack('«getStructFormat(v.type.primitive)»', «getDbValue(it, v)»))
					«ELSEIF v.type instanceof LinearAlgebraType»
						b.put(b"«Utils.getDbKey(v)»", «getDbValue(it, v)».getData().tobytes())
					«ELSE»
						b.put(b"«Utils.getDbKey(v)»", «getDbValue(it, v)».tobytes())
					«ENDIF»
				«ENDFOR»
			
			db.close()
			print("Reference database " + dbName + " created.")
		«ENDIF»
		«IF postProcessing !== null»

			def __dumpVariables(self, iteration):
				if not self.__writer.disabled:
					quads = mesh.geometry.quads
					self.__writer.startVtpFile(iteration, «PythonGeneratorUtils.getCodeName(irRoot.currentTimeVariable)», «PythonGeneratorUtils.getCodeName(irRoot.nodeCoordVariable)», quads)
					self.__writer.openNodeData()
					«val outputVarsByConnectivities = irRoot.postProcessing.outputVariables.groupBy(x | x.support.name)»
					«val nodeVariables = outputVarsByConnectivities.get("node")»
					«IF !nodeVariables.nullOrEmpty»
						«FOR v : nodeVariables»
							self.__writer.openNodeArray("«v.outputName»", «v.target.type.baseSizes.size»);
							for i in range(self.__nbNodes):
								self.__writer.write(«getWriteCallContent(v.target)»)
							self.__writer.closeNodeArray()
						«ENDFOR»
					«ENDIF»
					self.__writer.closeNodeData()
					self.__writer.openCellData()
					«val cellVariables = outputVarsByConnectivities.get("cell")»
					«IF !cellVariables.nullOrEmpty»
						«FOR v : cellVariables»
							self.__writer.openCellArray("«v.outputName»", «v.target.type.baseSizes.size»);
							for i in range(self.__nbCells):
								self.__writer.write(«getWriteCallContent(v.target)»)
							self.__writer.closeCellArray()
						«ENDFOR»
					«ENDIF»
					self.__writer.closeCellData()
					self.__writer.closeVtpFile()
					«PythonGeneratorUtils.getCodeName(postProcessing.lastDumpVariable)» = «PythonGeneratorUtils.getCodeName(postProcessing.periodReference)»
		«ENDIF»
	«IF main && hasLevelDB»

		def compare_db(currentName, refName):
			result = True

			try:
				dbRef = plyvel.DB(refName)
				itRef = dbRef.iterator()

				db = plyvel.DB(currentName)
				it = db.iterator()

				# Results comparison
				print("# Comparing results ...");

				for key, refValue in itRef:
					currentValue = db.get(key)
					if currentValue == None:
						print("ERROR - Key : " + str(key) + " not found.")
						result = False
					else:
						ok = (currentValue == refValue)
						if ok:
							print(str(key) + ": OK")
						else:
							print(str(key) + ": ERROR")
							result = False
							print("value = " + str(currentValue))
							print("  ref = " + str(refValue))

				# looking for key in the db that are not in the ref (new variables)
				for key, currentValue in it:
					if dbRef.get(key) == None:
						print("ERROR - Key : " + key + " can not be compared (not present in the ref).")
						result = False

			finally:
				it.close()
				itRef.close()
				db.close()
				dbRef.close()

			return result
	«ENDIF»
	«IF main»

		if __name__ == '__main__':
			args = sys.argv[1:]
			
			if len(args) == 1:
				f = open(args[0])
				data = json.load(f)
				f.close()

				# Mesh instanciation
				mesh = «irRoot.mesh.className»()
				mesh.jsonInit(data["mesh"])

				# Module instanciation
				«FOR m : irRoot.modules»
					«m.name» = «m.className»(mesh)
					«IF !m.main»«m.name».set_mainModule(«irRoot.mainModule.name»)«ENDIF»
					«m.name».jsonInit(data["«m.name»"])
				«ENDFOR»

				# Start simulation
				«name».simulate()
				«IF main && hasLevelDB»

					«val dbName = irRoot.name + "DB"»
					# Non regression testing
					if «name».«nrName» != None and «name».«nrName» == "«IrUtils.NonRegressionValues.CreateReference.toString»":
						«name».create_db("«dbName».ref")
					if «name».«nrName» != None and «name».«nrName» == "«IrUtils.NonRegressionValues.CompareToReference.toString»":
						«name».create_db("«dbName».current")
						ok = compare_db("«dbName».current", "«dbName».ref");
						plyvel.destroy_db("«dbName».current");
						if not ok:
							exit(1)
				«ENDIF»
			else:
				print("[ERROR] Wrong number of arguments: expected 1, actual " + str(len(args)), file=sys.stderr)
				print("        Expecting user data file name, for example «irRoot.name».json", file=sys.stderr)
				exit(1)
	«ENDIF»
	'''

	private static def getWriteCallContent(Variable v)
	{
		val t = v.type
		switch t
		{
			ConnectivityType: '''«PythonGeneratorUtils.getCodeName(v)»[i]'''
			LinearAlgebraType: '''«PythonGeneratorUtils.getCodeName(v)».getValue(i)'''
			default: throw new RuntimeException("Unexpected type: " + t.class.name)
		}
	}

	private static def getDbValue(IrModule m, Variable v)
	{
		val vModule = IrUtils.getContainerOfType(v, IrModule)
		if (vModule == m)
			"self." + v.name
		else
			vModule.name + "." + v.name
	}

	private static def getStructFormat(PrimitiveType t)
	{
		switch t
		{
			case BOOL: '?'
			case INT: 'i'
			case REAL: 'd'
		}
	}
}