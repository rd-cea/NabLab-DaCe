/*******************************************************************************
 * Copyright (c) 2018 CEA
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 * 	Benoit Lelandais - initial implementation
 * 	Marie-Pierre Oudot - initial implementation
 * 	Jean-Sylvain Camier - Nabla generation support
 *******************************************************************************/
package fr.cea.nabla.ir.transformers

import fr.cea.nabla.ir.ir.BasicType
import fr.cea.nabla.ir.ir.Expression
import fr.cea.nabla.ir.ir.IrFactory
import fr.cea.nabla.ir.ir.IrModule
import fr.cea.nabla.ir.ir.Reduction
import fr.cea.nabla.ir.ir.ReductionInstruction
import org.eclipse.emf.ecore.util.EcoreUtil

import static fr.cea.nabla.ir.transformers.IrTransformationUtils.*

class ReplaceInternalReductions extends ReplaceReductionsBase implements IrTransformationStep
{
	static val Operators = #{ 'sum'->'+', 'prod'->'*' }
	
	override getDescription() 
	{
		'Replace internal reductions by loops'
	}

	/**
	 * Transforme le module m pour qu'il n'est plus d'instance de ReductionInstruction.
	 * Les réductions sont remplacées par des opérateurs ou des fonctions traditionnelles.
	 * Le choix se fait en fonction de la liste Operators.
	 */
	override transform(IrModule m)
	{
		for (reductionInstr : m.eAllContents.filter(ReductionInstruction).filter[!external].toIterable)
		{
			// création des fonctions correspondantes
			// 2 arguments IN : 1 du type de la collection, l'autre du type de retour (appel en chaine)
			val reduc = reductionInstr.reduction
									
			// transformation de la reduction
			val loopExpression = createAffectationRHS(m, reductionInstr)
			val loop = createReductionLoop(reductionInstr.range, reductionInstr.singletons, reductionInstr.result, loopExpression, '=')
			val variableDefinition = IrFactory::eINSTANCE.createVarDefinition => [ variables += reductionInstr.result ]
			replace(reductionInstr, #[variableDefinition, loop])			

			// si la réduction n'est pas référencée, on l'efface
			if (!m.eAllContents.filter(ReductionInstruction).exists[x | x.reduction == reduc])
				EcoreUtil::delete(reduc, true)
		}
		return true
	}
	
	private def Expression createAffectationRHS(IrModule m, ReductionInstruction reductionInstr)
	{
		val reduction = reductionInstr.reduction
		val varRef = IrFactory::eINSTANCE.createVarRef => 
		[ 
			variable = reductionInstr.result
			type = createExpressionType(variable.type)
		]
		
		if (Operators.keySet.contains(reduction.name))
		{
			return IrFactory::eINSTANCE.createBinaryExpression =>
			[
				type = createExpressionType(reduction.returnType)
				operator = Operators.get(reduction.name)
				left = varRef
				right = IrFactory::eINSTANCE.createParenthesis => 
				[ 
					expression = reductionInstr.arg
					type = EcoreUtil::copy(expression.type)
				]
			]
		}
		else
		{
			// creation de la fonction
			val f = findOrCreateFunction(m, reduction)										
			// transformation de la reduction
			return IrFactory::eINSTANCE.createFunctionCall =>
			[
				type = createExpressionType(f.returnType)
				function = f
				args += varRef
				args += reductionInstr.arg
			] 
		}
	}
	
	private def findOrCreateFunction(IrModule m, Reduction r)
	{
		var function = m.functions.findFirst
		[   
			name == r.name && 
			inTypes.length == 2 && 
			inTypes.get(0) == r.collectionType && 
			inTypes.get(1) == r.returnType && 
			returnType == r.returnType
		]
		
		if (function === null) 
		{ 
			function = IrFactory::eINSTANCE.createFunction =>
			[
				name = r.name
				inTypes += r.collectionType
				inTypes += r.returnType
				returnType = r.returnType
				provider = r.provider
			]
			m.functions += function
		}
		
		return function
	}
	
	private def createExpressionType(BasicType t)
	{
		IrFactory::eINSTANCE.createExpressionType => 
		[
			basicType = t
			dimension = 0
		]
	}
}