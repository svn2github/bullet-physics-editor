using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CppCLI;
using HotChocolate;
using System.Diagnostics;
using System.IO;
using WeifenLuo.WinFormsUI.Docking;

namespace OpenGLEditorWindows
{
    public partial class DocumentForm : Form, OpenGLSceneViewDelegate
    {
        UndoManager undo;
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
        DataGridView objectView = null;

        DockFourViews fourViewDock = null;
        DockPropertyPanel propertyPanel = null;
        DockGridPanel gridPanel = null;
        DockLogPanel logPanel = null;
        
        StringWriter logWriter;

        ExperimentalBulletWrapper bulletWrapper = null;
        Timer simulationTimer = null;
        OpenGLManipulatingController bulletController = null;

        public DocumentForm()
        {
            InitializeComponent();

            // need to fix that, never works correctly because we are in constructor            
            if (this.DesignMode) 
                return;

            bulletController = new OpenGLManipulatingController();
            bulletWrapper = new ExperimentalBulletWrapper();
            bulletController.Model = bulletWrapper;
            undo = new UndoManager();

            fourViewDock = new DockFourViews();
            propertyPanel = new DockPropertyPanel();
            gridPanel = new DockGridPanel();
            logPanel = new DockLogPanel();
            
            openGLSceneViewLeft = fourViewDock.openGLSceneViewLeft;
            openGLSceneViewTop = fourViewDock.openGLSceneViewTop;
            openGLSceneViewFront = fourViewDock.openGLSceneViewFront;
            openGLSceneViewPerspective = fourViewDock.openGLSceneViewPerspective;

            mainSplit = fourViewDock.mainSplit;
            topSplit = fourViewDock.topSplit;
            bottomSplit = fourViewDock.bottomSplit;

            logTextBox = logPanel.logTextBox;
            objectView = gridPanel.objectView;
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

            undo.NeedsSaveChanged += new EventHandler(undo_NeedsSaveChanged);
            this.FormClosing += new FormClosingEventHandler(Form1_FormClosing);

            views = new List<OpenGLSceneView>();
            OnEachViewDo(view => views.Add(view));

            this.KeyDown += new KeyEventHandler(DocumentForm_KeyDown);

            fourViewDock.Show(dockPanel1);
            propertyPanel.Show(dockPanel1);
            gridPanel.Show(dockPanel1);
            logPanel.Show(dockPanel1);
            
            dockPanel1.DockLeftPortion = 0.15;   // 15 percent of dock space, 25 percent is default
            dockPanel1.DockRightPortion = 0.15;

            Manipulated = bulletController;
            propertyGrid.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid_PropertyValueChanged);

            simulationTimer = new Timer();
            simulationTimer.Interval = 1000 / 60;
            simulationTimer.Enabled = false;
            simulationTimer.Tick += new EventHandler(simulationTimer_Tick);

            objectView.SelectionChanged += new EventHandler(objectView_SelectionChanged);
            
            comboBoxViewMode.Items.Add("Solid");
            comboBoxViewMode.Items.Add("Wireframe");
            comboBoxViewMode.SelectedItem = "Solid";

            comboBoxViewMode.SelectedIndexChanged += new EventHandler(comboBoxViewMode_SelectedIndexChanged);
        }

        void comboBoxViewMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            OnEachViewDo(view => 
                {
                    view.CurrentViewMode = (string)comboBoxViewMode.SelectedItem == "Solid" ? ViewMode.ViewModeSolid : ViewMode.ViewModeWireframe;
                    view.Invalidate();
                });
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
            formTitle.Append("Bullet Physics Editor - ");
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
                
        private void OnEachViewDo(Action<OpenGLSceneView> actionOnView)
        {
            actionOnView(openGLSceneViewLeft);
            actionOnView(openGLSceneViewTop);
            actionOnView(openGLSceneViewFront);
            actionOnView(openGLSceneViewPerspective);
        }

        void InvalidateViewsExcept(OpenGLSceneView view)
        {
            OnEachViewDo(v => { if (v != view) v.Invalidate(); });
            SyncObjectView();
            propertyGrid.Refresh();
        }

        void InvalidateAllViews()
        {
            OnEachViewDo(v => v.Invalidate());
            SyncObjectView();
            propertyGrid.Refresh();
        }

        bool ignoreSelectionChanged = false;

        void objectView_SelectionChanged(object sender, EventArgs e)
        {
            if (ignoreSelectionChanged)
                return;

            if (objectView.RowCount != bulletWrapper.Count)
                return;

            for (uint i = 0; i < bulletWrapper.Count; i++)
            {
                if (objectView.Rows[(int)i].Selected)
                    bulletWrapper.SetSelected(1, i);
                else
                    bulletWrapper.SetSelected(0, i);
            }

            bulletController.UpdateSelection();
            OnEachViewDo(v => v.Invalidate());
            propertyGrid.Refresh();
        }

        void SyncObjectView()
        {
            ignoreSelectionChanged = true;

            objectView.DataSource = new BulletObjectWrapperCollection(bulletWrapper);
            for (uint i = 0; i < bulletWrapper.Count; i++)
            {
                if (bulletWrapper.IsSelected(i) == 1)
                    objectView.Rows[(int)i].Selected = true;
                else
                    objectView.Rows[(int)i].Selected = false;
            }

            ignoreSelectionChanged = false;
        }

        private void SetManipulator(ManipulatorType manipulator)
        {
            OnEachViewDo(view => view.CurrentManipulator = manipulator);
            Manipulated.CurrentManipulator = manipulator;
            InvalidateAllViews();

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
                InvalidateAllViews();
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
            UpdateLogWindow();
        }

        public void ManipulationEnded(OpenGLSceneView view)
        {
            Trace.WriteLine("manipulationEnded");

            propertyGrid.Refresh();

            InvalidateViewsExcept(view);
            UpdateLogWindow();
        }

        public void SelectionChanged(OpenGLSceneView view)
        {
            InvalidateViewsExcept(view);
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.ChangeSelection(1);
            InvalidateAllViews();
        }

        private void invertSelectionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.InvertSelection();
            InvalidateAllViews();
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
                UpdateLogWindow();
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
            InvalidateAllViews(); //this also invalidates the Object View
            //OnEachViewDo(view => view.Invalidate());
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
            InvalidateAllViews();
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
            simulationTimer.Enabled = !simulationTimer.Enabled;
            btnPlay.Checked = simulationTimer.Enabled;
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated.SelectedCount <= 0)
                return;

            Manipulated.RemoveSelected();
            InvalidateAllViews();
        }


        private void UpdateLogWindow()
        {
            logTextBox.Text = logWriter.ToString();
            if (logTextBox.Text.Length - 1 >= 0)
            {
                logTextBox.Select(logTextBox.Text.Length - 1, 0);
                logTextBox.ScrollToCaret();
            }
        }
    }    
}
