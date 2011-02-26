using System;
using System.Windows.Forms;

namespace Unpacker
{
    public static class Program
    {
        public static MainForm MainWindow;

        [STAThread]
        static void Main()
        {
            MainWindow = new MainForm();
            Application.Run(MainWindow);
        }
    };
}