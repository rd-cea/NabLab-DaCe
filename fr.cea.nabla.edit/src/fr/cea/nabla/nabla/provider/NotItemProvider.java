/**
 * generated by Xtext 2.15.0
 */
package fr.cea.nabla.nabla.provider;


import fr.cea.nabla.nabla.NablaFactory;
import fr.cea.nabla.nabla.NablaPackage;
import fr.cea.nabla.nabla.Not;

import java.util.Collection;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EStructuralFeature;

import org.eclipse.emf.edit.provider.IItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.ViewerNotification;

/**
 * This is the item provider adapter for a {@link fr.cea.nabla.nabla.Not} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class NotItemProvider extends ExpressionItemProvider {
	/**
	 * This constructs an instance from a factory and a notifier.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotItemProvider(AdapterFactory adapterFactory) {
		super(adapterFactory);
	}

	/**
	 * This returns the property descriptors for the adapted class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public List<IItemPropertyDescriptor> getPropertyDescriptors(Object object) {
		if (itemPropertyDescriptors == null) {
			super.getPropertyDescriptors(object);

		}
		return itemPropertyDescriptors;
	}

	/**
	 * This specifies how to implement {@link #getChildren} and is used to deduce an appropriate feature for an
	 * {@link org.eclipse.emf.edit.command.AddCommand}, {@link org.eclipse.emf.edit.command.RemoveCommand} or
	 * {@link org.eclipse.emf.edit.command.MoveCommand} in {@link #createCommand}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Collection<? extends EStructuralFeature> getChildrenFeatures(Object object) {
		if (childrenFeatures == null) {
			super.getChildrenFeatures(object);
			childrenFeatures.add(NablaPackage.Literals.NOT__EXPRESSION);
		}
		return childrenFeatures;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EStructuralFeature getChildFeature(Object object, Object child) {
		// Check the type of the specified child object and return the proper feature to use for
		// adding (see {@link AddCommand}) it as a child.

		return super.getChildFeature(object, child);
	}

	/**
	 * This returns Not.gif.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object getImage(Object object) {
		return overlayImage(object, getResourceLocator().getImage("full/obj16/Not"));
	}

	/**
	 * This returns the label text for the adapted class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String getText(Object object) {
		return getString("_UI_Not_type");
	}


	/**
	 * This handles model notifications by calling {@link #updateChildren} to update any cached
	 * children and by creating a viewer notification, which it passes to {@link #fireNotifyChanged}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void notifyChanged(Notification notification) {
		updateChildren(notification);

		switch (notification.getFeatureID(Not.class)) {
			case NablaPackage.NOT__EXPRESSION:
				fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), true, false));
				return;
		}
		super.notifyChanged(notification);
	}

	/**
	 * This adds {@link org.eclipse.emf.edit.command.CommandParameter}s describing the children
	 * that can be created under this object.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected void collectNewChildDescriptors(Collection<Object> newChildDescriptors, Object object) {
		super.collectNewChildDescriptors(newChildDescriptors, object);

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createExpression()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createReal2Constant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createReal3Constant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createVarRef()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createOr()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createAnd()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createEquality()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createComparison()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createPlus()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createMinus()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createMulOrDiv()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createModulo()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createParenthesis()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createUnaryMinus()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createNot()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createIntConstant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createRealConstant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createBoolConstant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createReal2x2Constant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createReal3x3Constant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createRealXCompactConstant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createMinConstant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createMaxConstant()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createFunctionCall()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.NOT__EXPRESSION,
				 NablaFactory.eINSTANCE.createReductionCall()));
	}

}
