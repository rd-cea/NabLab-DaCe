import gmsh
import json
import glob

class FullGmshCartesianMesh2D():
    ''' Private  variables '''
    __maxNodesOfCell = 4
    
    def getMaxNodesOfCell(self):
        return self.__maxNodesOfCell
    
    ''' Get data from the json file '''
    def jsonInit(self):
        # Looking at a file with the .json as extension
        jsonFileFound = glob.glob('*.json')
        if len(jsonFileFound) !=1:
            raise ValueError('should be only one json file in the current directory')
        jsonFilename = jsonFileFound[0]
        # Open the json file and get data in this file
        with open(jsonFilename) as f:
            data = json.load(f)
        self.__nbXQuads = data['mesh']['nbXQuads']
        self.__nbYQuads = data['mesh']['nbYQuads']
        self.__xSize = data['mesh']['xSize']
        self.__ySize = data['mesh']['ySize']
        self.__create()
    
    ''' Number of the nodes '''
    def getNbNodes(self):
        return (self.__nbXQuads + 1)*(self.__nbYQuads+1)
    
    ''' Number of the cells '''
    def getNbCells(self):
        return self.__nbXQuads * self.__nbYQuads
    
    ''' Get tags of the different cells '''
    def getTagsQuadrangle(self):
        self.quadrangleTags, self.cellNodesTags = gmsh.model.mesh.getElementsByType(3)
        return self.quadrangleTags, self.cellNodesTags
    
    ''' Get tags of the different edges '''
    def getTagsEdges(self):
        self.lineTags, self.edgeNodesTags = gmsh.model.mesh.getElementsByType(1)
        return self.lineTags, self.edgeNodesTags
    
    ''' Get nodes of the quadrangles '''
    def getQuadrangleNodes(self, quadrangleTag):
        listNodeTagsQuadrangle = []
        for i in range(0,len(self.cellNodesTags), 4):
            listNodeTagsQuadrangle.append([self.cellNodesTags[i], self.cellNodesTags[i+1], self.cellNodesTags[i+2], self.cellNodesTags[i+3]])
           
        if(quadrangleTag > 0):
            #print("Nodes of the " + str(quadrangleTag) + " cell : " + str(listNodeTagsQuadrangle[quadrangleTag-(len(self.lineTags)+1)]))
            return str(listNodeTagsQuadrangle[quadrangleTag-(len(self.lineTags)+1)])
    
    ''' Get nodes of the edges '''
    def getEdgeNodes(self, edgeTag):
        listNodeTagsLines = []
        for i in range(0,len(self.edgeNodesTags), 2):
            listNodeTagsLines.append([self.edgeNodesTags[i], self.edgeNodesTags[i+1]])
            
        if(edgeTag > 0):
            #print("Nodes of the "+ str(edgeTag) + " edge : " + str(listNodeTagsLines[edgeTag-1]))
            return str(listNodeTagsLines[edgeTag-1])
    
    ''' Add values on all cells '''
    def addValuesOnCells(self, name, arrayDataInCells, step):
        tagView = gmsh.view.add(name)
        # Add values of the temperature in nodes
        gmsh.view.addModelData(tagView, step, self.__modelName, 'ElementData', self.quadrangleTags, arrayDataInCells)
        return tagView
    
    ''' Add values on all nodes '''
    def addValuesOnNodes(self, name, arrayDataInNodes, step):
        tagView = gmsh.view.add(name)
        # Add values of the volume in cells
        gmsh.view.addModelData(tagView, step, self.__modelName, 'NodeData', self.nodesTags, arrayDataInNodes, 0, 1)
        return tagView
    
    ''' Add values on all edges '''
    def addValuesOnEdges(self, name, arrayDataInNodes, step):
        tagView = gmsh.view.add(name)
        gmsh.view.addModelData(tagView, step, self.__modelName, 'ElementData', self.lineTags, arrayDataInNodes)
        return tagView
    
    ''' Get values on elements (nodes, edges, or cells) '''
    def getValuesData(self, tag, step=0):
        dataType, tags, data, time, numComp = gmsh.view.getModelData(tag, step)
        return dataType, tags, data, time, numComp
    
    ''' Run the event loop of the graphical user interface '''
    def launchVisualizationMesh(self):
        gmsh.fltk.run()
    
    def __init__(self, modelName):
        self.__nbXQuads = 0
        self.__nbYQuads = 0
        self.__xSize = 0.
        self.__ySize = 0.
        self.__modelName = modelName
        self.__dimMesh = 2
        # The x, y, z coordinates of all the nodes:
        self.coords = []
        # The connectivities of the rectangle elements (4 node tags per rectangle)
        self.rect = []
        # The connectivities of the line elements (2 node tags for each line element):
        self.lin = []
        # List tags of the nodes
        self.nodesTags = []
        # List tags of the edges
        self.lineTags =  []
        # List of nodes tags of the cells
        self.cellNodesTags =  []
        # List of nodes tags of the edges
        self.edgeNodesTags =  []
        # Tag of the surface
        self.tagSurface = 0 
        # Tags of the cells
        self.quadrangleTags = []
        # Tag of the discrete entity of the lines
        self.tagDiscreteEntityLines =  0
    
    def __create(self):
        ''' Initialize Gmsh API must be called before any call to the other functions in the API '''
        gmsh.initialize()
        
        ''' Add a new model, with name 'name', and set it as the current model '''
        gmsh.model.add(self.__modelName)
        
        ''' Describe the build of the mesh '''
        for j in range(self.__nbYQuads + 1):
            for i in range(self.__nbXQuads + 1):
                self.coords.extend([
                    round(i*self.__xSize,2), # x
                    round(j*self.__ySize,2), # y
                    0.0 # z
                ])
                
                if i > 0 and j > 0:
                    self.rect.extend([self.__tag(i-1, j-1), self.__tag(i, j - 1), self.__tag(i,j), self.__tag(i - 1, j)])
                if i > 0:
                    self.lin.extend([self.__tag(i-1, j), self.__tag(i,j)])
                if j > 0:
                    self.lin.extend([self.__tag(i,j-1), self.__tag(i,j)])
        
        ''' Add the tags of the different nodes '''
        print("numberofNodes : ", self.getNbNodes())
        for i in range(1, self.getNbNodes() + 1):
            self.nodesTags.append(i)
        print("size of self.nodeTags : ", len(self.nodesTags))
        
        ''' Add discrete entity on surface '''
        self.tagSurface = gmsh.model.addDiscreteEntity(2)
        
        ''' Add all nodes on the surface '''
        gmsh.model.mesh.addNodes(2, self.tagSurface, self.nodesTags, self.coords)
        print("coordinate of nodes : ", self.coords)
        print("array of nodesTags : ", self.nodesTags)
        print("type of nodesTags : ", type(self.nodesTags))
        print("size of coords : ", len(self.coords))
        print("size of nodesTags : ", len(self.nodesTags))
        
        ''' Add discrete entity for the lines '''
        self.tagDiscreteEntityLines = gmsh.model.addDiscreteEntity(1)
        
        ''' Add lines '''
        gmsh.model.mesh.addElementsByType(self.tagDiscreteEntityLines, 1, [], self.lin)
        
        ''' Add cells '''
        gmsh.model.mesh.addElementsByType(self.tagSurface, 3, [], self.rect)
        
        ''' Generate a mesh of the current model, up to dimension `dim' (0, 1, 2 or 3) '''
        gmsh.model.mesh.generate(self.__dimMesh)
        
        ''' Write a file. The export format is determined by the file extension '''
        gmsh.write(self.__modelName + ".msh")
    
    ''' Tag of different points'''
    def __tag(self, i, j):
        return i +j*(self.__nbXQuads+1) + 1
    
    def __del__(self):
        ''' Finalize the Gmsh API. This must be called when you are done using the Gmsh API '''
        gmsh.finalize()
    

if __name__ == '__main__':
    testMesh = FullGmshCartesianMesh2D("FullGmshCartesianMesh2DTest")
    testMesh.jsonInit()
    testMesh.launchVisualizationMesh()
