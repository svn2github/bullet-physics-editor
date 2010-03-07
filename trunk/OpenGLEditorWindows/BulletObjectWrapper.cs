using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CppCLI;

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

        public string Name 
        {
            get { return BulletWrapper.GetName(Index); }
            set { BulletWrapper.SetName(value, Index); }
        }

        public override string ToString()
        {
            return Name;
        }
    }

    public class BulletObjectWrapperCollection : List<BulletObjectWrapper>
    {
        public BulletObjectWrapperCollection(ExperimentalBulletWrapper wrapper)
        {
            for (uint i = 0; i < wrapper.Count; i++)
            {
                this.Add(new BulletObjectWrapper(i, wrapper));
            }
        }
    }
}
