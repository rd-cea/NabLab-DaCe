/**
 */
package fr.cea.nabla.ir.ir;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see fr.cea.nabla.ir.ir.IrPackage
 * @generated
 */
public interface IrFactory extends EFactory {
	/**
	 * The singleton instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	IrFactory eINSTANCE = fr.cea.nabla.ir.ir.impl.IrFactoryImpl.init();

	/**
	 * Returns a new object of class '<em>Annotation</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Annotation</em>'.
	 * @generated
	 */
	IrAnnotation createIrAnnotation();

	/**
	 * Returns a new object of class '<em>Module</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Module</em>'.
	 * @generated
	 */
	IrModule createIrModule();

	/**
	 * Returns a new object of class '<em>Import</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Import</em>'.
	 * @generated
	 */
	Import createImport();

	/**
	 * Returns a new object of class '<em>Arg</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arg</em>'.
	 * @generated
	 */
	Arg createArg();

	/**
	 * Returns a new object of class '<em>Simple Variable</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Simple Variable</em>'.
	 * @generated
	 */
	SimpleVariable createSimpleVariable();

	/**
	 * Returns a new object of class '<em>Connectivity Variable</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Connectivity Variable</em>'.
	 * @generated
	 */
	ConnectivityVariable createConnectivityVariable();

	/**
	 * Returns a new object of class '<em>Function</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Function</em>'.
	 * @generated
	 */
	Function createFunction();

	/**
	 * Returns a new object of class '<em>Reduction</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Reduction</em>'.
	 * @generated
	 */
	Reduction createReduction();

	/**
	 * Returns a new object of class '<em>Connectivity</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Connectivity</em>'.
	 * @generated
	 */
	Connectivity createConnectivity();

	/**
	 * Returns a new object of class '<em>Item Arg Type</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Item Arg Type</em>'.
	 * @generated
	 */
	ItemArgType createItemArgType();

	/**
	 * Returns a new object of class '<em>Instruction Job</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Instruction Job</em>'.
	 * @generated
	 */
	InstructionJob createInstructionJob();

	/**
	 * Returns a new object of class '<em>In Situ Job</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>In Situ Job</em>'.
	 * @generated
	 */
	InSituJob createInSituJob();

	/**
	 * Returns a new object of class '<em>Before Time Loop Job</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Before Time Loop Job</em>'.
	 * @generated
	 */
	BeforeTimeLoopJob createBeforeTimeLoopJob();

	/**
	 * Returns a new object of class '<em>After Time Loop Job</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>After Time Loop Job</em>'.
	 * @generated
	 */
	AfterTimeLoopJob createAfterTimeLoopJob();

	/**
	 * Returns a new object of class '<em>Next Time Loop Iteration Job</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Next Time Loop Iteration Job</em>'.
	 * @generated
	 */
	NextTimeLoopIterationJob createNextTimeLoopIterationJob();

	/**
	 * Returns a new object of class '<em>Time Loop Copy</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Time Loop Copy</em>'.
	 * @generated
	 */
	TimeLoopCopy createTimeLoopCopy();

	/**
	 * Returns a new object of class '<em>Var Definition</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Var Definition</em>'.
	 * @generated
	 */
	VarDefinition createVarDefinition();

	/**
	 * Returns a new object of class '<em>Instruction Block</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Instruction Block</em>'.
	 * @generated
	 */
	InstructionBlock createInstructionBlock();

	/**
	 * Returns a new object of class '<em>Affectation</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Affectation</em>'.
	 * @generated
	 */
	Affectation createAffectation();

	/**
	 * Returns a new object of class '<em>Space Iteration Block</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Space Iteration Block</em>'.
	 * @generated
	 */
	SpaceIterationBlock createSpaceIterationBlock();

	/**
	 * Returns a new object of class '<em>Interval Iteration Block</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Interval Iteration Block</em>'.
	 * @generated
	 */
	IntervalIterationBlock createIntervalIterationBlock();

	/**
	 * Returns a new object of class '<em>Size Type Symbol</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Size Type Symbol</em>'.
	 * @generated
	 */
	SizeTypeSymbol createSizeTypeSymbol();

	/**
	 * Returns a new object of class '<em>Size Type Int</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Size Type Int</em>'.
	 * @generated
	 */
	SizeTypeInt createSizeTypeInt();

	/**
	 * Returns a new object of class '<em>Size Type Operation</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Size Type Operation</em>'.
	 * @generated
	 */
	SizeTypeOperation createSizeTypeOperation();

	/**
	 * Returns a new object of class '<em>Size Type Symbol Ref</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Size Type Symbol Ref</em>'.
	 * @generated
	 */
	SizeTypeSymbolRef createSizeTypeSymbolRef();

