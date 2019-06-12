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
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Path = System.IO.Path;

namespace KCode.Videocutter
{
    public partial class MainWindow : Window
    {
        private bool IsPlaying { get; set; }
        private DirectoryInfo CurrentDir { get; set; }
        private FileInfo CurrentFile { get; set; }
        private MediaTimeline MediaTimeline;

        public MainWindow()
        {
            InitializeComponent();
        }

        public void OpenFile(string fpath)
        {
            Console.WriteLine($"Opening file {fpath}");
            MediaTimeline = new MediaTimeline(new Uri(fpath));
            MediaTimeline.RepeatBehavior = RepeatBehavior.Forever;
            cMediaElement.Clock = MediaTimeline.CreateClock();
            cFileInfo.FilePath = new Uri(fpath);
            CurrentFile = new FileInfo(fpath);
            sFilename.Content = CurrentFile.Name;
            sFileSize.Content = CurrentFile.LengthAsHumanString();
            IsPlaying = true;

            if (CurrentDir != CurrentFile.Directory)
            {
                CurrentDir = CurrentFile.Directory;
                var dirFiles = CurrentDir.GetFiles("*.mp4").Select(x => x.Name).OrderBy(x => x);
                cFilesList.ItemsSource = dirFiles;
            }
            cFilesList.SelectedValue = CurrentFile.Name;
        }

        private ClockController MediaController { get => cMediaElement.Clock.Controller; }
        private void BtnJumpStart_Click(object sender, RoutedEventArgs e) => MediaController.Seek(TimeSpan.Zero, TimeSeekOrigin.BeginTime);
        private void Play() { MediaController.Resume(); IsPlaying = true; }
        private void Pause() { MediaController.Pause(); IsPlaying = false; }
        private void Stop() { MediaController.Stop(); IsPlaying = false; }

        private void BtnPlayPause_Click(object sender, RoutedEventArgs e) { if (IsPlaying) { Pause(); } else { Play(); } }
        private void BtnStop_Click(object sender, RoutedEventArgs e) { Stop(); }

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

        private void BtnSkipForward_Click(object sender, RoutedEventArgs e) => JumpSkip(TimeSpan.FromSeconds(5));
        private void BtnSkipBackward_Click(object sender, RoutedEventArgs e) => JumpSkip(-TimeSpan.FromSeconds(3));
        private void JumpSkip(TimeSpan distance) => JumpTo(cMediaElement.Position + distance);
        private void JumpTo(TimeSpan target) => cMediaElement.Clock.Controller.Seek(TimeSpanValueRangeLimited(target, TimeSpan.Zero, cMediaElement.Clock.NaturalDuration.TimeSpan), System.Windows.Media.Animation.TimeSeekOrigin.BeginTime);
        private TimeSpan TimeSpanValueRangeLimited(TimeSpan value, TimeSpan min, TimeSpan max) => value > max ? max : (value < min ? min : value);

        private void CFilesList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (cFilesList.SelectedValue != null)
            {
                OpenFile(Path.Combine(CurrentDir.FullName, (string)cFilesList.SelectedValue));
            }
        }
    }
}
