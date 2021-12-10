/*******************************************************************************
 * Copyright (c) 2021 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.ir.generator.arcane

import fr.cea.nabla.ir.ir.Variable

import static extension fr.cea.nabla.ir.generator.arcane.StringExtensions.*
import static extension fr.cea.nabla.ir.ArgOrVarExtensions.*

class VariableExtensions
{
	static def getCodeName(Variable it)
	{
		if (global)
			'm_' + name.separateWith(StringExtensions.LowerCase)
		else
			name.separateWith(StringExtensions.LowerCase)
	}

	static def getOptionName(Variable it)
	{
		name.separateWith(StringExtensions.Dash)
	}
}