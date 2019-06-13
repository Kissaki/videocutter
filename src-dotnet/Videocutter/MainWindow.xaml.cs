using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
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
        private Thread UpdateLoopThread;

        public MainWindow()
        {
            InitializeComponent();

            UpdateLoopThread = new Thread(UpdateLoop);
            UpdateLoopThread.Start();

            if (!File.Exists("ffmpeg.exe"))
            {
                var wantDownloadRes = MessageBox.Show("The program ffmpeg is required to extract and combine video, but it was not found.\nDo you want to download it now? (This is automated.)\nIf you choose no you will be able to open, view and prepare videos, but not encode them.", "No ffmpeg - download", MessageBoxButton.YesNo, MessageBoxImage.Question);
                if (wantDownloadRes == MessageBoxResult.Yes)
                {
                    FfmpegDownloader.Download();
                }
            }
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
                var dirFiles = GetVideoFiles(CurrentFile.Directory);
                cFilesList.ItemsSource = dirFiles;
            }
            cFilesList.SelectedValue = CurrentFile.Name;

            UpdateWindowTitle();

            cMediaElement.MediaOpened += CMediaElement_MediaOpened;
        }

        private IOrderedEnumerable<string> GetVideoFiles(DirectoryInfo fi)
        {
            CurrentDir = CurrentFile.Directory;
            var dirFiles = CurrentDir.GetFiles().Select(x => x.Name).Where(IsVideoFile).OrderBy(x => x);
            return dirFiles;
        }

        private static bool IsVideoFile(string filename) => filename.EndsWith(".mp4") || filename.EndsWith(".mkv");

        private void UpdateWindowTitle() => Title = "VideoCutter" + TitlePostfix;

        private string TitlePostfix { get => CurrentFile != null ? " - " + CurrentFile.Name + " in " + CurrentDir.FullName : string.Empty; }

        private void CMediaElement_MediaOpened(object sender, RoutedEventArgs e)
        {
            cPosition.Maximum = cMediaElement.NaturalDuration.TimeSpan.TotalMilliseconds;
            cFrom.Maximum = cMediaElement.NaturalDuration.TimeSpan.TotalMilliseconds;
            cTo.Maximum = cMediaElement.NaturalDuration.TimeSpan.TotalMilliseconds;
        }

        private void UpdateLoop()
        {
            while (true)
            {
                UpdateSeekers();
                Thread.Sleep(TimeSpan.FromMilliseconds(200));
            }
        }

        private void UpdateSeekers()
        {
            if (Clock == null)
            {
                return;
            }
            Dispatcher.Invoke(() => SetCurrentTime((Clock.CurrentTime ?? TimeSpan.Zero).TotalMilliseconds));
        }
        private void SetCurrentTime(double ms) => Dispatcher.Invoke(() => cPosition.Value = ms);

        private MediaClock Clock { get => cMediaElement.Clock; }
        private ClockController MediaController { get => Clock.Controller; }
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
        private void JumpTo(TimeSpan target) => cMediaElement.Clock.Controller.Seek(TimeSpanValueRangeLimited(target, TimeSpan.Zero, cMediaElement.Clock.NaturalDuration.TimeSpan), TimeSeekOrigin.BeginTime);
        private TimeSpan TimeSpanValueRangeLimited(TimeSpan value, TimeSpan min, TimeSpan max) => value > max ? max : (value < min ? min : value);

        private void CFilesList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (cFilesList.SelectedValue != null)
            {
                OpenFile(Path.Combine(CurrentDir.FullName, (string)cFilesList.SelectedValue));
            }
        }

        private void BtnExport_Click(object sender, RoutedEventArgs e)
        {
            if (!File.Exists("ffmpeg.exe"))
            {
                MessageBox.Show("The ffmpeg program could not be found. It is used to export. Can not export.");
                return;
            }
            var fromTimeMs = cFrom.Value;
            var toTimeMs = cTo.Value;
            var outPath = GetExtractFilepath((int)fromTimeMs, (int)toTimeMs);
            var fromS = fromTimeMs / 1000;
            var toS = toTimeMs / 1000;
            var extractArguments = @$"-ss {fromS.ToString("N3", CultureInfo.InvariantCulture)} -t {toS.ToString("N3", CultureInfo.InvariantCulture)}";
            var startArguments = @$"{extractArguments} -i ""{CurrentFile.FullName}"" ""{outPath}""";
            Debug.WriteLine($"Starting ffmpeg.exe {startArguments}");
            var p = Process.Start("ffmpeg.exe", startArguments);
            //p.OutputDataReceived += (sender, e) => sStatus.Content = e.Data;
            p.OutputDataReceived += (sender, e) => Debug.WriteLine($"ffmpeg.exe says: {e.Data}");
            //p.OutputDataReceived += (sender, e) => { using var log = File.AppendText("videocutter.log"); log.WriteLine(e.Data); };
            p.ErrorDataReceived += (sender, e) => Debug.WriteLine($"ffmpeg.exe says ERROR: {e.Data}");
            p.Exited += (sender, e) => { Debug.WriteLine($"ffmpeg.exe ended with exit code {p.ExitCode}"); p.Dispose(); };
        }

        private string GetExtractFilepath(int fromMs, int toMs) => CurrentFile.FullName + $"_{fromMs.ToString("D")}-{toMs.ToString("D")}.mp4";
    }
}
