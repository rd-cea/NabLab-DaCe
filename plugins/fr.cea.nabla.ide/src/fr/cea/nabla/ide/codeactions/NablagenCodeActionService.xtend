/*******************************************************************************
 * Copyright (c) 2022 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.ide.codeactions

import fr.cea.nabla.nablagen.MainModule
import fr.cea.nabla.validation.NablagenValidator
import java.util.ArrayList
import javax.inject.Inject
import org.eclipse.emf.common.util.URI
import org.eclipse.lsp4j.CodeAction
import org.eclipse.lsp4j.CodeActionKind
import org.eclipse.lsp4j.Diagnostic
import org.eclipse.lsp4j.Position
import org.eclipse.lsp4j.Range
import org.eclipse.lsp4j.TextEdit
import org.eclipse.lsp4j.WorkspaceEdit
import org.eclipse.lsp4j.jsonrpc.messages.Either
import org.eclipse.xtext.ide.server.Document
import org.eclipse.xtext.ide.server.codeActions.ICodeActionService2
import org.eclipse.xtext.ide.server.codeActions.ICodeActionService2.Options
import org.eclipse.xtext.resource.EObjectAtOffsetHelper
import org.eclipse.xtext.resource.XtextResource

class NablagenCodeActionService implements ICodeActionService2
{

	static val TAB = "\t"
	static val ITERATION_MAX_ATTR = "iterationMax"
	static val TIME_MAX_ATTR = "timeMax"

	@Inject
	EObjectAtOffsetHelper eObjectAtOffsetHelper

	override getCodeActions(Options options)
	{

		val result = <CodeAction>newArrayList
		// No longer needed since C++ backends are disabled
		return result.map[Either.forRight(it)]
	}

	private def void addTextEdit(WorkspaceEdit edit, URI uri, TextEdit... textEdit)
	{
		edit.changes.put(uri.toString, textEdit)
	}

}
