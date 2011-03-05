/******************************************************************************
This source file is part of metro2033-tools
For the latest info, see http://code.google.com/p/metro2033-tools/

Copyright (c) 2010-2011 Ivan Shishkin <codingdude@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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
