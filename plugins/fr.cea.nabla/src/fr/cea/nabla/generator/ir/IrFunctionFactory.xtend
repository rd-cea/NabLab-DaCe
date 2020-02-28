/*******************************************************************************
 * Copyright (c) 2020 CEA
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.generator.ir

import com.google.inject.Inject
import com.google.inject.Singleton
import fr.cea.nabla.ir.ir.IrFactory
import fr.cea.nabla.nabla.Function
import fr.cea.nabla.nabla.FunctionOrReduction
import fr.cea.nabla.nabla.NablaModule
import fr.cea.nabla.nabla.Reduction
import org.eclipse.xtext.EcoreUtil2

/**
 * Attention : cette classe doit être un singleton car elle utilise des méthodes create.
 * Si elle n'est pas singleton, plusieurs instances d'un même objet seront créées lors
 * deu parcours du graphe d'origine (voir la documentation Xtext).
 */
@Singleton
class IrFunctionFactory 
{
	@Inject extension IrAnnotationHelper
	@Inject extension BaseType2IrType
	@Inject extension IrArgOrVarFactory
	@Inject extension IrSizeTypeFactory
	@Inject extension IrInstructionFactory

	def dispatch create IrFactory::eINSTANCE.createFunction toIrFunction(Function f)
	{
		annotations += f.toIrAnnotation
		name = f.name
		provider = f.moduleName
		f.vars.forEach[x | variables += x.toIrSizeTypeSymbol]
		inArgs += f.inArgs.map[x | toIrArg(x, x.name)]
		returnType = f.returnType.toIrBaseType
		if (!f.external) body = f.body.toIrInstruction
	}

	def dispatch create IrFactory::eINSTANCE.createFunction toIrFunction(Reduction f)
	{
		annotations += f.toIrAnnotation
		// build a unique name with name and type
		name = f.name.toFirstLower + f.type.primitive.getName().charAt(0) + f.type.sizes.size
		provider = f.moduleName
		f.vars.forEach[x | variables += x.toIrSizeTypeSymbol]
		inArgs += f.inArgs.map[x | toIrArg(x, x.name)]
		returnType = f.type.toIrBaseType
		body = f.body.toIrInstruction
	}

	private def getModuleName(FunctionOrReduction it) 
	{
		val module = EcoreUtil2.getContainerOfType(it, NablaModule)
		return module.name
	}
}