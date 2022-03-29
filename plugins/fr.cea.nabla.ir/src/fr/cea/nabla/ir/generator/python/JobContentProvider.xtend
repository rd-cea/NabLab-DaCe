/*******************************************************************************
 * Copyright (c) 2021 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.ir.generator.python

import fr.cea.nabla.ir.IrUtils
import fr.cea.nabla.ir.ir.ExecuteTimeLoopJob
import fr.cea.nabla.ir.ir.IrRoot
import fr.cea.nabla.ir.ir.Job

import static extension fr.cea.nabla.ir.JobCallerExtensions.*
import static extension fr.cea.nabla.ir.generator.Utils.*
import static extension fr.cea.nabla.ir.generator.python.ExpressionContentProvider.*
import static extension fr.cea.nabla.ir.generator.python.InstructionContentProvider.*
import fr.cea.nabla.ir.ir.Variable

class JobContentProvider 
{
	static def getContent(Job it)
	'''
		"""
		 Job «getCodeName» called @«at» in «caller.codeName» method.
		 In variables: «FOR v : inVars.sortBy[name] SEPARATOR ', '»«v.getName»«ENDFOR»
		 Out variables: «FOR v : outVars.sortBy[name] SEPARATOR ', '»«v.getName»«ENDFOR»
		"""
		def _«codeName»(self):
			«innerContent»
	'''

	private static def dispatch CharSequence getInnerContent(Job it)
	'''
		«instruction.innerContent»
	'''

	private static def dispatch CharSequence getInnerContent(ExecuteTimeLoopJob it)
	'''
		«val itVar = iterationCounter.codeName»
		«val irRoot = IrUtils.getContainerOfType(it, IrRoot)»
		«val tn = irRoot.currentTimeVariable.codeName»
		«val deltat = irRoot.timeStepVariable.codeName»
		«val ppInfo = irRoot.postProcessing»
		self.«itVar» = 0
		«IF irRoot.currentTimeVariable.needDefinition»self.«tn» = 0.0«ENDIF»
		«IF irRoot.timeStepVariable.needDefinition»self.«deltat» = 0.0«ENDIF»
		continueLoop = True
		while continueLoop:
			self.«itVar» += 1
			«IF caller.main»
				print("START ITERATION «iterationCounter.name»: %5d - t: %5.5f - deltat: %5.5f\n" % (self.«itVar», self.«tn», self.«deltat»))
				«IF ppInfo !== null»
					if (self.«ppInfo.periodReference.codeName» >= self.«ppInfo.lastDumpVariable.codeName» + self.«ppInfo.periodValue.codeName»):
						self.__dumpVariables(self.«itVar»)
				«ENDIF»
			«ELSE»
				print("Start iteration «iterationCounter.name»: %5d\n" % (self.«itVar»))
			«ENDIF»

			«FOR j : calls»
				self._«j.callName»() # @«j.at»
			«ENDFOR»

			# Evaluate loop condition with variables at time n
			continueLoop = («whileCondition.content»)

			«instruction.innerContent»
		«IF caller.main»

			print("FINAL TIME: %5.5f - deltat: %5.5f\n" % (self.«tn», self.«deltat»))
			«IF ppInfo !== null»self.__dumpVariables(self.«itVar»+1);«ENDIF»
		«ENDIF»
	'''

	private static def needDefinition(Variable v)
	{
		!v.option && v.defaultValue === null
	}
}