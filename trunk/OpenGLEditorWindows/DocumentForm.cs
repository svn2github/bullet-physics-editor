using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ManagedCpp;
using HotChocolate;
using HotChocolate.Bindings;
using System.Diagnostics;
using System.IO;
using WeifenLuo.WinFormsUI.Docking;

namespace OpenGLEditorWindows
{
    public partial class DocumentForm : Form, OpenGLSceneViewDelegate
    {
        UndoManager undo;

        PropertyObserver<float> observerSelectionX;
        PropertyObserver<float> observerSelectionY;
        PropertyObserver<float> observerSelectionZ;

        List<OpenGLSceneView> views;

        string lastFileName = null;
        string fileDialogFilter = "Bullet (*.bullet)|*.bullet";

        OpenGLSceneView openGLSceneViewLeft = null;
        OpenGLSceneView openGLSceneViewTop = null;
        OpenGLSceneView openGLSceneViewFront = null;
        OpenGLSceneView openGLSceneViewPerspective = null;

        SplitContainer mainSplit = null;
        SplitContainer topSplit = null;
        SplitContainer bottomSplit = null;

        PropertyGrid propertyGrid = null;
        TextBox logTextBox = null;
        TreeView sceneGraphTreeView = null;

        DockFourViews fourViewDock = null;
        DockPropertyPanel propertyPanel = null;
        DockHierarchyPanel hierarchyPanel = null;
        DockLogPanel logPanel = null;

        StringWriter logWriter;

        ExperimentalBulletWrapper bulletWrapper = null;
        Timer simulationTimer = null;
        OpenGLManipulatingController bulletController = null;

        public DocumentForm()
        {
            InitializeComponent();

            if (this.DesignMode)
                return;

            bulletController = new OpenGLManipulatingController();
            bulletWrapper = new ExperimentalBulletWrapper();
            bulletController.Model = bulletWrapper;
            undo = new UndoManager();

            fourViewDock = new DockFourViews();
            propertyPanel = new DockPropertyPanel();
            hierarchyPanel = new DockHierarchyPanel();
            logPanel = new DockLogPanel();

            openGLSceneViewLeft = fourViewDock.openGLSceneViewLeft;
            openGLSceneViewTop = fourViewDock.openGLSceneViewTop;
            openGLSceneViewFront = fourViewDock.openGLSceneViewFront;
            openGLSceneViewPerspective = fourViewDock.openGLSceneViewPerspective;

            mainSplit = fourViewDock.mainSplit;
            topSplit = fourViewDock.topSplit;
            bottomSplit = fourViewDock.bottomSplit;

            logTextBox = logPanel.logTextBox;
            sceneGraphTreeView = hierarchyPanel.sceneGraphTreeView;
            propertyGrid = propertyPanel.propertyGrid;

            logWriter = new StringWriter();
            Trace.Listeners.Add(new TextWriterTraceListener(logWriter));

            logTextBox.Text = logWriter.ToString();

            topSplit.SplitterMoving += new SplitterCancelEventHandler(topSplit_SplitterMoving);
            bottomSplit.SplitterMoving += new SplitterCancelEventHandler(bottomSplit_SplitterMoving);

            topSplit.SplitterMoved += new SplitterEventHandler(topSplit_SplitterMoved);
            bottomSplit.SplitterMoved += new SplitterEventHandler(bottomSplit_SplitterMoved);

            openGLSceneViewLeft.CurrentCameraMode = CameraMode.CameraModeLeft;
            openGLSceneViewTop.CurrentCameraMode = CameraMode.CameraModeTop;
            openGLSceneViewFront.CurrentCameraMode = CameraMode.CameraModeFront;
            openGLSceneViewPerspective.CurrentCameraMode = CameraMode.CameraModePerspective;

            OnEachViewDo(view => view.CurrentManipulator = ManipulatorType.ManipulatorTypeDefault);

            bulletController.CurrentManipulator = openGLSceneViewLeft.CurrentManipulator;

            OnEachViewDo(view =>
                {
                    view.Displayed = view.Manipulated = bulletController;
                    view.TheDelegate = this;
                });

            observerSelectionX = this.ObserveProperty<float>("SelectionX");
            observerSelectionY = this.ObserveProperty<float>("SelectionY");
            observerSelectionZ = this.ObserveProperty<float>("SelectionZ");
            
            undo.NeedsSaveChanged += new EventHandler(undo_NeedsSaveChanged);
            this.FormClosing += new FormClosingEventHandler(Form1_FormClosing);

            views = new List<OpenGLSceneView>();
            OnEachViewDo(view => views.Add(view));

            this.KeyDown += new KeyEventHandler(DocumentForm_KeyDown);

            fourViewDock.Show(dockPanel1);
            propertyPanel.Show(dockPanel1);
            hierarchyPanel.Show(dockPanel1);
            logPanel.Show(dockPanel1);

            dockPanel1.DockLeftPortion = 0.15;   // 15 percent of dock space
            dockPanel1.DockRightPortion = 0.15;  // 25 percent is default

            Manipulated = bulletController;
            propertyGrid.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid_PropertyValueChanged);

