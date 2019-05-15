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
package fr.cea.nabla.ir.generator.kokkos

import com.google.inject.Inject
import fr.cea.nabla.ir.generator.IteratorRefExtensions
import fr.cea.nabla.ir.ir.ArrayVariable
import fr.cea.nabla.ir.ir.BinaryExpression
import fr.cea.nabla.ir.ir.BoolConstant
import fr.cea.nabla.ir.ir.ContractedIf
import fr.cea.nabla.ir.ir.FunctionCall
import fr.cea.nabla.ir.ir.IntConstant
import fr.cea.nabla.ir.ir.Job
import fr.cea.nabla.ir.ir.MaxConstant
import fr.cea.nabla.ir.ir.MinConstant
import fr.cea.nabla.ir.ir.Parenthesis
import fr.cea.nabla.ir.ir.Real2Constant
import fr.cea.nabla.ir.ir.Real2x2Constant
import fr.cea.nabla.ir.ir.Real3Constant
import fr.cea.nabla.ir.ir.Real3x3Constant
import fr.cea.nabla.ir.ir.RealConstant
import fr.cea.nabla.ir.ir.ScalarVariable
import fr.cea.nabla.ir.ir.UnaryExpression
import fr.cea.nabla.ir.ir.VarRef
import fr.cea.nabla.ir.ir.Variable
import java.util.ArrayList
import org.eclipse.emf.ecore.EObject

import static extension fr.cea.nabla.ir.JobExtensions.*
import static extension fr.cea.nabla.ir.VariableExtensions.isScalarConst

class ExpressionContentProvider
{
	@Inject extension IteratorRefExtensions
	
	def dispatch CharSequence getContent(ContractedIf it) 
	'''(«condition.content» ? «thenExpression.content» ':' «elseExpression.content»'''

	def dispatch CharSequence getContent(BinaryExpression it) '''«left.content» «operator» «right.content»'''
	def dispatch CharSequence getContent(UnaryExpression it) '''«operator»«expression.content»'''
	def dispatch CharSequence getContent(Parenthesis it) '''(«expression.content»)'''
	def dispatch CharSequence getContent(IntConstant it) '''«value»'''
	def dispatch CharSequence getContent(RealConstant it) '''«value»'''
	def dispatch CharSequence getContent(Real2Constant it) '''Real2(«x», «y»)'''
	def dispatch CharSequence getContent(Real3Constant it) '''Real3(«x», «y», «z»)'''
	def dispatch CharSequence getContent(Real2x2Constant it) '''Real2x2(«x.content», «y.content»)'''
	def dispatch CharSequence getContent(Real3x3Constant it) '''Real3x3(«x.content», «y.content», «z.content»)'''
	def dispatch CharSequence getContent(BoolConstant it) '''«value»'''
	
	def dispatch CharSequence getContent(MinConstant it) 
	{
		switch getType().basicType
		{
			case INT  : '''numeric_limits<int>::min()'''
			case REAL : '''numeric_limits<double>::min()'''
			case REAL2, case REAL2X2, case REAL3, case REAL3X3: '''«getType().basicType»(numeric_limits<double>::min())'''
			default: throw new Exception('Invalid expression Min for type: ' + getType().basicType)
		}
	}

	def dispatch CharSequence getContent(MaxConstant it) 
	{
		switch getType().basicType
		{
			case INT  : '''numeric_limits<int>::max()'''
			case REAL : '''numeric_limits<double>::max()'''
			case REAL2, case REAL2X2, case REAL3, case REAL3X3: '''«getType().basicType»(numeric_limits<double>::max())'''
			default: throw new Exception('Invalid expression Max for type: ' + getType().basicType)
		}
	}

	def dispatch CharSequence getContent(FunctionCall it) 
	'''«function.provider»Functions::«function.name»(«FOR a:args SEPARATOR ', '»«a.content»«ENDFOR»)'''
	
	def dispatch CharSequence getContent(VarRef it) 
	'''«codeName»«iteratorsContent»«FOR f:fields BEFORE '.' SEPARATOR '.'»«f»«ENDFOR»'''

	private def getCodeName(VarRef it)
	{
		if (constRef) '''as_const(«variable.codeName»)'''
		else '''«variable.codeName»'''
	}
	
	private def isConstRef(VarRef it)
	{
		val j = getJob(it)
		val scalar = variable instanceof ScalarVariable
		if (j === null) scalar
		else scalar && j.inVars.exists[x | x == variable]
	}
	
	private def getCodeName(Variable it)
	{
		if (scalarConst) 'options->' + name
		else name
	}

	private def Job getJob(EObject o)
	{
		if (o === null) null
		else if (o instanceof Job) o as Job
		else o.eContainer.job
	}

	private def getIteratorsContent(VarRef it) 
	{ 
		if (iterators.empty || variable instanceof ScalarVariable) return ''
		val array = variable as ArrayVariable
		if (array.dimensions.size < iterators.size) return ''
		var content = new ArrayList<CharSequence>
		for (r : iterators)
			content += r.indexName				
		return '(' + content.join(',') + ')'
	}
}