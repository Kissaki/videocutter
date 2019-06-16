using KCode.Videocutter.DataTypes;
using KCode.Videocutter.ExternalInterfaces;
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
        //public static readonly DependencyProperty SliceMinProperty = DependencyProperty.Register(nameof(SliceMin), typeof(TimeSpan), typeof(MainWindow), new PropertyMetadata(TimeSpan.Zero));
        //public static readonly DependencyProperty SliceMaxProperty = DependencyProperty.Register(nameof(SliceMax), typeof(TimeSpan), typeof(MainWindow), new PropertyMetadata(TimeSpan.Zero));
        public static readonly DependencyProperty SliceMinMsProperty = DependencyProperty.Register(nameof(SliceMinMs), typeof(double), typeof(MainWindow), new PropertyMetadata(0.0));
        public static readonly DependencyProperty SliceMaxMsProperty = DependencyProperty.Register(nameof(SliceMaxMs), typeof(double), typeof(MainWindow), new PropertyMetadata(0.0));
        public static readonly DependencyProperty CurrentPosMsProperty = DependencyProperty.Register(nameof(CurrentPosMs), typeof(double), typeof(MainWindow), new PropertyMetadata(0.0));
        public static readonly DependencyProperty MarkingsProperty = DependencyProperty.Register("Markings", typeof(Markings), typeof(MainWindow), new PropertyMetadata(new Markings()));

        private bool IsPlaying { get; set; }
        private DirectoryInfo CurrentDir { get; set; }
        private FileInfo CurrentFile { get; set; }
        private MediaTimeline MediaTimeline;
        private Thread UpdateLoopThread;

        //public TimeSpan SliceMin
        //{
        //    get { return (TimeSpan)GetValue(SliceMinProperty); }
        //    set { SetValue(SliceMinProperty, value); }
        //}
        //public TimeSpan SliceMax
        //{
        //    get { return (TimeSpan)GetValue(SliceMaxProperty); }
        //    set { SetValue(SliceMaxProperty, value); }
        //}
        public double SliceMinMs
        {
            get { return (double)GetValue(SliceMinMsProperty); }
            set { SetValue(SliceMinMsProperty, value); }
        }
        public double SliceMaxMs
        {
            get { return (double)GetValue(SliceMaxMsProperty); }
            set { SetValue(SliceMaxMsProperty, value); }
        }
        public double CurrentPosMs
        {
            get { return (double)GetValue(CurrentPosMsProperty); }
            set { SetValue(CurrentPosMsProperty, value); }
        }

        public Markings Markings
        {
            get { return (Markings)GetValue(MarkingsProperty); }
            set { SetValue(MarkingsProperty, value); }
        }

        private FfmpegInterface Ffmpeg = new FfmpegInterface();

        public MainWindow()
        {
            InitializeComponent();

            cMediaElement.MediaOpened += CMediaElement_MediaOpened;

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

            Ffmpeg.ActiveCountChanged += (sender, e) => Dispatcher.Invoke(() => sStatus.Content = $"Active exports: {Ffmpeg.ActiveCount}");

            // TODO: Solve this through bindings now
            //cFrom.ValueChanged += (sender, e) => { if (TimeSpan.FromMilliseconds(cPosition.Minimum) > Clock.CurrentTime.Value) { JumpTo(TimeSpan.FromMilliseconds(cPosition.Minimum)); } };
            // TODO: Solve this through bindings now
            //cTo.ValueChanged += (sender, e) => { if (Clock.CurrentTime.Value > TimeSpan.FromMilliseconds(cPosition.Maximum)) { JumpTo(TimeSpan.FromMilliseconds(SliceMaxMs)); } };
            // TODO: This is currently in conflict with the UpdateLoopThread which auto updates the value according to the current position, but in a separate thread so the value will be outdated because playback went on once the value is changed on the event thread and the change event triggers.
            //cPosition.ValueChanged += (sender, e) => JumpTo(TimeSpan.FromMilliseconds(cPosition.Value));
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

            //Markings.Clear();
            //Markings.AddRange();
            Markings = Markings.LoadFor(fpath);
            //cMarkingsList.Markings = Markings.LoadFor(fpath);
        }

        public void SetSlice(Marking marking = null)
        {
            SliceMinMs = marking?.StartMs ?? 0.0;
            SliceMaxMs = marking?.EndMs ?? cTo.Maximum;

            JumpTo(TimeSpan.FromMilliseconds(SliceMinMs));
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
            cTo.Maximum = cMediaElement.NaturalDuration.TimeSpan.TotalMilliseconds;

            SliceMinMs = 0.0;
            SliceMaxMs = cTo.Maximum;
            CurrentPosMs = 0.0;
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
            Dispatcher.Invoke(() => { if (cPosition.Value >= SliceMaxMs) { JumpToStart(); } });
        }
        private void SetCurrentTime(double ms) => CurrentPosMs = ms;

        private MediaClock Clock { get => cMediaElement.Clock; }
        private ClockController MediaController { get => Clock.Controller; }
        private void BtnJumpStart_Click(object sender, RoutedEventArgs e) => JumpToStart();

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

        private void BtnSkipForward_Click(object sender, RoutedEventArgs e) => JumpRelative(TimeSpan.FromSeconds(5));
        private void BtnSkipBackward_Click(object sender, RoutedEventArgs e) => JumpRelative(-TimeSpan.FromSeconds(3));
        private void JumpRelative(TimeSpan distance) => JumpTo(cMediaElement.Position + distance);
        private void JumpTo(TimeSpan target) => cMediaElement.Clock.Controller.Seek(TimeSpanValueRangeLimited(target, TimeSpan.FromMilliseconds(SliceMinMs), TimeSpan.FromMilliseconds(SliceMaxMs)), TimeSeekOrigin.BeginTime);
        private TimeSpan TimeSpanValueRangeLimited(TimeSpan value, TimeSpan min, TimeSpan max)
        {
            return value > max ? max : (value < min ? min : value);
        }
        private void JumpToStart() => MediaController.Seek(TimeSpan.FromMilliseconds(SliceMinMs), TimeSeekOrigin.BeginTime);

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

            var slice = new Marking
            {
                StartMs = (int)cFrom.Value,
                EndMs = (int)cTo.Value,
            };
            Ffmpeg.ExportSlice(CurrentFile, slice);
        }

        private void CMarkingsList_Play(object sender, Controls.MarkingsList.MarkingEventArgs e) => SetSlice(e.Marking);
        private void CMarkingsList_Export(object sender, Controls.MarkingsList.MarkingEventArgs e) => Ffmpeg.ExportSlice(CurrentFile, e.Marking);

    }
}
