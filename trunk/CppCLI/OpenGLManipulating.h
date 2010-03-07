/*
 *  OpenGLManipulating.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 8/3/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "OpenGLSelecting.h"

namespace CppCLI
{
	public interface class OpenGLManipulating : OpenGLSelecting
	{
		property ManipulatorType CurrentManipulator { ManipulatorType get(); void set(ManipulatorType value); }
		property Vector3D SelectionCenter { Vector3D get(); void set(Vector3D value); }
		property Quaternion SelectionRotation { Quaternion get(); void set(Quaternion value); }
		property Vector3D SelectionScale { Vector3D get(); void set(Vector3D value); }
		property uint SelectedCount { uint get(); }

		property float PositionX { float get(); void set(float value); }
		property float PositionY { float get(); void set(float value); }
		property float PositionZ { float get(); void set(float value); }

		property float RotationX { float get(); void set(float value); }
		property float RotationY { float get(); void set(float value); }
		property float RotationZ { float get(); void set(float value); }

		property float ScaleX { float get(); void set(float value); }
		property float ScaleY { float get(); void set(float value); }
		property float ScaleZ { float get(); void set(float value); }

		void MoveSelectedBy(Vector3D offset);
		void RotateSelectedBy(Quaternion offset);
		void ScaleSelectedBy(Vector3D offset);
		void UpdateSelection();
		void Draw(ViewMode mode);
		void ChangeSelection(CocoaBool isSelected);
		void InvertSelection();
		void CloneSelected();
		void RemoveSelected();

		String^ GetName(uint index);
		void SetName(String^ name, uint index);
	};
}