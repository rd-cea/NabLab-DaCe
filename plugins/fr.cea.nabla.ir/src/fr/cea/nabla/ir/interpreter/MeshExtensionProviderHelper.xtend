/*******************************************************************************
 * Copyright (c) 2023 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.ir.interpreter

import fr.cea.nabla.ir.ir.Connectivity
import fr.cea.nabla.ir.ir.ConnectivityCall
import fr.cea.nabla.ir.ir.IrModule
import fr.cea.nabla.ir.ir.Iterator
import fr.cea.nabla.javalib.mesh.CartesianMesh2D
import java.lang.reflect.Method
import java.net.URLClassLoader
import java.util.HashMap
import java.util.logging.Logger

class MeshExtensionProviderHelper implements ExtensionProviderHelper
{
	protected URLClassLoader cl
	val Class<?> providerClass = typeof(CartesianMesh2D)
	static CartesianMesh2D providerInstance = null // singleton

	Method groupMethod
	val methods = new HashMap<Connectivity, Method>
	val groupSizes = new HashMap<String, Integer>
	val sizes = new HashMap<Connectivity, Integer>

	def getNodes() { providerInstance.geometry.nodes }
	def getQuads() { providerInstance.geometry.quads }

	override createProviderInstance(IrModule module)
	{
		if (providerInstance === null)
			providerInstance = providerClass.constructor.newInstance as CartesianMesh2D
	}

	override getProviderClass(String functionName)
	{
		providerClass
	}

	override getProviderInstance(IrModule module)
	{
		providerInstance
	}

	def int getIndexOf(Iterator iterator, int id)
	{
		throw new RuntimeException("Not yet implemented")
	}

	def int[] getElements(String groupName)
	{
		return groupMethod.invoke(providerInstance, groupName) as int[]
	}

	def int[] getElements(Connectivity connectivity, int[] args)
	{
		val method = methods.get(connectivity)
		call(method, args) as int[]
	}

	def int getSingleton(String groupName)
	{
		return groupMethod.invoke(providerInstance, groupName) as Integer
	}

	def int getSingleton(Connectivity connectivity, int[] args)
	{
		val method = methods.get(connectivity)
		call(method, args) as Integer
	}

	def void init(Iterable<Connectivity> connectivities)
	{
		groupMethod = providerClass.getDeclaredMethod("getGroup", #[typeof(String)])
		groupMethod.setAccessible(true)

		// all arguments are of type int
		for (c : connectivities)
		{
			val methodName = "get" + c.name.toFirstUpper
			val nbArgs = c.inTypes.size
			methods.put(c, getMeshMethod(methodName, nbArgs))
		}
	}

	def int getSize(ConnectivityCall c)
	{
		if (c.group === null)
			getSize(c.connectivity)
		else
		{
			var size = groupSizes.get(c.group)
			if (size === null)
				groupSizes.put(c.group, getElements(c.group).size)
			return size
		}
	}

	def int getSize(Connectivity c)
	{
		var size = sizes.get(c)
		if (size === null)
		{
			size = (c.inTypes.empty ? getNbElems(c) : getMaxNbElems(c))
			sizes.put(c, size)
		}
		return size
	}

	package def logSizes(Logger logger, Context.Level level)
	{
		sizes.keySet.forEach[k | logger.log(level, "	" + k.name + " de taille " + sizes.get(k))]
	}

	private def int getNbElems(Connectivity connectivity)
	{
		val methodName = "getNb" + connectivity.name.toFirstUpper
		val method = getMeshMethod(methodName, 0)
		method.invoke(providerInstance, null) as Integer
	}

	private def int getMaxNbElems(Connectivity connectivity)
	{
		val varName = "MaxNb" + connectivity.name.toFirstUpper
		val varValue = connectivity.provider.generationVariables.get(varName)
		if (varValue === null) throw new RuntimeException("Unsupported mesh: " + connectivity.provider.extensionName)
		return Integer.parseInt(varValue)
	}

	private def Method getMeshMethod(String methodName, int nbArgs)
	{
		// all arguments are of type int
		val Class<?>[] parameterTypes = newArrayOfSize(nbArgs)
		for (i : 0..<parameterTypes.size) parameterTypes.set(i, typeof(int))
		val method = providerClass.getDeclaredMethod(methodName, parameterTypes)
		method.setAccessible(true)
		return method
	}

	private def Object call(Method method, int[] args)
	{
		val Object[] parameterValues = args.map[x | Integer.valueOf(x)]
		return method.invoke(providerInstance, parameterValues)
	}
}