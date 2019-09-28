using KCode.Videocutter.DataTypes;
using KCode.Videocutter.ExternalInterfaces;
using System;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using Path = System.IO.Path;

namespace KCode.Videocutter
{
    public partial class MainWindow : Window, IDisposable
    {
        public static readonly DependencyProperty CurrentFileProperty = DependencyProperty.Register(nameof(CurrentFile), typeof(FileInfo), typeof(MainWindow), new PropertyMetadata());
        public static readonly DependencyProperty MarkingsProperty = DependencyProperty.Register(nameof(Markings), typeof(MarkingCollection), typeof(MainWindow), new PropertyMetadata(new MarkingCollection()));
        //public static readonly DependencyProperty SliceMinProperty = DependencyProperty.Register(nameof(SliceMin), typeof(TimeSpan), typeof(MainWindow), new PropertyMetadata(TimeSpan.Zero));
        //public static readonly DependencyProperty SliceMaxProperty = DependencyProperty.Register(nameof(SliceMax), typeof(TimeSpan), typeof(MainWindow), new PropertyMetadata(TimeSpan.Zero));
        public static readonly DependencyProperty SliceMinMsProperty = DependencyProperty.Register(nameof(SliceMinMs), typeof(double), typeof(MainWindow), new PropertyMetadata(0.0));
        public static readonly DependencyProperty SliceMaxMsProperty = DependencyProperty.Register(nameof(SliceMaxMs), typeof(double), typeof(MainWindow), new PropertyMetadata(0.0));
        public static readonly DependencyProperty CurrentPosMsProperty = DependencyProperty.Register(nameof(CurrentPosMs), typeof(double), typeof(MainWindow), new PropertyMetadata(0.0));
        public static readonly DependencyProperty IsPlayerRepeatOnProperty = DependencyProperty.Register(nameof(IsPlayerRepeatOn), typeof(bool), typeof(MainWindow), new PropertyMetadata(defaultValue: true));
        public static readonly DependencyProperty IsPlayerMutedProperty = DependencyProperty.Register(nameof(IsPlayerMuted), typeof(bool), typeof(MainWindow), new PropertyMetadata(defaultValue: false));
        public static readonly DependencyProperty PlayerVolumeProperty = DependencyProperty.Register(nameof(PlayerVolume), typeof(double), typeof(MainWindow), new PropertyMetadata(defaultValue: 0.5));
        public static readonly DependencyProperty SettingContainerFormatProperty = DependencyProperty.Register(nameof(SettingContainerFormat), typeof(MediaContainerFormat), typeof(MainWindow));
        public static readonly DependencyProperty SettingVideoCodecProperty = DependencyProperty.Register(nameof(SettingVideoCodec), typeof(MediaVideoCodec), typeof(MainWindow));
        public static readonly DependencyProperty SettingAudioCodecProperty = DependencyProperty.Register(nameof(SettingAudioCodec), typeof(MediaAudioCodec), typeof(MainWindow));

        public FileInfo CurrentFile { get => (FileInfo)GetValue(CurrentFileProperty); set => SetValue(CurrentFileProperty, value); }
        internal MarkingCollection Markings { get => (MarkingCollection)GetValue(MarkingsProperty); set => SetValue(MarkingsProperty, value); }
        public double SliceMinMs { get => (double)GetValue(SliceMinMsProperty); set => SetValue(SliceMinMsProperty, value); }
        public double SliceMaxMs { get => (double)GetValue(SliceMaxMsProperty); set => SetValue(SliceMaxMsProperty, value); }
        public double CurrentPosMs { get => (double)GetValue(CurrentPosMsProperty); set => SetValue(CurrentPosMsProperty, value); }
        public bool IsPlayerRepeatOn { get => (bool)GetValue(IsPlayerRepeatOnProperty); set => SetValue(IsPlayerRepeatOnProperty, value); }
        public bool IsPlayerMuted { get => (bool)GetValue(IsPlayerMutedProperty); set => SetValue(IsPlayerMutedProperty, value); }
        public double PlayerVolume { get => (double)GetValue(PlayerVolumeProperty); set => SetValue(PlayerVolumeProperty, value); }
        public MediaContainerFormat SettingContainerFormat { get => (MediaContainerFormat)GetValue(SettingContainerFormatProperty); set => SetValue(SettingContainerFormatProperty, value); }
        public MediaVideoCodec SettingVideoCodec { get => (MediaVideoCodec)GetValue(SettingVideoCodecProperty); set => SetValue(SettingVideoCodecProperty, value); }
        public MediaAudioCodec SettingAudioCodec { get => (MediaAudioCodec)GetValue(SettingAudioCodecProperty); set => SetValue(SettingAudioCodecProperty, value); }

        private bool IsPlaying { get; set; }
        private DirectoryInfo CurrentDir { get; set; }
        private MediaTimeline MediaTimeline;
        private readonly Thread UpdateLoopThread;

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


        private readonly FfmpegInterface Ffmpeg = new FfmpegInterface();

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
            Closed += (sender, e) => { if (CurrentFile != null) { Markings.Save(CurrentFile); }; };

            // TODO: Solve this through bindings now
            //cFrom.ValueChanged += (sender, e) => { if (TimeSpan.FromMilliseconds(cPosition.Minimum) > Clock.CurrentTime.Value) { JumpTo(TimeSpan.FromMilliseconds(cPosition.Minimum)); } };
            // TODO: Solve this through bindings now
            //cTo.ValueChanged += (sender, e) => { if (Clock.CurrentTime.Value > TimeSpan.FromMilliseconds(cPosition.Maximum)) { JumpTo(TimeSpan.FromMilliseconds(SliceMaxMs)); } };
            // TODO: This is currently in conflict with the UpdateLoopThread which auto updates the value according to the current position, but in a separate thread so the value will be outdated because playback went on once the value is changed on the event thread and the change event triggers.
            //cPosition.ValueChanged += (sender, e) => JumpTo(TimeSpan.FromMilliseconds(cPosition.Value));

