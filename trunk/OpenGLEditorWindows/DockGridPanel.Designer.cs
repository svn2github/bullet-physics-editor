namespace OpenGLEditorWindows
{
    partial class DockGridPanel
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.objectView = new System.Windows.Forms.DataGridView();
            ((System.ComponentModel.ISupportInitialize)(this.objectView)).BeginInit();
            this.SuspendLayout();
            // 
            // objectView
            // 
            this.objectView.AllowUserToAddRows = false;
            this.objectView.AllowUserToDeleteRows = false;
            this.objectView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.objectView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.objectView.Location = new System.Drawing.Point(0, 0);
            this.objectView.Name = "objectView";
            this.objectView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.objectView.Size = new System.Drawing.Size(235, 342);
            this.objectView.TabIndex = 0;
            // 
            // DockGridPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(235, 342);
            this.CloseButton = false;
            this.CloseButtonVisible = false;
            this.Controls.Add(this.objectView);
            this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft)
                        | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight)));
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.Name = "DockGridPanel";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockLeft;
            this.TabText = "Grid View";
            this.Text = "Grid View";
            ((System.ComponentModel.ISupportInitialize)(this.objectView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.DataGridView objectView;





    }
}