	/**
	 * Returns a new object of class '<em>Reduction Instruction</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Reduction Instruction</em>'.
	 * @generated
	 */
	ReductionInstruction createReductionInstruction();

	/**
	 * Returns a new object of class '<em>Loop</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Loop</em>'.
	 * @generated
	 */
	Loop createLoop();

	/**
	 * Returns a new object of class '<em>If</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>If</em>'.
	 * @generated
	 */
	If createIf();

	/**
	 * Returns a new object of class '<em>Return</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Return</em>'.
	 * @generated
	 */
	Return createReturn();

	/**
	 * Returns a new object of class '<em>Contracted If</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Contracted If</em>'.
	 * @generated
	 */
	ContractedIf createContractedIf();

	/**
	 * Returns a new object of class '<em>Binary Expression</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Binary Expression</em>'.
	 * @generated
	 */
	BinaryExpression createBinaryExpression();

	/**
	 * Returns a new object of class '<em>Unary Expression</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Unary Expression</em>'.
	 * @generated
	 */
	UnaryExpression createUnaryExpression();

	/**
	 * Returns a new object of class '<em>Parenthesis</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Parenthesis</em>'.
	 * @generated
	 */
	Parenthesis createParenthesis();

	/**
	 * Returns a new object of class '<em>Int Constant</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Int Constant</em>'.
	 * @generated
	 */
	IntConstant createIntConstant();

	/**
	 * Returns a new object of class '<em>Real Constant</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Real Constant</em>'.
	 * @generated
	 */
	RealConstant createRealConstant();

	/**
	 * Returns a new object of class '<em>Bool Constant</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Bool Constant</em>'.
	 * @generated
	 */
	BoolConstant createBoolConstant();

	/**
	 * Returns a new object of class '<em>Min Constant</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Min Constant</em>'.
	 * @generated
	 */
	MinConstant createMinConstant();

	/**
	 * Returns a new object of class '<em>Max Constant</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Max Constant</em>'.
	 * @generated
	 */
	MaxConstant createMaxConstant();

	/**
	 * Returns a new object of class '<em>Base Type Constant</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Base Type Constant</em>'.
	 * @generated
	 */
	BaseTypeConstant createBaseTypeConstant();

	/**
	 * Returns a new object of class '<em>Vector Constant</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Vector Constant</em>'.
	 * @generated
	 */
	VectorConstant createVectorConstant();

	/**
	 * Returns a new object of class '<em>Function Call</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Function Call</em>'.
	 * @generated
	 */
	FunctionCall createFunctionCall();

	/**
	 * Returns a new object of class '<em>Arg Or Var Ref</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arg Or Var Ref</em>'.
	 * @generated
	 */
	ArgOrVarRef createArgOrVarRef();

	/**
	 * Returns a new object of class '<em>Iterator</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Iterator</em>'.
	 * @generated
	 */
	Iterator createIterator();

	/**
	 * Returns a new object of class '<em>Connectivity Call</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Connectivity Call</em>'.
	 * @generated
	 */
	ConnectivityCall createConnectivityCall();

	/**
	 * Returns a new object of class '<em>Connectivity Call Iterator Ref</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Connectivity Call Iterator Ref</em>'.
	 * @generated
	 */
	ConnectivityCallIteratorRef createConnectivityCallIteratorRef();

	/**
	 * Returns a new object of class '<em>Arg Or Var Ref Iterator Ref</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arg Or Var Ref Iterator Ref</em>'.
	 * @generated
	 */
	ArgOrVarRefIteratorRef createArgOrVarRefIteratorRef();

	/**
	 * Returns a new object of class '<em>Item Type</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Item Type</em>'.
	 * @generated
	 */
	ItemType createItemType();

	/**
	 * Returns a new object of class '<em>Type</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Type</em>'.
	 * @generated
	 */
	IrType createIrType();

	/**
	 * Returns a new object of class '<em>Base Type</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Base Type</em>'.
	 * @generated
	 */
	BaseType createBaseType();

	/**
	 * Returns a new object of class '<em>Connectivity Type</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Connectivity Type</em>'.
	 * @generated
	 */
	ConnectivityType createConnectivityType();

	/**
	 * Returns a new object of class '<em>Time Loop Body Job</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Time Loop Body Job</em>'.
	 * @generated
	 */
	TimeLoopBodyJob createTimeLoopBodyJob();

	/**
	 * Returns the package supported by this factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the package supported by this factory.
	 * @generated
	 */
	IrPackage getIrPackage();

} //IrFactory