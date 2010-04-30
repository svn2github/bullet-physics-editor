/*
 *  ExperimentalBulletWrapper.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 04/02/10.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "../PureCpp/BulletWrapperHelper.h"

#pragma managed

#include "OpenGLManipulatingModel.h"

namespace CppCLI
{
	public ref class ExperimentalBulletWrapper : OpenGLManipulatingModelItem
	{
	private:
		BulletWrapperHelper *wrapper;
	public:
		ExperimentalBulletWrapper();
		void StepSimulation(btScalar timeStep);
		void Load(String ^fileName);
		void Save(String ^fileName);

		virtual property uint Count { uint get(); }
		virtual	CocoaBool IsSelected(uint index);
		virtual	void SetSelected(CocoaBool selected, uint index);
		virtual void Draw(ViewMode mode);
		virtual	void Draw(uint index, CocoaBool forSelection, ViewMode mode);
		virtual	void CloneSelected();
		virtual	void RemoveSelected();
		
		virtual	void WillSelect();
		virtual	void DidSelect();

		virtual String^ GetShapeType(uint index);

		virtual float GetMass(uint index);
		virtual void SetMass(float value, uint index);

		virtual String^ GetName(uint index);
		virtual void SetName(String^ name, uint index);

		virtual Vector3D GetPosition(uint index);
		virtual Quaternion GetRotation(uint index);
		virtual Vector3D GetScale(uint index);
		virtual void SetPosition(Vector3D position, uint index);
		virtual void SetRotation(Quaternion rotation, uint index);
		virtual void SetScale(Vector3D scale, uint index);
		virtual void MoveBy(Vector3D offset, uint index);
		virtual void RotateBy(Quaternion offset, uint index);
		virtual void ScaleBy(Vector3D offset, uint index);
	};
}