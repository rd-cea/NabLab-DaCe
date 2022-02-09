/*******************************************************************************
 * Copyright (c) 2022 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.ir.generator.arcane

import fr.cea.nabla.ir.IrTypeExtensions
import fr.cea.nabla.ir.ir.BaseType
import fr.cea.nabla.ir.ir.Cardinality
import fr.cea.nabla.ir.ir.Connectivity
import fr.cea.nabla.ir.ir.ConnectivityType
import fr.cea.nabla.ir.ir.Expression
import fr.cea.nabla.ir.ir.IrType
import fr.cea.nabla.ir.ir.ItemIndex
import fr.cea.nabla.ir.ir.LinearAlgebraType
import fr.cea.nabla.ir.ir.PrimitiveType
import java.util.ArrayList

import static extension fr.cea.nabla.ir.ContainerExtensions.*
import static extension fr.cea.nabla.ir.generator.arcane.ExpressionContentProvider.*

class TypeContentProvider
{
	static def getTypeName(IrType it)
	{
		switch it
		{
			BaseType:
			{
				val t = typeNameAndDimension
				val dimension = t.value
				switch dimension
				{
					case 0: t.key
					case 1: "UniqueArray<" + t.key + ">"
					case 2: "UniqueArray2<" + t.key + ">"
					default: throw new RuntimeException("Unsupported dimension: " + t.value)
				}
			}
			ConnectivityType: getVariableTypeName(it)
			LinearAlgebraType: getLinearAlgebraClass(it)
			default: throw new RuntimeException("Unexpected type: " + class.name)
		}
	}

	static def isArcaneStlVector(IrType t)
	{
		if (t instanceof LinearAlgebraType)
			t.sizes.size == 1 && t.sizes.head instanceof Cardinality
		else
			false
	}

	static def getLinearAlgebraClass(LinearAlgebraType t)
	{
		switch t.sizes.size
		{
			case 1:
			{
				val s = t.sizes.head
				if (s instanceof Cardinality)
				{
					val itemType = s.container.connectivityCall.connectivity.returnType
					"Arcane2StlVector<" + itemType.name.toFirstUpper + ">"
				}
				else 
					IrTypeExtensions.VectorClass
			}
			case 2: IrTypeExtensions.MatrixClass
			default: throw new RuntimeException("Unexpected dimension: " + t.sizes.size)
		}
	}

	static def getFunctionArgTypeName(IrType it, boolean const)
	{
		if (!IrTypeExtensions.isScalar(it) && it instanceof BaseType && (it as BaseType).primitive == PrimitiveType.REAL)
		{
			val dim = ((it as BaseType).sizes.size)
			switch dim
			{
				case 1: '''RealArrayVariant'''
				case 2: '''RealArray2Variant'''
				default: throw new RuntimeException("Not yet implemented")
			}
		}
		else
			switch it
			{
				case null: null
				BaseType:
				{
					val t = typeNameAndDimension
					val dimension = t.value
					switch dimension
					{
						case 0: '''«IF const»const «ENDIF»«t.key»'''
						case 1: '''«IF const»Const«ENDIF»ArrayView<«t.key»>'''
						default: '''«IF const»Const«ENDIF»Array«dimension»View<«t.key»>'''
					}
				}
				LinearAlgebraType: IrTypeExtensions.getLinearAlgebraClass(it)
				// no ConnectivityType in functions
				default: throw new RuntimeException("Unexpected type: " + class.name)
			}
	}

	static def CharSequence formatIteratorsAndIndices(IrType it, Iterable<ItemIndex> iterators, Iterable<Expression> indices)
	{
		switch it
		{
			case (iterators.empty && indices.empty): ''''''
			BaseType: '''«FOR i : indices»[«i.content»]«ENDFOR»'''
			// for the ArcaneStlVector, the setValue method must keep the ItemEnumerator arg (not the index)
			LinearAlgebraType: '''«FOR i : getConnectivityIndexList(iterators, !TypeContentProvider.isArcaneStlVector(it)) SEPARATOR ', '»«i»«ENDFOR»«FOR i : indices SEPARATOR ', '»«i.content»«ENDFOR»'''
			ConnectivityType: '''«FOR i : getConnectivityIndexList(iterators, false)»[«i»]«ENDFOR»«FOR i : indices»[«i.content»]«ENDFOR»'''
		}
	}

	static def dispatch getVariableTypeName(ConnectivityType it)
	{
		val support = connectivities.head.returnType.name.toFirstUpper
		val t = base.typeNameAndDimension
		val dimension = connectivities.size - 1 + t.value
		return "Variable" + support + getVariableDimensionName(dimension, true) + t.key
	}

	static def dispatch getVariableTypeName(BaseType it)
	{
		val t = typeNameAndDimension
		return "Variable" + getVariableDimensionName(t.value, false) + t.key
	}

	static def dispatch getVariableTypeName(LinearAlgebraType it)
	{
		throw new RuntimeException("Not yet implemented")
	}

	static def Pair<String, Integer> getTypeNameAndDimension(BaseType it)
	{
		if (primitive == PrimitiveType.REAL)
		{
			if (intSizes.size == 1)
			{
				val s = intSizes.get(0)
				if (s == 2) return 'Real2' -> 0
				if (s == 3) return 'Real3' -> 0
			}
			else if (intSizes.size == 2)
			{
				val x = intSizes.get(0)
				val y = intSizes.get(1)
				if (x == 2 && y == 2) return 'Real2x2' -> 0
				if (x == 3 && y == 3) return 'Real3x3' -> 0
			}
		}

		// All the other cases
		val typeName = switch primitive
		{
			case BOOL: 'bool'
			case INT: 'Int32'
			case REAL: 'Real'
		}
		return typeName -> sizes.size
	}

	static def getResizeDims(IrType it)
	{
		switch it
		{
			BaseType: sizes.map[content]
			LinearAlgebraType: sizes.map[content]
			ConnectivityType:
			{
				val t = base.typeNameAndDimension
				val dimension = connectivities.size - 1 + t.value
				// only Array2 must be resized
				if (dimension > 0)
				{
					val dimensions = new ArrayList<CharSequence>
					dimensions += connectivities.tail.map[nbElems]
					dimensions += base.sizes.map[content]
					dimensions.subList(0, dimension)
				}
				else
					#[]
			}
			default: #[]
		}
	}

	private static def getVariableDimensionName(int dimension, boolean hasSupport)
	{
		switch dimension
		{
			case 0: (hasSupport ? "" : "Scalar")
			case 1: "Array"
			case 2: "Array2"
			default: throw new RuntimeException("Unexpected dimension for variable type: " + dimension)
		}
	}

	/* Waiting for more... and management of global matrices */
	private static def getConnectivityIndexList(Iterable<ItemIndex> iterators, boolean fullIndex)
	{
		val indices = new ArrayList<String>
		if (!iterators.empty)
		{
			val firstName = iterators.head.name
			indices += (fullIndex ? firstName + ".index()" : firstName)
			for (iterator : iterators.tail)
			{
				val name = iterator.name
				if (firstName == name)
					indices += name + ".index()"
				else
					indices += name
			}
		}
		return indices
	}

	private static def getNbElems(Connectivity it)
	{
		if (inTypes.empty)
			"nb" + returnType.name.toFirstUpper + "()"
		else
		{
			val varName = "MaxNb" + name.toFirstUpper
			provider.generationVariables.get(varName)
		}
	}
}