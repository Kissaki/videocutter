using KCode.Videocutter.DataTypes;
using System;
using System.IO;

namespace KCode.Videocutter.ExternalInterfaces
{
    public class FfmpegArguments
    {
        private static class Arg
        {
            public static readonly string NeverOverwriteTarget = "-n";
            public static readonly string OverwriteTarget = "-y";
            public static readonly string StartTime = "-ss";
            public static readonly string EndTime = "-to";
            public static readonly string Duration = "-t";
        }

        private static string GetExtractFilepath(FileInfo sourceFilePath, int fromMs, int toMs) => sourceFilePath.FullName + $"_{fromMs:D}-{toMs:D}{sourceFilePath.Extension}";

        public FileInfo SourceFile { get; set; }
        public FileInfo TargetFile { get; set; }
        public TimeSpan? FromTime { get; set; }
        public TimeSpan? ToTime { get; set; }
        public TimeSpan? Duration { get; set; }
        public bool? OverwriteTargetFile { get; set; }
        public MediaContainerFormat ContainerFormat { get; set; }
        public MediaVideoCodec VideoCodec { get; set; }
        public MediaAudioCodec AudioCodec { get; set; }

        internal FfmpegArguments(FileInfo sourceFilePath, int fromMs, int toMs)
        {
            SourceFile = sourceFilePath;
            TargetFile = new FileInfo(GetExtractFilepath(sourceFilePath, fromMs, toMs));
            FromTime = TimeSpan.FromMilliseconds(fromMs);
            ToTime = TimeSpan.FromMilliseconds(toMs);
        }

        public override string ToString()
        {
            var overwriteFlag = OverwriteTargetFile ?? false ? Arg.OverwriteTarget : Arg.NeverOverwriteTarget;
            var fromS = TimeSpanToSeconds(FromTime);
            var toS = TimeSpanToSeconds(ToTime);
            var extractArguments = @$"{overwriteFlag} {Arg.StartTime} {fromS} {Arg.EndTime} {toS}";
            var targetArguments = GetTargetArguments();

            return @$"{extractArguments} -i ""{SourceFile.FullName}"" {targetArguments} ""{TargetFile.FullName}{GetContainerFileExtension()}""";
        }

        private object GetTargetArguments() => $"{GetVideoArguments()} {GetAudioArguments()} -map 0 {GetContainerFormatArguments()}";

        private string GetVideoArguments()
        {
            if (VideoCodec == MediaVideoCodec.FromSource)
            {
                return string.Empty;
            }
            return VideoCodec switch
            {
                MediaVideoCodec.AV1 => "-c:v libaom-av1 -strict experimental",
                MediaVideoCodec.VP9 => "-c:v libvpx-vp9",
                MediaVideoCodec.H264 => "-c:v libx264",
                MediaVideoCodec.H265 => "-c:v libx265",
                MediaVideoCodec.Copy => "-c:v copy",
                MediaVideoCodec.Drop => "-vn",
                _ => throw new NotImplementedException($"The ffmpeg parameter creation for this video codec is not implemented yet. {Enum.GetName(typeof(MediaVideoCodec), VideoCodec)}"),
            };
        }

        private string GetAudioArguments()
        {
            if (AudioCodec == MediaAudioCodec.FromSource)
            {
                return string.Empty;
            }
            return AudioCodec switch
            {
                MediaAudioCodec.AAC => "-c:a aac",
                MediaAudioCodec.Opus => "-c:a libopus",
                MediaAudioCodec.MP3 => "-c:a libmp3lame",
                MediaAudioCodec.Copy => "-c:a copy",
                MediaAudioCodec.Drop => "-an",
                _ => throw new NotImplementedException($"The ffmpeg parameter creation for this video codec is not implemented yet. {Enum.GetName(typeof(MediaVideoCodec), VideoCodec)}"),
            };
        }

        private string GetContainerFileExtension()
        {
            if (ContainerFormat == MediaContainerFormat.FromSource)
            {
                return string.Empty;
            }
            return ContainerFormat switch
            {
                MediaContainerFormat.MP4 => ".mp4",
                MediaContainerFormat.MKV => ".mkv",
                MediaContainerFormat.WEBM => ".webm",
                _ => throw new NotImplementedException($"The ffmpeg parameter creation for this container type is not implemented yet. {Enum.GetName(typeof(MediaContainerFormat), ContainerFormat)}"),
            };
        }

        private string GetContainerFormatArguments()
        {
            return ContainerFormat switch
            {
                MediaContainerFormat.MP4 => "-movflags +faststart",
                _ => string.Empty,
            };
        }

        public static string TimeSpanToSeconds(TimeSpan? ts)
        {
            var ms = ts.HasValue ? ts.Value.TotalMilliseconds / 1000 : 0;
            return FormattableString.Invariant($"{ms:0.000}");
        }
    }
}
