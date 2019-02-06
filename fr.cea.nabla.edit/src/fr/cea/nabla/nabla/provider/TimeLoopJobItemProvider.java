/**
 * generated by Xtext 2.15.0
 */
package fr.cea.nabla.nabla.provider;


import fr.cea.nabla.nabla.NablaFactory;
import fr.cea.nabla.nabla.NablaPackage;
import fr.cea.nabla.nabla.TimeLoopJob;

import java.util.Collection;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EStructuralFeature;

import org.eclipse.emf.edit.provider.IItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.ViewerNotification;

/**
 * This is the item provider adapter for a {@link fr.cea.nabla.nabla.TimeLoopJob} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class TimeLoopJobItemProvider extends JobItemProvider {
	/**
	 * This constructs an instance from a factory and a notifier.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public TimeLoopJobItemProvider(AdapterFactory adapterFactory) {
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
			childrenFeatures.add(NablaPackage.Literals.TIME_LOOP_JOB__ITERATOR);
			childrenFeatures.add(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION);
			childrenFeatures.add(NablaPackage.Literals.TIME_LOOP_JOB__BODY);
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
	 * This returns TimeLoopJob.gif.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object getImage(Object object) {
		return overlayImage(object, getResourceLocator().getImage("full/obj16/TimeLoopJob"));
	}

	/**
	 * This returns the label text for the adapted class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String getText(Object object) {
		String label = ((TimeLoopJob)object).getName();
		return label == null || label.length() == 0 ?
			getString("_UI_TimeLoopJob_type") :
			getString("_UI_TimeLoopJob_type") + " " + label;
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

		switch (notification.getFeatureID(TimeLoopJob.class)) {
			case NablaPackage.TIME_LOOP_JOB__ITERATOR:
			case NablaPackage.TIME_LOOP_JOB__INITIALIZATION:
			case NablaPackage.TIME_LOOP_JOB__BODY:
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
				(NablaPackage.Literals.TIME_LOOP_JOB__ITERATOR,
				 NablaFactory.eINSTANCE.createTimeIterator()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION,
				 NablaFactory.eINSTANCE.createInstruction()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION,
				 NablaFactory.eINSTANCE.createScalarVarDefinition()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION,
				 NablaFactory.eINSTANCE.createVarGroupDeclaration()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION,
				 NablaFactory.eINSTANCE.createInstructionBlock()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION,
				 NablaFactory.eINSTANCE.createAffectation()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION,
				 NablaFactory.eINSTANCE.createLoop()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION,
				 NablaFactory.eINSTANCE.createIf()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__BODY,
				 NablaFactory.eINSTANCE.createInstruction()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__BODY,
				 NablaFactory.eINSTANCE.createScalarVarDefinition()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__BODY,
				 NablaFactory.eINSTANCE.createVarGroupDeclaration()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__BODY,
				 NablaFactory.eINSTANCE.createInstructionBlock()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__BODY,
				 NablaFactory.eINSTANCE.createAffectation()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__BODY,
				 NablaFactory.eINSTANCE.createLoop()));

		newChildDescriptors.add
			(createChildParameter
				(NablaPackage.Literals.TIME_LOOP_JOB__BODY,
				 NablaFactory.eINSTANCE.createIf()));
	}

	/**
	 * This returns the label text for {@link org.eclipse.emf.edit.command.CreateChildCommand}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String getCreateChildText(Object owner, Object feature, Object child, Collection<?> selection) {
		Object childFeature = feature;
		Object childObject = child;

		boolean qualify =
			childFeature == NablaPackage.Literals.TIME_LOOP_JOB__INITIALIZATION ||
			childFeature == NablaPackage.Literals.TIME_LOOP_JOB__BODY;

		if (qualify) {
			return getString
				("_UI_CreateChild_text2",
				 new Object[] { getTypeText(childObject), getFeatureText(childFeature), getTypeText(owner) });
		}
		return super.getCreateChildText(owner, feature, child, selection);
	}

}
