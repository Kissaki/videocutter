using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace KCode.Videocutter.Controls
{
    public partial class FileInfoUC : UserControl
    {
        private Uri FilePathValue;
        public Uri FilePath { get => FilePathValue; set => SetFilePath(value); }

        public FileInfoUC()
        {
            InitializeComponent();
        }

        private void SetFilePath(Uri value)
        {
            FilePathValue = value;
            var fpath = FilePath.LocalPath;
            Filename.Content = fpath;
            var fi = new FileInfo(fpath);
            Size.Content = fi.LengthAsHumanString();
            Filename.Content = fi.Name;
        }

    }
}
