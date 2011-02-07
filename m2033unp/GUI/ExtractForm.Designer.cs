namespace Unpacker
{
    partial class ExtractForm
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
            this.InputFileBox = new System.Windows.Forms.ComboBox();
            this.OpenInputFileButton = new System.Windows.Forms.Button();
            this.OutputFileBox = new System.Windows.Forms.ComboBox();
            this.OpenOutputFolder = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // InputFileBox
            // 
            this.InputFileBox.FormattingEnabled = true;
            this.InputFileBox.Location = new System.Drawing.Point(12, 27);
            this.InputFileBox.Name = "InputFileBox";
            this.InputFileBox.Size = new System.Drawing.Size(281, 21);
            this.InputFileBox.TabIndex = 0;
            // 
            // OpenInputFileButton
            // 
            this.OpenInputFileButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.OpenInputFileButton.Location = new System.Drawing.Point(299, 27);
            this.OpenInputFileButton.Name = "OpenInputFileButton";
            this.OpenInputFileButton.Size = new System.Drawing.Size(28, 21);
            this.OpenInputFileButton.TabIndex = 1;
            this.OpenInputFileButton.Text = "...";
            this.OpenInputFileButton.UseVisualStyleBackColor = true;
            this.OpenInputFileButton.Click += new System.EventHandler(this.OpenInputFileButton_Click);
            // 
            // OutputFileBox
            // 
            this.OutputFileBox.FormattingEnabled = true;
            this.OutputFileBox.Location = new System.Drawing.Point(12, 77);
            this.OutputFileBox.Name = "OutputFileBox";
            this.OutputFileBox.Size = new System.Drawing.Size(281, 21);
            this.OutputFileBox.TabIndex = 2;
            // 
            // OpenOutputFolder
            // 
            this.OpenOutputFolder.Location = new System.Drawing.Point(300, 77);
            this.OpenOutputFolder.Name = "OpenOutputFolder";
            this.OpenOutputFolder.Size = new System.Drawing.Size(27, 21);
            this.OpenOutputFolder.TabIndex = 3;
            this.OpenOutputFolder.Text = "...";
            this.OpenOutputFolder.UseVisualStyleBackColor = true;
            this.OpenOutputFolder.Click += new System.EventHandler(this.OpenOutputFolder_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(98, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Source archive file:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 60);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(87, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Destination path:";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(49, 114);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 7;
            this.button1.Text = "OK";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(196, 114);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 8;
            this.button2.Text = "Cancel";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // ExtractForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(338, 154);
            this.Controls.Add(this.InputFileBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.OpenInputFileButton);
            this.Controls.Add(this.OpenOutputFolder);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.OutputFileBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ExtractForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Extract";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button OpenInputFileButton;
        private System.Windows.Forms.Button OpenOutputFolder;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        public System.Windows.Forms.ComboBox InputFileBox;
        public System.Windows.Forms.ComboBox OutputFileBox;

    }
}