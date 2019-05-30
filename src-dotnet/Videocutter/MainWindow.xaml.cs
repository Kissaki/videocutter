using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace KCode.Videocutter
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        public void OpenFile(string fpath)
        {
            Console.WriteLine($"Opening file {fpath}");
            cMediaElement.Source = new Uri(fpath);
            cFileInfo.FilePath = new Uri(fpath);
            var fi = new FileInfo(fpath);
            sFilename.Content = fi.Name;
            sFileSize.Content = FormatFileSize(fi.Length);
        }

        private static readonly string[] FileSizeMagnitudes = { "B", "KB", "MB", "GB", "TB" };

        private string FormatFileSize(long bytes)
        {
            double len = bytes;
            int order = 0;
            while (len >= 1024 && order < FileSizeMagnitudes.Length - 1)
            {
                ++order;
                len /= 1024;
            }

            return $"{ (object)len:0.##} { (object)FileSizeMagnitudes[order]}";
        }

        private void BtnPlay_Click(object sender, RoutedEventArgs e)
        {
            cMediaElement.Play();
        }

        private void BtnPause_Click(object sender, RoutedEventArgs e)
        {
            cMediaElement.Pause();
        }

        private void BtnStop_Click(object sender, RoutedEventArgs e)
        {
            cMediaElement.Stop();
        }

        private void MediaElement_Drop(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                return;
            }
            string[] fpaths = (string[])e.Data.GetData(DataFormats.FileDrop);
            if (fpaths.Length == 1)
            {
                OpenFile(fpaths[0]);
            }
            else
            {
                throw new NotImplementedException($"Multiple files were dropped ({fpaths.Length}).");
            }
        }

        private void MediaElement_DragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effects = DragDropEffects.Move;
            }
            else
            {
                e.Effects = DragDropEffects.None;
            }
            e.Handled = false;
        }

        private void BtnSkipForward_Click(object sender, RoutedEventArgs e)
        {
            var target = cMediaElement.Position + TimeSpan.FromSeconds(5);
            cMediaElement.Position = target > cMediaElement.NaturalDuration.TimeSpan ? cMediaElement.NaturalDuration.TimeSpan : target;
        }

        private void BtnSkipBackward_Click(object sender, RoutedEventArgs e)
        {
            var target = cMediaElement.Position - TimeSpan.FromSeconds(3);
            cMediaElement.Position = target < TimeSpan.Zero ? TimeSpan.Zero : target;
        }
    }
}
