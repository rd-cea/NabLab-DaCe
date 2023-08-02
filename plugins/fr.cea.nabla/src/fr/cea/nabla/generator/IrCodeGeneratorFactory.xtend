/*******************************************************************************
 * Copyright (c) 2022 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.generator

import com.google.inject.Inject
import fr.cea.nabla.ir.generator.IrCodeGenerator
import fr.cea.nabla.ir.generator.arcane.ArcaneGenerator
import fr.cea.nabla.ir.generator.cpp.CppGenerator
import fr.cea.nabla.ir.generator.dace.DaceGenerator
import fr.cea.nabla.ir.generator.java.JavaGenerator
import fr.cea.nabla.ir.generator.python.PythonGenerator
import fr.cea.nabla.nablagen.LevelDB
import fr.cea.nabla.nablagen.TargetType
import fr.cea.nabla.nablagen.TargetVar
import java.util.ArrayList
import java.util.Optional

class IrCodeGeneratorFactory
{
	def IrCodeGenerator create(String wsPath, TargetType targetType)
	{
		create(wsPath, targetType, #[], null, null, null)
	}

	def IrCodeGenerator create(String wsPath, TargetType targetType, Iterable<TargetVar> targetVars, LevelDB levelDB, String iterationMaxVarName, String timeMaxVarName)
	{
		val hasLevelDB = (levelDB !== null)
		val envVars = new ArrayList<Pair<String, String>>
		val debug = Optional.ofNullable(targetVars.findFirst[x | x.key.equals("DEBUG")]).map[x | Boolean.parseBoolean(x.value)].orElse(false)
		targetVars.filter[x | !x.key.equals("DEBUG")].forEach[x | envVars += x.key -> x.value]
		switch targetType
		{
			case DACE: new DaceGenerator(wsPath, hasLevelDB, envVars)
			case PYTHON: new PythonGenerator(wsPath, hasLevelDB, envVars)
			default:
			{
				throw new RuntimeException("Unknown target type in IrCodeGeneratorFactory")
			}
		}
	}
}
