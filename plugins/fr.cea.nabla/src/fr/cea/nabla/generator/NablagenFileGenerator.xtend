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

import fr.cea.nabla.generator.NablaGeneratorMessageDispatcher.MessageType
import fr.cea.nabla.nabla.NablaExtension
import fr.cea.nabla.nabla.NablaModule
import fr.cea.nabla.nabla.NablaRoot
import fr.cea.nabla.nablagen.TargetType

class NablagenFileGenerator extends StandaloneGeneratorBase
{
	def generate(NablaRoot moduleOrExtension, String genDir, String projectName)
	{
		val fsa = getConfiguredFileSystemAccess(genDir, false)
		var fileName = moduleOrExtension.name + ".ngen"
		// generated only once
		if (fsa.isFile(fileName))
		{
			dispatcher.post(MessageType::Exec, "    File already exists, no overwite: " + fileName)
		}
		else
		{
			dispatcher.post(MessageType::Exec, "    Generating: " + fileName)
			switch moduleOrExtension
			{
				NablaModule: fsa.generateFile(fileName, getApplicationContent(moduleOrExtension.name, projectName))
				NablaExtension: fsa.generateFile(fileName, getProviderContent(moduleOrExtension.name, projectName))
			}
		}
	}

	static def getProviderContent(String nablaExtensionName, String projectName)
	'''
	/*
	 * This file contains the providers for the «nablaExtensionName» NabLab extension.
	 * The list is ordered: the first Provider is the default one for the specified target.
	 * For example, if you entered two «TargetType::DACE» Provider instances,
	 * the first one in the following list will be the default one during NabLab
	 * code generation for the target «TargetType::DACE».
	 */

	/* 
	 * Python Extension Provider
	 */
	Provider «nablaExtensionName»Python : «nablaExtensionName»
	{
		target = «TargetType::PYTHON.literal»;
		// compatibleTargets can be added here
		outputPath = "/«projectName»/src-python";
	}
	
	/* 
	 * DaCe Extension Provider
	 */
	Provider «nablaExtensionName»Dace : «nablaExtensionName»
	{
		target = «TargetType::DACE.literal»;
		// compatibleTargets can be added here
		outputPath = "/«projectName»/src-dace";
	}
	'''

	static def getApplicationContent(String nablaModuleName, String projectName)
	'''
		Application «nablaModuleName»;

		MainModule «nablaModuleName» «nablaModuleName.toFirstLower»
		{
			nodeCoord = X;
			time = t;
			timeStep = delta_t;
			iterationMax = maxIter;
			timeMax = maxTime;
		}

		VtkOutput
		{
			periodReferenceVariable = «nablaModuleName.toFirstLower».n;
			outputVariables = «nablaModuleName.toFirstLower».e as "Energy";
		}

		Python
		{
			outputPath = "/«projectName»/src-gen-python";
		}
		
		Dace
		{
			outputPath = "/«projectName»/src-gen-dace";
		}
	'''
}