            simulationTimer = new Timer();
            simulationTimer.Interval = 1000 / 60;
            simulationTimer.Enabled = false;
            simulationTimer.Tick += new EventHandler(simulationTimer_Tick);
        }

        void simulationTimer_Tick(object sender, EventArgs e)
        {
            if (bulletWrapper != null)
            {
                bulletWrapper.StepSimulation(1.0f / 60.0f);
                bulletController.UpdateSelection();
                OnEachViewDo(view => view.Invalidate());
            }
        }

        void propertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            OnEachViewDo(view => view.Invalidate());
        }

        bool IsSaveQuestionCancelled()
        {
            if (undo.NeedsSave)
            {
                DialogResult result = MessageBox.Show(
                    "Do you want to save this document?",
                    "Question",
                    MessageBoxButtons.YesNoCancel);

                switch (result)
                {
                    case DialogResult.Cancel:
                        return true;
                    case DialogResult.Yes:
                        saveToolStripMenuItem_Click(this, EventArgs.Empty);
                        return false;
                    case DialogResult.No:
                    default:
                        return false;
                }
            }
            return false;
        }

        void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (IsSaveQuestionCancelled())
                e.Cancel = true;
        }

        void undo_NeedsSaveChanged(object sender, EventArgs e)
        {
            StringBuilder formTitle = new StringBuilder();
            formTitle.Append("OpenGL Editor - ");
            if (string.IsNullOrEmpty(lastFileName))
                formTitle.Append("Untitled");
            else
                formTitle.Append(Path.GetFileNameWithoutExtension(lastFileName));
            if (undo.NeedsSave)
                formTitle.Append(" *");
            this.Text = formTitle.ToString();

            logTextBox.Text = logWriter.ToString();
            if (logTextBox.Text.Length - 1 >= 0)
            {
                logTextBox.Select(logTextBox.Text.Length - 1, 0);
                logTextBox.ScrollToCaret();
            }
        }

        #region Bindings magic

        void BindSelectionXYZ(OpenGLManipulatingController controller)
        {
            controller.ObserverSelectionX.WillChange += new EventHandler(ObserverSelectionX_WillChange);
            controller.ObserverSelectionY.WillChange += new EventHandler(ObserverSelectionY_WillChange);
            controller.ObserverSelectionZ.WillChange += new EventHandler(ObserverSelectionZ_WillChange);
            controller.ObserverSelectionX.DidChange += new EventHandler(ObserverSelectionX_DidChange);
            controller.ObserverSelectionY.DidChange += new EventHandler(ObserverSelectionY_DidChange);
            controller.ObserverSelectionZ.DidChange += new EventHandler(ObserverSelectionZ_DidChange);
        }

        void ObserverSelectionX_WillChange(object sender, EventArgs e)
        {
            observerSelectionX.RaiseWillChange();
        }

        void ObserverSelectionY_WillChange(object sender, EventArgs e)
        {
            observerSelectionY.RaiseWillChange();
        }

        void ObserverSelectionZ_WillChange(object sender, EventArgs e)
        {
            observerSelectionZ.RaiseWillChange();
        }

        void ObserverSelectionX_DidChange(object sender, EventArgs e)
        {
            observerSelectionX.RaiseDidChange();
        }

        void ObserverSelectionY_DidChange(object sender, EventArgs e)
        {
            observerSelectionY.RaiseDidChange();
        }

        void ObserverSelectionZ_DidChange(object sender, EventArgs e)
        {
            observerSelectionZ.RaiseDidChange();
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionX
        {
            get { return Manipulated.SelectionX; }
            set
            {
                ManipulationStarted(null);
                Manipulated.SelectionX = value;
                ManipulationEnded(null);
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionY
        {
            get { return Manipulated.SelectionY; }
            set
            {
                ManipulationStarted(null);
                Manipulated.SelectionY = value;
                ManipulationEnded(null);
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionZ
        {
            get { return Manipulated.SelectionZ; }
            set
            {
                ManipulationStarted(null);
                Manipulated.SelectionZ = value;
                ManipulationEnded(null);
            }
        }

        #endregion

        private void OnEachViewDo(Action<OpenGLSceneView> actionOnView)
        {
            actionOnView(openGLSceneViewLeft);
            actionOnView(openGLSceneViewTop);
            actionOnView(openGLSceneViewFront);
            actionOnView(openGLSceneViewPerspective);
        }

        private void SetManipulator(ManipulatorType manipulator)
        {
            OnEachViewDo(view => view.CurrentManipulator = manipulator);
            Manipulated.CurrentManipulator = manipulator;
            OnEachViewDo(view => view.Invalidate());

            btnSelect.Checked = btnTranslate.Checked = btnRotate.Checked = btnScale.Checked = false;
            switch (manipulator)
            {
                case ManipulatorType.ManipulatorTypeDefault:
                    btnSelect.Checked = true;
                    break;
                case ManipulatorType.ManipulatorTypeRotation:
                    btnRotate.Checked = true;
                    break;
                case ManipulatorType.ManipulatorTypeScale:
                    btnScale.Checked = true;
                    break;
                case ManipulatorType.ManipulatorTypeTranslation:
                    btnTranslate.Checked = true;
                    break;
                default:
                    break;
            }
        }

        OpenGLManipulatingController Manipulated
        {
            get { return openGLSceneViewLeft.Manipulated as OpenGLManipulatingController; }
            set
            {
                value.CurrentManipulator = openGLSceneViewLeft.CurrentManipulator;
                propertyGrid.SelectedObject = value;

                OnEachViewDo(view => view.Manipulated = value);
                OnEachViewDo(view => view.Invalidate());
            }
        }

        private void btnSelect_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeDefault);
        }

        private void btnTranslate_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeTranslation);
        }

        private void btnRotate_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeRotation);
        }

        private void btnScale_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeScale);
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            undo.Undo();
        }

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            undo.Redo();
        }

        public void ManipulationStarted(OpenGLSceneView view)
        {
            Trace.WriteLine("manipulationStarted");
        }

        public void ManipulationEnded(OpenGLSceneView view)
        {
            Trace.WriteLine("manipulationEnded");

            propertyGrid.Refresh();

            OnEachViewDo(v => { if (v != view) v.Invalidate(); });
        }

        public void SelectionChanged(OpenGLSceneView view)
        {
            propertyGrid.Refresh();
            //propertyGrid.SelectedObject = this.items;
            OnEachViewDo(v => { if (v != view) v.Invalidate(); });
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.ChangeSelection(1);
            OnEachViewDo(view => view.Invalidate());
        }

        private void invertSelectionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.InvertSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        private void editToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
        {
            undoToolStripMenuItem.Text = undo.CanUndo ?
                "Undo " + undo.UndoName : "Undo";

            redoToolStripMenuItem.Text = undo.CanRedo ?
                "Redo " + undo.RedoName : "Redo";

            undoToolStripMenuItem.Enabled = undo.CanUndo;
            redoToolStripMenuItem.Enabled = undo.CanRedo;
        }

        #region Splitter sync

        bool ignoreSplitterMoved = true;

        private void topSplit_SplitterMoved(object sender, SplitterEventArgs e)
        {
            if (ignoreSplitterMoved)
                return;

            bottomSplit.SplitterDistance = e.SplitX;
            ignoreSplitterMoved = true;
        }

        private void bottomSplit_SplitterMoved(object sender, SplitterEventArgs e)
        {
            if (ignoreSplitterMoved)
                return;

            topSplit.SplitterDistance = e.SplitX;
            ignoreSplitterMoved = true;
        }

        private void topSplit_SplitterMoving(object sender, SplitterCancelEventArgs e)
        {
            ignoreSplitterMoved = false;
        }

        private void bottomSplit_SplitterMoving(object sender, SplitterCancelEventArgs e)
        {
            ignoreSplitterMoved = false;
        }

        private void toggleOneViewFourViewMenuItem_Click(object sender, EventArgs e)
        {
            // one view -> four views
            if (mainSplit.Panel1Collapsed)
            {
                mainSplit.Panel1Collapsed = false;
                bottomSplit.Panel1Collapsed = false;
                openGLSceneViewPerspective.CurrentCameraMode = CameraMode.CameraModePerspective;
            }
            else // four views -> one view
            {
                Point screenPoint = Control.MousePosition;
                foreach (OpenGLSceneView view in views)
                {
                    Point clientPoint = view.PointToClient(screenPoint);
                    if (view.ClientRectangle.Contains(clientPoint))
                    {
                        openGLSceneViewPerspective.CurrentCameraMode = view.CurrentCameraMode;
                        mainSplit.Panel1Collapsed = true;
                        bottomSplit.Panel1Collapsed = true;
                        return;
                    }
                }

                Trace.WriteLine("No view is under mouse");
            }
        }

        #endregion

        #region File menu

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (IsSaveQuestionCancelled())
                return;

            bulletWrapper = new ExperimentalBulletWrapper();
            bulletController.Model = bulletWrapper;
            bulletController.UpdateSelection();
            undo.Clear();
            OnEachViewDo(view => view.Invalidate());
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (IsSaveQuestionCancelled())
                return;

            using (OpenFileDialog dlg = new OpenFileDialog())
            {
                dlg.Filter = fileDialogFilter;
                dlg.RestoreDirectory = true;
                if (dlg.ShowDialog() != DialogResult.OK)
                    return;

                lastFileName = dlg.FileName;
            }

            if (Path.GetExtension(lastFileName) == ".bullet")
            {
                bulletWrapper = new ExperimentalBulletWrapper();
                bulletWrapper.Load(lastFileName);
                bulletController.Model = bulletWrapper;
                Manipulated = bulletController;
            }
            
            undo.Clear();
            OnEachViewDo(view => view.Invalidate());
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(lastFileName))
            {
                using (SaveFileDialog dlg = new SaveFileDialog())
                {
                    dlg.Filter = fileDialogFilter;
                    dlg.RestoreDirectory = true;
                    if (dlg.ShowDialog() != DialogResult.OK)
                        return;

                    lastFileName = dlg.FileName;
                }
            }
            bulletWrapper.Save(lastFileName);
            undo.DocumentSaved();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog dlg = new SaveFileDialog())
            {
                dlg.Filter = fileDialogFilter;
                dlg.RestoreDirectory = true;
                if (dlg.ShowDialog() != DialogResult.OK)
                    return;

                lastFileName = dlg.FileName;
            }
            bulletWrapper.Save(lastFileName);
            undo.DocumentSaved();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        #endregion

        private void manipulatorToolStripMenuItem_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            int tag = int.Parse(e.ClickedItem.Tag.ToString());
            SetManipulator((ManipulatorType)tag);
        }

        #region Keyboard

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            // this allows to process keys in DocumentForm_KeyDown
            foreach (var view in views)
            {
                if (view.Focused)
                    base.ProcessKeyMessage(ref msg);
            }

            // but we also allow standard processing
            return base.ProcessCmdKey(ref msg, keyData);
        }

        void DocumentForm_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Space:
                    toggleOneViewFourViewMenuItem_Click(this, EventArgs.Empty);
                    break;
                case Keys.Q:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(selectToolStripMenuItem));
                    break;
                case Keys.W:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(translateToolStripMenuItem));
                    break;
                case Keys.E:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(rotateToolStripMenuItem));
                    break;
                case Keys.R:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(scaleToolStripMenuItem));
                    break;
            }
        }
      
        #endregion

        private void btnPlay_Click(object sender, EventArgs e)
        {
            simulationTimer.Enabled = true;
        }

        private void btnPause_Click(object sender, EventArgs e)
        {
            simulationTimer.Enabled = false;
        }
    }
}
