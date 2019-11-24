/*******************************************************************************
 * Copyright (c) 2018 CEA
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
package fr.cea.nabla.javalib.mesh

import java.util.ArrayList
import org.eclipse.xtend.lib.annotations.Accessors

class Mesh
{
	@Accessors val ArrayList<double[]> nodes
	@Accessors val Edge[] edges
	@Accessors val Quad[] quads
	@Accessors val int[] innerNodeIds

	new(int nbNodes, int nbEdges, int nbQuads, int nbInnerNodes)
	{
		nodes = new ArrayList<double[]>(nbNodes)
		edges = newArrayOfSize(nbEdges)
		quads = newArrayOfSize(nbQuads)
		innerNodeIds = newIntArrayOfSize(nbInnerNodes)
	}

	def getQuadIdsOfNode(int nodeId)
	{
		val size = quads.filter[q|q.nodeIds.contains(nodeId)].size
		val candidateQuadIds = newIntArrayOfSize(size)
		var int quadId = 0
		var int candidateQuadId = 0
		for (q : quads)
		{
			if (q.nodeIds.contains(nodeId))
			{
				candidateQuadIds.set(candidateQuadId, quadId)
				candidateQuadId++
			}
			quadId++
		}
		return candidateQuadIds
	}

	def getOuterEdgeIds()
	{
		val size = edges.filter[e|!e.innerEdge].size
		val candidateEdgeIds = newIntArrayOfSize(size)
		var int edgeId = 0
		var int candidateEdgeId = 0
		for (e : edges)
		{
			if (!e.innerEdge)
			{
				candidateEdgeIds.set(candidateEdgeId, edgeId)
				candidateEdgeId++
			}
			edgeId++
		}
		return candidateEdgeIds
	}

	def isInnerEdge(Edge it)
	{
		innerNodeIds.contains(nodeIds.get(0)) || innerNodeIds.contains(nodeIds.get(1))
	}

	def dump()
	{
		println('Mesh ')
		println('  nodes (' + nodes.size + ') : ' + nodes.map[toString].join(','))
		println('  edges : ' + edges.map[toString].join(','))	
		println('  quads : ' + quads.map[toString].join(','))
		println('  outer edges : ' + outerEdgeIds.map[toString].join(','))
	}
}