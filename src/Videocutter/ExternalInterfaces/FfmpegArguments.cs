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

        public FfmpegArguments(FileInfo sourceFilePath, int fromMs, int toMs)
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

        private object GetTargetArguments() => $"{GetVideoArguments()} {GetAudioArguments()} {GetContainerFormatArguments()}";

        private string GetVideoArguments()
        {
            if (VideoCodec == MediaVideoCodec.FromSource)
            {
                return string.Empty;
            }
            switch (VideoCodec)
            {
                case MediaVideoCodec.AV1:
                    return "-c:v libaom-av1 -strict experimental";
                case MediaVideoCodec.VP9:
                    return "-c:v libvpx-vp9";
                case MediaVideoCodec.H264:
                    return "-c:v libx264";
                case MediaVideoCodec.H265:
                    return "-c:v libx265";
                case MediaVideoCodec.Copy:
                    return "-c:v copy";
                case MediaVideoCodec.Drop:
                    return "-vn";
                default:
                    throw new NotImplementedException($"The ffmpeg parameter creation for this video codec is not implemented yet. {Enum.GetName(typeof(MediaVideoCodec), VideoCodec)}");
            }
        }

        private string GetAudioArguments()
        {
            if (AudioCodec == MediaAudioCodec.FromSource)
            {
                return string.Empty;
            }
            switch (AudioCodec)
            {
                case MediaAudioCodec.AAC:
                    return "-c:a aac";
                case MediaAudioCodec.Opus:
                    return "-c:a libopus";
                case MediaAudioCodec.MP3:
                    return "-c:a libmp3lame";
                case MediaAudioCodec.Copy:
                    return "-c:a copy";
                case MediaAudioCodec.Drop:
                    return "-an";
                default:
                    throw new NotImplementedException($"The ffmpeg parameter creation for this video codec is not implemented yet. {Enum.GetName(typeof(MediaVideoCodec), VideoCodec)}");
            }
        }

        private string GetContainerFileExtension()
        {
            if (ContainerFormat == MediaContainerFormat.FromSource)
            {
                return string.Empty;
            }
            switch (ContainerFormat)
            {
                case MediaContainerFormat.MP4:
                    return ".mp4";
                case MediaContainerFormat.MKV:
                    return ".mkv";
                case MediaContainerFormat.WEBM:
                    return ".webm";
                default:
                    throw new NotImplementedException($"The ffmpeg parameter creation for this container type is not implemented yet. {Enum.GetName(typeof(MediaContainerFormat), ContainerFormat)}");
            }
        }

        private string GetContainerFormatArguments()
        {
            switch (ContainerFormat)
            {
                case MediaContainerFormat.MP4:
                    return "-movflags +faststart";
                default:
                    return string.Empty;
            }
        }

        public static string TimeSpanToSeconds(TimeSpan? ts)
        {
            var ms = ts.HasValue ? ts.Value.TotalMilliseconds / 1000 : 0;
            return FormattableString.Invariant($"{ms:0.000}");
        }
    }
}
