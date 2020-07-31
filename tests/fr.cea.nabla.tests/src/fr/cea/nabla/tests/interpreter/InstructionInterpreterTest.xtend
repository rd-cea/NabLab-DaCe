/*******************************************************************************
 * Copyright (c) 2020 CEA
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.tests.interpreter

import com.google.inject.Inject
import fr.cea.nabla.ir.MandatoryVariables
import fr.cea.nabla.ir.interpreter.ModuleInterpreter
import fr.cea.nabla.ir.interpreter.NV0Real
import fr.cea.nabla.ir.interpreter.NV1Real
import fr.cea.nabla.ir.interpreter.NV3Real
import fr.cea.nabla.tests.CompilationChainHelper
import fr.cea.nabla.tests.NablaInjectorProvider
import fr.cea.nabla.tests.TestUtils
import java.util.logging.ConsoleHandler
import java.util.logging.Level
import org.eclipse.xtext.testing.InjectWith
import org.eclipse.xtext.testing.XtextRunner
import org.junit.Assert
import org.junit.runner.RunWith

@RunWith(XtextRunner)
@InjectWith(NablaInjectorProvider)
class InstructionInterpreterTest extends AbstractInstructionInterpreterTest
{
	@Inject CompilationChainHelper compilationHelper
	@Inject extension TestUtils

	override assertInterpreteVarDefinition(String model)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)
		val context = moduleInterpreter.interpreteWithOptionDefaultValues

		assertVariableValueInContext(irModule, context, "t", new NV0Real(1.0))
	}

	override assertInterpreteInstructionBlock(String model)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)
		val context = moduleInterpreter.interpreteWithOptionDefaultValues

		assertVariableValueInContext(irModule, context, "t", new NV0Real(1.0))
	}

	override assertInterpreteAffectation(String model)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)
		val context = moduleInterpreter.interpreteWithOptionDefaultValues

		assertVariableValueInContext(irModule, context, "t", new NV0Real(1.0))
	}

	override assertInterpreteLoop(String model, int xQuads, int yQuads)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)
		val context = moduleInterpreter.interpreteWithOptionDefaultValues

		val nbCells = xQuads * yQuads
		val double[] u = newDoubleArrayOfSize(nbCells)
		for (i : 0..<u.length)
			u.set(i, 1.0)

		assertVariableValueInContext(irModule, context, "U", new NV1Real(u))

		val cjr = (context.getVariableValue("C") as NV3Real).data

		val nbNodesOfCell = 4
		val xEdgeLength = (context.getVariableValue(MandatoryVariables::X_EDGE_LENGTH) as NV0Real).data
		val yEdgeLength = (context.getVariableValue(MandatoryVariables::Y_EDGE_LENGTH) as NV0Real).data
		for (j : 0..<nbCells)
		{
			for (r : 0..<nbNodesOfCell)
			{
				Assert.assertEquals(0.5*(xEdgeLength), Math.abs(cjr.get(j,r).get(0)), TestUtils.DoubleTolerance)
				Assert.assertEquals(0.5*(yEdgeLength), Math.abs(cjr.get(j,r).get(1)), TestUtils.DoubleTolerance)
			}
		}

		// Test reduction
		val bmin = (context.getVariableValue("Bmin") as NV0Real).data
		Assert.assertEquals(-0.00390625, bmin, TestUtils.DoubleTolerance)
	}

	override assertInterpreteIf(String model, int xQuads, int yQuads)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)
		val context = moduleInterpreter.interpreteWithOptionDefaultValues

		val nbCells = xQuads * yQuads
		val double[] u = newDoubleArrayOfSize(nbCells)
		for (i : 0..<u.length)
			if (i % 2 == 0)
				u.set(i, 0.0)
			else
				u.set(i, 1.0)

		assertVariableValueInContext(irModule, context, "U", new NV1Real(u))
	}

	override assertInterpreteWhile(String model, int xQuads, int yQuads)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)
		val context = moduleInterpreter.interpreteWithOptionDefaultValues

		val nbCells = xQuads * yQuads
		val double[] u = newDoubleArrayOfSize(nbCells)
		for (i : 0..<u.length)
			u.set(i, 2.0)

		assertVariableValueInContext(irModule, context, "U", new NV1Real(u))
	}

	override assertInterpreteSetDefinition(String model, int xQuads, int yQuads)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)
		val context = moduleInterpreter.interpreteWithOptionDefaultValues

		val nbCells = xQuads * yQuads
		val double[] u = newDoubleArrayOfSize(nbCells)
		for (i : 0..<u.length)
			u.set(i, 1.0)

		assertVariableValueInContext(irModule, context, "U", new NV1Real(u))
	}

	override assertInterpreteExit(String model)
	{
		val irModule = compilationHelper.getIrModuleForInterpretation(model, testGenModel)
		val handler = new ConsoleHandler
		handler.level = Level::OFF
		val moduleInterpreter = new ModuleInterpreter(irModule, handler)

		try {
			moduleInterpreter.interpreteWithOptionDefaultValues
			Assert::fail("Should throw exception")
		} catch (RuntimeException e) {
			Assert.assertEquals("V must be less than 100", e.message)
		}
	}
}