/*******************************************************************************
 * Copyright (c) 2022 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.ir.transformers

import fr.cea.nabla.ir.ir.ArgOrVar
import fr.cea.nabla.ir.ir.ArgOrVarRef
import fr.cea.nabla.ir.ir.BaseType
import fr.cea.nabla.ir.ir.IrFactory
import fr.cea.nabla.ir.ir.PrimitiveType
import org.eclipse.emf.ecore.util.EcoreUtil

class OperatorUtils
{
	public static val OperatorPrefix = "operator"

	enum BinOpType { ArrayArray, ArrayScalar, ScalarArray }

	def create IrFactory::eINSTANCE.createReturn createReturnForUnaryOp(ArgOrVarRef a, String op)
	{
		expression = IrFactory::eINSTANCE.createUnaryExpression =>
		[
			type = createScalarBaseType((a.type as BaseType).primitive)
			constExpr = true
			expression = a
			operator = op
		]
	}

	def create IrFactory::eINSTANCE.createReturn createReturnForBinaryOp(ArgOrVarRef a, ArgOrVarRef b, BinOpType binOpType, String op)
	{
		expression = IrFactory::eINSTANCE.createBinaryExpression =>
		[
			type = createScalarBaseType((a.type as BaseType).primitive)
			constExpr = true
			left = a
			right = b
			operator = op
		]
	}

	def createArgOrVarRef(ArgOrVar v)
	{
		IrFactory::eINSTANCE.createArgOrVarRef =>
		[
			target = v
			type = EcoreUtil::copy(v.type)
			constExpr = true
		]
	}

	def createScalarBaseType(PrimitiveType t)
	{
		IrFactory::eINSTANCE.createBaseType =>
		[
			primitive = t
			isStatic = true
		]
	}

	def getBinOpType(BaseType leftType, BaseType rightType)
	{
		if (leftType.sizes.empty) BinOpType::ScalarArray
		else if (rightType.sizes.empty) BinOpType::ArrayScalar
		else BinOpType::ArrayArray
	}
}