            var args = Environment.GetCommandLineArgs();
            if (args.Length == 2)
            {
                OpenFile(args[1]);
            }
            else if (args.Length > 2)
            {
                throw new ArgumentException("Invalid program arguments. Currently only accepts no or a single filepath parameter.");
            }
        }

        public void OpenFile(string fpath)
        {
            var newPath = new FileInfo(fpath);
            if (newPath?.FullName == CurrentFile?.FullName)
            {
                return;
            }
            if (CurrentFile != null)
            {
                Markings.Save(CurrentFile);
            }

            Console.WriteLine($"Opening file {fpath}");
            MediaTimeline = new MediaTimeline(new Uri(fpath))
            {
                RepeatBehavior = RepeatBehavior.Forever
            };
            cMediaElement.Clock = MediaTimeline.CreateClock();
            //cFileInfo.FilePath = new Uri(fpath);
            CurrentFile = new FileInfo(fpath);
            sFilename.Content = CurrentFile.Name;
            sFileSize.Content = CurrentFile.LengthAsHumanString();
            IsPlaying = true;

            if (CurrentDir?.FullName != CurrentFile.Directory?.FullName)
            {
                var dirFiles = GetVideoFiles();
                cFilesList.ItemsSource = dirFiles;
            }
            cFilesList.SelectedValue = CurrentFile.Name;

            UpdateWindowTitle();

            //Markings.Clear();
            //Markings.AddRange();
            Markings = MarkingCollection.LoadFor(new FileInfo(fpath));
            //cMarkingsList.Markings = Markings.LoadFor(fpath);
        }

        public void SetSlice(Marking marking = null)
        {
            SliceMinMs = marking?.StartMs ?? 0.0;
            SliceMaxMs = marking?.EndMs ?? cTo.Maximum;

            JumpTo(TimeSpan.FromMilliseconds(SliceMinMs));
        }

        private IOrderedEnumerable<string> GetVideoFiles()
        {
            CurrentDir = CurrentFile.Directory;
            var dirFiles = CurrentDir.GetFiles().Select(x => x.Name).Where(IsVideoFile).OrderBy(x => x);
            return dirFiles;
        }

        private static bool IsVideoFile(string filename) => filename.EndsWith(".mp4", StringComparison.InvariantCultureIgnoreCase) || filename.EndsWith(".mkv", StringComparison.InvariantCultureIgnoreCase);

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

        private int CurrentTimeMsPrecise => (int)(Clock.CurrentTime ?? TimeSpan.Zero).TotalMilliseconds;

        private void UpdateSeekers()
        {
            if (Clock == null)
            {
                return;
            }
            Dispatcher.Invoke(() => SetCurrentTime(CurrentTimeMsPrecise));
            Dispatcher.Invoke(() => { if (cPosition.Value >= SliceMaxMs) { if (IsPlayerRepeatOn) { JumpToStart(); } else { Stop(); } } });
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
        private static TimeSpan TimeSpanValueRangeLimited(TimeSpan value, TimeSpan min, TimeSpan max)
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
            var slice = new Marking
            {
                StartMs = (int)cFrom.Value,
                EndMs = (int)cTo.Value,
            };
            ExportSlice(slice);
        }

        private void ExportSlice(Marking slice)
        {
            Ffmpeg.ExportSlice(CurrentFile, slice, SettingContainerFormat, SettingVideoCodec, SettingAudioCodec);
        }

        private void CMarkingsList_Play(object sender, Controls.MarkingEventArgs e) { SetSlice(e.Marking); Play(); }
        private void CMarkingsList_Export(object sender, Controls.MarkingEventArgs e) => ExportSlice(e.Marking);
        // If new value is after end time, assume we just passed the end time and the user wanted to set the end time as the start time as well (possibly in preparation to change the end time afterwards).
        private void CMarkingsList_SetBegin(object sender, Controls.MarkingEventArgs e) => e.Marking.StartMs = CurrentTimeMsPrecise > e.Marking.EndMs ? e.Marking.EndMs : CurrentTimeMsPrecise;
        // If new value is before start time, expect the playback looped around from the end to the start, and use the end (slice max value) instead.
        private void CMarkingsList_SetEnd(object sender, Controls.MarkingEventArgs e) => e.Marking.EndMs = CurrentTimeMsPrecise < e.Marking.StartMs ? (IsPlayerRepeatOn ? (int)SliceMaxMs : e.Marking.StartMs) : CurrentTimeMsPrecise;
        private void BtnAddMarking_Click(object sender, RoutedEventArgs e) => Markings.Add(new Marking() { StartMs = (int)CurrentPosMs, EndMs = (int)CurrentPosMs, });
        private void BtnAddMarking5s_Click(object sender, RoutedEventArgs e) => Markings.Add(new Marking() { StartMs = (int)CurrentPosMs - 5000, EndMs = (int)CurrentPosMs, });
        private void BtnExportAllMarkings_Click(object sender, RoutedEventArgs e) { foreach (var item in Markings) { ExportSlice(item); } }

        public void Dispose()
        {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            Ffmpeg.Dispose();

            if (!disposing)
            {
                throw new InvalidOperationException();
            }
        }
    }
}
