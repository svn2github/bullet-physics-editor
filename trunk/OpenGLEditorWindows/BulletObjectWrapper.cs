using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ManagedCpp;

namespace OpenGLEditorWindows
{
    public class BulletObjectWrapper
    {
        public uint Index;
        public ExperimentalBulletWrapper BulletWrapper;

        public BulletObjectWrapper(uint index, ExperimentalBulletWrapper wrapper)
        {
            Index = index;
            BulletWrapper = wrapper;
        }

        public override string ToString()
        {
            return BulletWrapper.GetName(Index);
        }
    }
}
