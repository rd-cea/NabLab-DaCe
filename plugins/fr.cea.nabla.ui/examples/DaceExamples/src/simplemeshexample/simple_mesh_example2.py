"""
 * Copyright (c) 2021 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
"""
import sys
import json
import numpy as np
from cartesianmesh2d import CartesianMesh2D
from pvdfilewriter2d import PvdFileWriter2D

import dace 
from dace.sdfg import SDFG
from dace.memlet import Memlet

class SimpleMeshExample:
    stopTime = 1.0
    maxIterations = 1
    deltat = 0.001
 
    ''' Mesh and mesh variables'''
    def __init__(self, mesh):
        self.__mesh = mesh
        self.__nbNodes = mesh.nbNodes
        self.__nbCells = mesh.nbCells
        self.__maxNodesOfCell = mesh.MaxNbNodesOfCell

    def jsonInit(self, jsonContent):
        self.__outputPath = jsonContent["outputPath"]
        self.__writer = PvdFileWriter2D("SimpleMeshExample", self.__outputPath)
        self.outputPeriod = jsonContent["outputPeriod"]
        self.lastDump = -sys.maxsize - 1 # Integer.MIN_VALUE
        self.X = np.empty((self.__nbNodes, 2), dtype=np.double)
        self.nodes_sum = np.empty(self.__nbCells, dtype=np.double)
        self.cst = np.empty(self.__nbNodes, dtype=np.double)

        # Copy node coordinates
        gNodes = mesh.geometry.nodes
        for rNodes in range(self.__nbNodes):
            self.X[rNodes] = gNodes[rNodes]

    def _computeCst(self):
        for rNodes in range(self.__nbNodes): 
            self.cst[rNodes] = 1.0
    
    def _computeTn(self):
        self.t_nplus1 = self.t_n + self.deltat

    def _initTime(self):
        self.t_n0 = 0.0
    
    def _computeSum(self):
        for jCells in range(self.__nbCells):
            jId = jCells
            reduction0 = 0.0
            nodesOfCellJ = mesh.getNodesOfCell(jId)
            nbNodesOfCellJ = len(nodesOfCellJ)
            for rNodesOfCellJ in range(nbNodesOfCellJ):
                rId = nodesOfCellJ[rNodesOfCellJ]
                rNodes = rId
                reduction0 = self.__sumR0(reduction0, self.cst[rNodes] * jCells)
            self.nodes_sum[jCells] = reduction0
            
    def _setUpTimeLoopN(self):
        self.t_n = self.t_n0;

    def _assertSum(self):
        for jCells in range(self.__nbCells):
            b = self.__assertEquals(4.0 * jCells, self.nodes_sum[jCells])

    def _executeTimeLoopN(self):
        self.n = 0
        continueLoop = True
        while continueLoop:
            self.n += 1
            print("START ITERATION n: %5d - t: %5.5f - deltat: %5.5f\n" % (self.n, self.t_n, self.deltat))
        
            if (self.n >= self.lastDump + self.outputPeriod):
                self.__dumpVariables(self.n)

            self._computeTn() # @1.0
        
            # Evaluate loop condition with variables at time n
            continueLoop = (self.n < self.maxIterations)
        
            self.t_n = self.t_nplus1
        
        print("FINAL TIME: %5.5f - deltat: %5.5f\n" % (self.t_n, self.deltat))
        self.__dumpVariables(self.n+1)

    def __assertEquals(self, expected, actual):
        assert (expected == actual)

    def __sumR0(self, a, b):
        return a + b

    def __dumpVariables(self, iteration):
        if not self.__writer.disabled:
            quads = mesh.geometry.quads
            self.__writer.startVtpFile(iteration, self.t_n, self.X, quads)
            self.__writer.openNodeData()
            self.__writer.closeNodeData()
            self.__writer.openCellData()
            self.__writer.openCellArray("sum", 0)
            for i in range(self.__nbCells):
                self.__writer.write(self.nodes_sum[i])
            self.__writer.closeCellArray()
            self.__writer.closeCellData()
            self.__writer.closeVtpFile()
            self.lastDump = self.n
            
        
    def _generateGraphSDFG(self):
        
        nbCells = dace.symbol('nbCells')
        nbNodes = dace.symbol('nbNodes')
        maxNodesOfCell = dace.symbol('maxNodesOfCell')
        
        @dace.program
        def _computeCstComputeSum(nodesOfCells: dace.int64[nbCells, maxNodesOfCell], cst: dace.float64[nbNodes], nodes_sum: dace.float64[nbCells]):
            for rNodes in dace.map[0:nbNodes]:
                with dace.tasklet:
                        valuesOnNodes >> cst[rNodes]
                        valuesOnNodes = 1
            for jCells in dace.map[0:nbCells]:
                tmp = np.empty((maxNodesOfCell), dtype=np.float64)
                nodesOfCellJ = np.empty((maxNodesOfCell), dtype=np.int64)
                for rNodes in dace.map[0:maxNodesOfCell]:
                    with dace.tasklet:
                        idNodes << nodesOfCells[jCells,rNodes]
                        nodesCell >> nodesOfCellJ[rNodes]
                        nodesCell = idNodes
                for rNodes in dace.map[0:maxNodesOfCell]:
                    with dace.tasklet:
                        read_values_nodes << cst[nodesOfCellJ[rNodes]]
                        values_on_nodes >> tmp[rNodes]
                        values_on_nodes = read_values_nodes
                nodes_sum[jCells] = np.sum(tmp*jCells, axis=0)
        
        '''Initialize values of input data'''
        
        nodesOfCells_array = np.empty((0, 4), np.int64)
        for jCells in range(self.__nbCells):
            jId = jCells
            nodesOfCellJ = mesh.getNodesOfCell(jId)
            nodesOfCells_array = np.append(nodesOfCells_array, np.array([nodesOfCellJ]), axis=0)
            
        '''Initialize values of output data'''
        nodes_sum_array = np.full((self.__nbCells), 0.0)
        nodes_sum_array = np.array(nodes_sum_array)
        nodes_sum_array.astype(np.float64)
        
        print("nodesOfCells_array", nodesOfCells_array)
        
        
        _computeCstComputeSum(cst=self.cst, nodesOfCells=nodesOfCells_array, nodes_sum=nodes_sum_array, nbCells=self.__nbCells, nbNodes=self.__nbNodes, maxNodesOfCell=self.__maxNodesOfCell)
    
        print("cst : ", self.cst)
        
        print("nodes_sum_array : ", nodes_sum_array)
        
        
        sdfg = _computeCstComputeSum.to_sdfg()
        sdfg.view("sdfgGraphWithSingleState")
        
    
    def simulate(self):
        print("Start execution of simpleMeshExample")
        self._computeCst() # @1.0
        self._initTime() # @1.0
        self._computeSum() # @2.0
        self._setUpTimeLoopN() # @2.0
        self._assertSum() # @3.0
        self._executeTimeLoopN() # @3.0
        self._generateGraphSDFG()
        print("End of execution of simpleMeshExample")

if __name__ == '__main__':
    args = sys.argv[1:]
    
    if len(args) == 1:
        f = open(args[0])
        data = json.load(f)
        f.close()

        # Mesh instanciation    
        mesh = CartesianMesh2D()
        mesh.jsonInit(data["mesh"])
    
        # Module instanciation
        simpleMeshExample = SimpleMeshExample(mesh)
        simpleMeshExampleData = data["simpleMeshExample"]
        if simpleMeshExampleData:
            simpleMeshExample.jsonInit(simpleMeshExampleData) 

        # Start simulation
        simpleMeshExample.simulate()
    else:
        print("[ERROR] Wrong number of arguments: expected 1, actual " + str(len(args)), file=sys.stderr)
        print("        Expecting user data file name, for example SimpleMeshExample.json", file=sys.stderr)
        exit(1)