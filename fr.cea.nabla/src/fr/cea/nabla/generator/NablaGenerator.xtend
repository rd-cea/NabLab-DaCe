/*
 * generated by Xtext 2.9.0
 */
package fr.cea.nabla.generator

import com.google.inject.Inject
import fr.cea.nabla.generator.ir.Nabla2Ir
import fr.cea.nabla.ir.JobDependencyBuilder
import fr.cea.nabla.ir.ReplaceDefaultValueByInitJob
import fr.cea.nabla.ir.ReplaceInternalReduction
import fr.cea.nabla.ir.ReplaceUtf8ByAsciiChar
import fr.cea.nabla.ir.generator.java.Ir2Java
import fr.cea.nabla.ir.generator.kokkos.Ir2Kokkos
import fr.cea.nabla.nabla.NablaModule
import org.eclipse.emf.ecore.EObject
import org.eclipse.emf.ecore.resource.Resource
import org.eclipse.emf.ecore.resource.ResourceSet
import org.eclipse.emf.ecore.xmi.impl.XMIResourceFactoryImpl
import org.eclipse.xtext.generator.AbstractGenerator
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

/**
 * Generates code from your model files on save.
 * 
 * See https://www.eclipse.org/Xtext/documentation/303_runtime_concepts.html#code-generation
 */
class NablaGenerator extends AbstractGenerator 
{
	static val IrExtension = 'nablair'
	
	@Inject GeneratorUtils utils
	@Inject SmallLatexGenerator latexGenerator
	@Inject Nabla2Ir nabla2ir
	@Inject JobDependencyBuilder jobDependencyBuilder
//	@Inject Ir2N ir2n
	@Inject Ir2Java ir2java
	@Inject Ir2Kokkos ir2kmds
	
	// IR transformation passes
	val passes = #[new ReplaceUtf8ByAsciiChar, new ReplaceInternalReduction, new ReplaceDefaultValueByInitJob]

	override doGenerate(Resource input, IFileSystemAccess2 fsa, IGeneratorContext context) 
	{
		// 1 seul module par resource par d�finition (cf .xtext)
		val module = input.contents.filter(NablaModule).head
		val fileNameWithoutExtension = input.fileNameWithoutExtension

		// ecriture du fichier de modele
		if (!module.jobs.empty)
		{
			println('Generating Latex document')
			fsa.generateFile(module.name + '/' + fileNameWithoutExtension.addExtension('tex'), latexGenerator.getLatexContent(module))
			
			println('Serializing Nabla Intermediate Representation')
			val irModule = nabla2ir.toIrModule(module)
			var i=0
			//createAndSaveResource(fsa, input.resourceSet, irModule.name + '/' + fileNameWithoutExtension.addExtension(i.fileExtension), irModule)
			
			for (p : passes)
			{
				i++
				println('IR->IR: ' + p.description)
				p.transform(irModule)
				//createAndSaveResource(fsa, input.resourceSet, irModule.name + '/' + fileNameWithoutExtension.addExtension(i.fileExtension), irModule)		
			}

			// calcul des 'at'
			jobDependencyBuilder.fillAt(irModule)
			createAndSaveResource(fsa, input.resourceSet, irModule.name + '/' + fileNameWithoutExtension.addExtension(IrExtension), irModule)
	
			// g�n�ration du fichier .n
//			println('Generating IR --> Nabla V1 (.n)')
//			val nContent = ir2n.getFileContent(irModule)
//			fsa.generateFile(irModule.name + '/' + fileNameWithoutExtension.addExtension('n'), nContent)
	
			// g�n�ration du fichier .java
			println('Generating IR --> Java (.java)')
			val javaClassName = fileNameWithoutExtension.toFirstUpper
			val javaContent = ir2java.getFileContent(irModule, javaClassName)
			fsa.generateFile(irModule.name + '/' + javaClassName.addExtension('java'), javaContent)	

			// g�n�ration du fichier .kokkos
			println('Generating IR --> Kokkos (.cc)')
			val kmdsClassName = javaClassName + 'Kokkos'
			val kmdsContent = ir2kmds.getFileContent(irModule, kmdsClassName)
			fsa.generateFile(irModule.name + '/' + kmdsClassName.addExtension('cpp'), kmdsContent)	
		}
	}
	
//	private def getFileExtension(int passNumber) { 'pass' + passNumber + '.' + IrExtension }
	
	private def addExtension(String fileNameWithoutExtension, String fileExtension)
	{
		fileNameWithoutExtension + '.' + fileExtension
	} 
	
	private def getFileNameWithoutExtension(Resource input)
	{
		input.resourceSet.resourceFactoryRegistry.extensionToFactoryMap.put(Resource.Factory.Registry.DEFAULT_EXTENSION, new XMIResourceFactoryImpl)
		return input.URI.trimFileExtension.lastSegment
	}
	
	/** Cr�e et sauve la resource au m�me endroit que le param�tre baseResource en changeant l'extension par newExtension */
	private def createAndSaveResource(IFileSystemAccess2 fsa, ResourceSet rSet, String fileName, EObject content)
	{
		val uri = fsa.getURI(fileName)
		val resource = rSet.createResource(uri)
		resource.contents += content
		resource.save(utils.xmlSaveOptions)
	}
}
