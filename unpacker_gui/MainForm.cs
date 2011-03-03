/******************************************************************************
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
                    Version 2, December 2004 

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net> 

 Everyone is permitted to copy and distribute verbatim or modified 
 copies of this license document, and changing it is allowed as long 
 as the name is changed. 

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 

  0. You just DO WHAT THE FUCK YOU WANT TO.
******************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.Reflection;
using System.Resources;
using System.IO;

namespace Unpacker
{
    public partial class MainForm : Form
    {
        private ExtractForm extractForm;
        private Process unpacker;
        private bool converterExited;

        public MainForm()
        {
            InitializeComponent();

            OutputWindow.Text = global::Unpacker.Properties.Resources.m2033;
            OutputWindow.Select(0, 0);

            extractForm = new ExtractForm();

            converterExited = true;

            FormClosing += new FormClosingEventHandler(onClose);
        }

        private void processDataReceived(object sender, DataReceivedEventArgs r)
        {
            if (!String.IsNullOrEmpty(r.Data))
            {
                OutputWindow.Invoke(new MethodInvoker(() =>
                {
                    OutputWindow.AppendText(r.Data + "\r\n");
                    OutputWindow.DeselectAll();
                }));
            }
        }

        private void processExited(object sender, EventArgs e)
        {
            unpacker.WaitForExit();
            OutputWindow.Invoke(new MethodInvoker(() =>
            {
                extractToolStripMenuItem.Enabled = true;
                cancelToolStripMenuItem.Enabled = false;
                OutputWindow.AppendText("Completed.\r\n");
                OutputWindow.DeselectAll();
                converterExited = true;
            }));
        }

        private void extractToolStripMenuItem_Click(object sender, EventArgs e)
        {
            extractForm.ShowDialog(this);
            if (extractForm.DialogResult != DialogResult.OK)
            {
                return;
            }

            unpacker = new Process();
            unpacker.StartInfo.FileName = "m2033unp.exe";
            unpacker.StartInfo.CreateNoWindow = true;
            unpacker.StartInfo.UseShellExecute = false;
            unpacker.StartInfo.RedirectStandardOutput = true;
            unpacker.StartInfo.RedirectStandardError = true;
            unpacker.Exited += new EventHandler(processExited);
            unpacker.EnableRaisingEvents = true;
            unpacker.OutputDataReceived += new DataReceivedEventHandler(processDataReceived);
            unpacker.ErrorDataReceived += new DataReceivedEventHandler(processDataReceived);

            unpacker.StartInfo.Arguments = parseDialogData();

            extractToolStripMenuItem.Enabled = false;
            cancelToolStripMenuItem.Enabled = true;
            OutputWindow.Text = "Started...\r\n";
            OutputWindow.DeselectAll();

            converterExited = false;

            try
            {
                unpacker.Start();
            }
            catch (Win32Exception ex)
            {
                OutputWindow.Text = ex.ToString();
                converterExited = true;
                extractToolStripMenuItem.Enabled = true;
                cancelToolStripMenuItem.Enabled = false;
                return;
            }

            unpacker.BeginOutputReadLine();
            unpacker.BeginErrorReadLine();
        }

        private String parseDialogData()
        {
            String file = extractForm.InputFileBox.Text;
            String path = extractForm.OutputFileBox.Text;

            if (String.IsNullOrEmpty(path))
            {
                return "";
            }

            return "\"" + file + "\" \"" + path + "\"";
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        protected virtual void onClose(object sender, EventArgs e)
        {
            if (!converterExited)
            {
                unpacker.Kill();
            }
        }

        private void cancelToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            if (!converterExited)
            {
                unpacker.Kill();
            }
        }
    }
}
