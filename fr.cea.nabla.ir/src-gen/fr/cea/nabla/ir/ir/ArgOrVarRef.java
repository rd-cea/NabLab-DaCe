/**
 */
package fr.cea.nabla.ir.ir;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Arg Or Var Ref</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link fr.cea.nabla.ir.ir.ArgOrVarRef#getTarget <em>Target</em>}</li>
 *   <li>{@link fr.cea.nabla.ir.ir.ArgOrVarRef#getIterators <em>Iterators</em>}</li>
 *   <li>{@link fr.cea.nabla.ir.ir.ArgOrVarRef#getIndices <em>Indices</em>}</li>
 * </ul>
 *
 * @see fr.cea.nabla.ir.ir.IrPackage#getArgOrVarRef()
 * @model
 * @generated
 */
public interface ArgOrVarRef extends Expression {
	/**
	 * Returns the value of the '<em><b>Target</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Target</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Target</em>' reference.
	 * @see #setTarget(ArgOrVar)
	 * @see fr.cea.nabla.ir.ir.IrPackage#getArgOrVarRef_Target()
	 * @model required="true"
	 * @generated
	 */
	ArgOrVar getTarget();

	/**
	 * Sets the value of the '{@link fr.cea.nabla.ir.ir.ArgOrVarRef#getTarget <em>Target</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Target</em>' reference.
	 * @see #getTarget()
	 * @generated
	 */
	void setTarget(ArgOrVar value);

	/**
	 * Returns the value of the '<em><b>Iterators</b></em>' containment reference list.
	 * The list contents are of type {@link fr.cea.nabla.ir.ir.ArgOrVarRefIteratorRef}.
	 * It is bidirectional and its opposite is '{@link fr.cea.nabla.ir.ir.ArgOrVarRefIteratorRef#getReferencedBy <em>Referenced By</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Iterators</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Iterators</em>' containment reference list.
	 * @see fr.cea.nabla.ir.ir.IrPackage#getArgOrVarRef_Iterators()
	 * @see fr.cea.nabla.ir.ir.ArgOrVarRefIteratorRef#getReferencedBy
	 * @model opposite="referencedBy" containment="true"
	 * @generated
	 */
	EList<ArgOrVarRefIteratorRef> getIterators();

	/**
	 * Returns the value of the '<em><b>Indices</b></em>' containment reference list.
	 * The list contents are of type {@link fr.cea.nabla.ir.ir.Dimension}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Indices</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Indices</em>' containment reference list.
	 * @see fr.cea.nabla.ir.ir.IrPackage#getArgOrVarRef_Indices()
	 * @model containment="true" resolveProxies="true"
	 * @generated
	 */
	EList<Dimension> getIndices();

} // ArgOrVarRef
