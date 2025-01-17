"""
 * Copyright (c) 2023 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
"""
import numpy as np
import dace
import sys

from meshgeometry import MeshGeometry

class CartesianMesh2D:
    def jsonInit(self, jsonContent):
        self.create(jsonContent["nbXQuads"], jsonContent["nbYQuads"], jsonContent["xSize"], jsonContent["ySize"])
    
    @property
    def geometry(self):
        return self._geometry
    
    @property
    def nbNodes(self):
        return len(self._geometry.nodes)
    @property
    def nbCells(self):
        return len(self._geometry.quads)
    @property
    def nbFaces(self):
        return len(self._geometry.edges)
    
    @property
    def nodes(self):
        return self.AllNodes
    @property
    def cells(self):
        return self.AllCells
    @property
    def faces(self):
        return self.AllFaces

    @dace.method
    def getNodesOfCell(self, cellId):
        return self._geometry.quads[cellId]
    def getNodesOfFace(self, faceId):
        return self._geometry.edges[faceId]
    def getFirstNodeOfFace(self, faceId):
        return self.getNodesOfFace(faceId)[0]
    def getSecondNodeOfFace(self, faceId):
        return self.getNodesOfFace(faceId)[1]
    
    @dace.method
    def getCellsOfNode(self, nodeId):
        i, j = self._id2IndexNode(nodeId)
        cellsOfNode = dace.ndarray(shape=(4), dtype=np.int32) # At most four cells per node
        cellsOfNode.fill((1 << 31) - 1)
        c = dace.ndarray(shape=(2), dtype=np.int32)
        c.fill(0)

        if i[0] < self._nbYQuads and j[0] < self._nbXQuads:
            cellsOfNode[c[0]] = self._index2IdCell(i[0], j[0])
            c[0] = c[0] + 1
        if i[0] < self._nbYQuads and j[0] > 0:
            cellsOfNode[c[0]] = self._index2IdCell(i[0], j[0]-1)
            c[0] = c[0] + 1
        if i[0] > 0 and j[0] < self._nbXQuads:
            cellsOfNode[c[0]] = self._index2IdCell(i[0]-1, j[0])
            c[0] = c[0] + 1
        if i[0] > 0 and j[0] > 0:
            cellsOfNode[c[0]] = self._index2IdCell(i[0]-1, j[0]-1)
            c[0] = c[0] + 1
        np.ndarray.sort(cellsOfNode) # DaCe callback
        return cellsOfNode

    def getCellsOfFace(self, faceId):
        i_f = faceId // ((2 * self._nbXQuads) + 1)
        k_f = faceId - (i_f * ((2 * self._nbXQuads) + 1))
        cellsOfFace = []
        if i_f < self._nbYQuads: # all except upper bound faces
            if k_f == (2 * self._nbXQuads):
                cellsOfFace.append(self._index2IdCell(i_f, self._nbXQuads - 1))
            else:
                if k_f == 1: # left bound edge
                    cellsOfFace.append(self._index2IdCell(i_f, 0))
                else:
                    if (k_f % 2) == 0: # horizontal edge
                        cellsOfFace.append(self._index2IdCell(i_f, k_f // 2))
                        if (i_f > 0): # Not bottom bound edge
                            cellsOfFace.append(self._index2IdCell(i_f - 1, k_f // 2))
                    else: # vertical edge (neither left bound nor right bound)
                        cellsOfFace.append(self._index2IdCell(i_f, (((k_f - 1) // 2) - 1)))
                        cellsOfFace.append(self._index2IdCell(i_f, ((k_f - 1) // 2)))
        else: # upper bound faces
            cellsOfFace.append(self._index2IdCell(i_f - 1, k_f))
        return np.array(cellsOfFace)
    
    def getNeighbourCells(self, cellId):
        x, y = self._id2IndexCell(cellId)
        neighbourCells = np.full((4,), -1)
        cpt = dace.ndarray(shape=(2), dtype=dace.int32)

        if x[0] >= 1:
            neighbourCells[cpt[0]] = self._index2IdCell(x-1, y)
            cpt[0] = cpt[0] + 1
        if x[0] < self._nbYQuads - 1:
            neighbourCells[cpt[0]] = self._index2IdCell(x+1, y)
            cpt[0] = cpt[0] + 1
        if y[0] >= 1:
            neighbourCells[cpt[0]] = self._index2IdCell(x, y-1)
            cpt[0] = cpt[0] + 1
        if y[0] < self._nbXQuads-1:
            neighbourCells[cpt[0]] = self._index2IdCell(x, y+1)
            cpt[0] = cpt[0] + 1
        neighbourCells = np.sort(neighbourCells)
        return neighbourCells
    
    def getFacesOfCell(self, cellId):
        x, y = self._id2IndexCell(cellId)
        facesOfCell = dace.ndarray(4, dtype=dace.int32)
        facesOfCell.fill(0)
        facesOfCell[0] = (2 * y[0] + x[0] * (2 * self._nbXQuads + 1))
        facesOfCell[1] = facesOfCell[0] + 1
        
        if y[0] == self._nbXQuads:
            facesOfCell[2] = facesOfCell[0] + 2
        else:
            facesOfCell[2] = facesOfCell[0] + 3
        if x[0] <  self._nbYQuads - 1:
            facesOfCell[3] = facesOfCell[0] + 2 *  self._nbXQuads + 1
        else:
            facesOfCell[3] = facesOfCell[0] + 2 *  self._nbXQuads + 1 - y[0]
        return facesOfCell
    
    def getCommonFace(self, cell1, cell2):
        cell1Faces = self.getFacesOfCell(cell1);
        cell2Faces = self.getFacesOfCell(cell2);
        commonFace = np.ndarray([2], dace.int64)
        commonFace = np.intersect1d(cell1Faces, cell2Faces)
        if commonFace.size > 0:
            return commonFace[0]
        else:
            return -1
        
    def getBackCell(self, faceId):
        cells = self.getCellsOfFace(faceId);
        if len(cells) < 2:
            raise Exception("Error in getBackCell(" + str(faceId) + "): please consider using this method with inner face only.");
        else:
            return cells[0]
    
    def getFrontCell(self, faceId):
        cells = self.getCellsOfFace(faceId);
        if len(cells) < 2:
            raise Exception("Error in getBackCell(" + str(faceId) + "): please consider using this method with inner face only.");
        else:
            return cells[1]
    
    def getTopFaceOfCell(self, cellId):
        i, j = self._id2IndexCell(cellId)
        bottomFace = 2 * j + i * (2 * self._nbXQuads + 1)
        topFace = bottomFace + (2 * self._nbXQuads + 1 if i < self._nbYQuads - 1 else 2 * self._nbXQuads + 1 - j)
        return topFace
    
    def getBottomFaceOfCell(self, cellId):
        i, j = self._id2IndexCell(cellId)
        bottomFace = 2 * j + i * (2 * self._nbXQuads + 1)
        return bottomFace
    
    def getLeftFaceOfCell(self, cellId):
        bottomFace = self.getBottomFaceOfCell(cellId)
        leftFace = bottomFace + 1
        return leftFace
    
    def getRightFaceOfCell(self, cellId):
        i, j = self._id2IndexCell(cellId)
        bottomFace = 2 * j + i * (2 * self._nbXQuads + 1);
        rightFace = bottomFace + (2 if j == self._nbXQuads - 1 else 3)
        return rightFace

    def getTopCell(self, cellId):
        i, j = self._id2IndexCell(cellId)
        if (i == self._nbYQuads - 1):
            return cellId
        return self._index2IdCell(i+1, j)
    
    def getBottomCell(self, cellId):
        i, j = self._id2IndexCell(cellId)
        if (i == 0):
            return cellId
        return self._index2IdCell(i-1, j)
    
    def getLeftCell(self, cellId):
        i, j = self._id2IndexCell(cellId)
        if (j == 0):
            return cellId
        return self._index2IdCell(i, j-1)
    
    def getRightCell(self, cellId):
        i, j = self._id2IndexCell(cellId)
        if (j == self._nbXQuads-1):
            return cellId
        return self._index2IdCell(i, j+1)
    
    def getBottomFaceNeighbour(self, faceId):
        return (faceId - (2 * self._nbXQuads + 1))
    
    def getBottomLeftFaceNeighbour(self, faceId):
        edges = self._geometry.edges
        if(self._isVerticalEdge(edges[faceId])):
            return faceId - 3
        else:
            return (faceId + 1) - (2 * self._nbXQuads + 1)
    
    def getBottomRightFaceNeighbour(self, faceId):
        edges = self._geometry.edges
        if (self._isVerticalEdge(edges[faceId])):
            return faceId - 1
        else: # horizontal
            return (faceId + 3) - (2 * self._nbXQuads + 1)
    
    def getTopFaceNeighbour(self, faceId):
        return (faceId + (2 * self._nbXQuads + 1))
    
    def getTopLeftFaceNeighbour(self, faceId):
        edges = self._geometry.edges
        if (self._isVerticalEdge(edges[faceId])):
            return (faceId - 3) + (2 * self._nbXQuads + 1)
        else: # horizontal
            return faceId + 1
    
    def getTopRightFaceNeighbour(self, faceId):
        edges = self._geometry.edges
        if (self._isVerticalEdge(edges[faceId])):
            return (faceId - 1) + (2 * self._nbXQuads + 1)
        else: # horizontal
            return faceId + 3
        
    def getRightFaceNeighbour(self, faceId):
        return faceId + 2
    
    def getLeftFaceNeighbour(self, faceId):
        return faceId - 2
    
    def dump(self):
        self._geometry.dump()
        print("MESH TOPOLOGY")
        innerNodes = self.InnerNodes
        topNodes = self.TopNodes
        bottomNodes = self.BottomNodes
        leftNodes = self.LeftNodes
        rightNodes = self.RightNodes
        outerFaces = self.OuterFaces
        innerFaces = self.InnerFaces
    
    def create(self, nbXQuads, nbYQuads, xSize, ySize):
        if (nbXQuads == -1 or nbYQuads == -1 or xSize == -1 or ySize == -1):
            raise Exception("Mesh attributes uninitialized")
        
        self._nbXQuads = nbXQuads
        self._nbYQuads = nbYQuads
        
        nodes = np.empty(((nbXQuads + 1) * (nbYQuads + 1), 2), dtype=np.double)
        quads = np.empty((nbXQuads * nbYQuads, 4), dtype=np.int32)
        edges = np.empty((2 * len(quads) + nbXQuads + nbYQuads, 2), dtype=np.int32)

        outerNodes = np.empty(2 * (nbXQuads + nbYQuads), dtype=np.int32)
        innerNodes = np.empty(len(nodes) - len(outerNodes), dtype=np.int32)
        topNodes = np.empty(nbXQuads + 1, dtype=np.int32)
        bottomNodes = np.empty(nbXQuads + 1, dtype=np.int32)
        leftNodes = np.empty(nbYQuads + 1, dtype=np.int32)
        rightNodes = np.empty(nbYQuads + 1, dtype=np.int32)

        nodeId = 0
        outerNodeId = 0
        innerNodeId = 0
        topNodeId = 0
        bottomNodeId = 0
        leftNodeId = 0
        rightNodeId = 0
        
        # node creation
        for j in range(nbYQuads + 1):
            for i in range(nbXQuads + 1):
                nodes[nodeId] = [xSize * i, ySize * j]
                if i!=0 and j!=0 and i!=nbXQuads and j!=nbYQuads:
                    innerNodes[innerNodeId] = nodeId
                    innerNodeId +=1
                else:
                    outerNodes[outerNodeId] = nodeId
                    outerNodeId += 1
                    if j==0:
                        bottomNodes[bottomNodeId] = nodeId
                        bottomNodeId += 1
                    if j==nbYQuads:
                        topNodes[topNodeId] = nodeId
                        topNodeId += 1
                    if i==0:
                        leftNodes[leftNodeId] = nodeId
                        leftNodeId += 1
                    if i==nbXQuads:
                        rightNodes[rightNodeId] = nodeId
                        rightNodeId += 1
                nodeId += 1
        
        # edge creation
        nbXNodes = nbXQuads+1
        edgeId = 0
        for i in range(len(nodes)):
            rightNodeIndex = i+1
            if rightNodeIndex%nbXNodes!=0:
                edges[edgeId] = [i, rightNodeIndex]
                edgeId += 1
            belowNodeIndex = i + nbXNodes
            if belowNodeIndex<len(nodes):
                edges[edgeId] = [i, belowNodeIndex]
                edgeId += 1
        
        # quad creation
        innerCells = np.empty((nbXQuads - 2)*(nbYQuads - 2), dtype=np.int32)
        outerCells = np.empty(2 * nbXQuads + 2 * (nbYQuads - 2), dtype=np.int32)
        topCells = np.empty(nbXQuads, dtype=np.int32)
        bottomCells = np.empty(nbXQuads, dtype=np.int32)
        leftCells = np.empty(nbYQuads, dtype=np.int32)
        rightCells = np.empty(nbYQuads, dtype=np.int32)
        
        quadId = 0
        innerCellId = 0
        outerCellId = 0
        topCellId = 0
        bottomCellId = 0
        leftCellId = 0
        rightCellId = 0
        
        for j in range(nbYQuads):
            for i in range(nbXQuads):
                if i != 0 and i != nbXQuads - 1 and j != 0 and j != nbYQuads - 1:
                    innerCells[innerCellId] = quadId
                    innerCellId += 1
                else:
                    outerCells[outerCellId] = quadId
                    outerCellId += 1
                    if j == nbYQuads - 1:
                        topCells[topCellId] = quadId
                        topCellId += 1
                    if j == 0:
                        bottomCells[bottomCellId] = quadId
                        bottomCellId += 1
                    if i == 0:
                        leftCells[leftCellId] = quadId
                        leftCellId += 1
                    if i == nbXQuads - 1:
                        rightCells[rightCellId] = quadId
                        rightCellId += 1
                
                upperLeftNodeIndex = (j*nbXNodes)+i;
                lowerLeftNodeIndex = upperLeftNodeIndex + nbXNodes;
                quads[quadId] = [upperLeftNodeIndex, upperLeftNodeIndex+1, lowerLeftNodeIndex+1, lowerLeftNodeIndex]
                quadId += 1
        
        self._geometry = MeshGeometry(nodes, edges, quads)
        
        # faces partitioning
        outFaces = np.empty(2 * nbXQuads + 2 * nbYQuads, dtype=np.int32)
        inFaces = np.empty(len(edges) - len(outFaces), dtype=np.int32)
        inHFaces = np.empty(nbXQuads * (nbYQuads - 1), dtype=np.int32)
        inVFaces = np.empty(nbYQuads * (nbXQuads - 1), dtype=np.int32)
        tFaces = np.empty(nbXQuads, dtype=np.int32)
        bFaces = np.empty(nbXQuads, dtype=np.int32)
        lFaces = np.empty(nbYQuads, dtype=np.int32)
        rFaces = np.empty(nbYQuads, dtype=np.int32)
        
        inFaceId = 0;
        outFaceId = 0;
        inHFaceId = 0;
        inVFaceId = 0;
        tFaceId = 0;
        bFaceId = 0;
        lFaceId = 0;
        rFaceId = 0;
        
        for edgeId in range(len(edges)):
            # Top boundary faces
            if edgeId >= 2 * nbXQuads * nbYQuads + nbYQuads:
                tFaces[tFaceId] = edgeId
                tFaceId += 1
            # Bottom boundary faces
            if (edgeId < 2 * nbXQuads) and (edgeId % 2 == 0):
                bFaces[bFaceId] = edgeId
                bFaceId += 1
            # Left boundary faces
            if (edgeId % (2 * nbXQuads + 1) == 1) and (edgeId < (2 * nbXQuads + 1) * nbYQuads):
                lFaces[lFaceId] = edgeId
                lFaceId += 1
            # Right boundary faces
            if edgeId % (2 * nbXQuads + 1) == 2 * nbXQuads:
                rFaces[rFaceId] = edgeId
                rFaceId += 1
            
            edge = edges[edgeId]
            if self._isInnerEdge(edge):
                inFaces[inFaceId] = edgeId
                inFaceId += 1
                if self._isVerticalEdge(edge):
                    inVFaces[inVFaceId]= edgeId
                    inVFaceId += 1
                elif self._isHorizontalEdge(edge):
                    inHFaces[inHFaceId] = edgeId
                    inHFaceId += 1
                else:
                    raise Exception("The inner edge " + str(edgeId) + " should be either vertical or horizontal")
            else:
                outFaces[outFaceId] = edgeId
                outFaceId += 1

        # NODES
        self.AllNodes = np.array(range(self.nbNodes))
        self.InnerNodes = innerNodes
        self.OuterNodes = outerNodes
        self.TopNodes = topNodes
        self.BottomNodes = bottomNodes
        self.LeftNodes = leftNodes
        self.RightNodes = rightNodes
        self.TopLeftNode = np.array([(nbXQuads + 1) * nbYQuads])
        self.TopRightNode = np.array([(nbXQuads + 1) * (nbYQuads +1) - 1])
        self.BottomLeftNode = np.array([0])
        self.BottomRightNode = np.array([nbXQuads])

        # CELLS
        self.AllCells = np.array(range(self.nbCells))
        self.InnerCells = innerCells
        self.OuterCells = outerCells
        self.TopCells = topCells
        self.BottomCells = bottomCells
        self.LeftCells = leftCells
        self.RightCells = rightCells

        # FACES
        self.AllFaces = np.array(range(self.nbFaces))
        self.InnerFaces = np.array(inFaces)
        self.OuterFaces = np.array(outFaces)
        self.InnerHorizontalFaces = np.array(inHFaces)
        self.InnerVerticalFaces = np.array(inVFaces)
        self.TopFaces = np.array(tFaces)
        self.BottomFaces = np.array(bFaces)
        self.LeftFaces = np.array(lFaces)
        self.RightFaces = np.array(rFaces)

    @dace.method
    def getTopNode(self, id):
        return self.TopNodes[id]

    @dace.method
    def getTopNodes(self):
        return self.TopNodes

    def _isInnerEdge(self, edge):
        i1, j1 = self._id2IndexNode(edge[0])
        i2, j2 = self._id2IndexNode(edge[1])
        # If nodes are located on the same boundary, then the face is an outer one
        if (i1 == 0 and i2 == 0) or (i1 == self._nbYQuads and i2 == self._nbYQuads) or (j1 == 0 and j2 == 0) or (j1 == self._nbXQuads and j2 == self._nbXQuads):
            return False
        #else it's an inner one
        return True
    
    def _isVerticalEdge(self, edge):
        return edge[0] == edge[1] + self._nbXQuads + 1 or edge[1] == edge[0] + self._nbXQuads + 1
    
    def _isHorizontalEdge(self, edge):
        return edge[0] == edge[1] + 1 or edge[1] == edge[0] + 1
    
    def _index2IdCell(self, i, j):
        return (i * self._nbXQuads) + j
    
    def _id2IndexCell(self, cellId):
        i = cellId // self._nbXQuads
        j = cellId % self._nbXQuads
        return i, j
    
    def _id2IndexNode(self, nodeId):
        i = nodeId // (self._nbXQuads + 1)
        j = nodeId % (self._nbXQuads + 1)
        return i, j

