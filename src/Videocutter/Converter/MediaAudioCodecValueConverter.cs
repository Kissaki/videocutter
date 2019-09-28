using KCode.Videocutter.DataTypes;
using System;
using System.Globalization;
using System.Windows.Data;

namespace KCode.Videocutter.Converter
{
    [ValueConversion(typeof(MediaAudioCodec), typeof(int))]
    public class MediaAudioCodecValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (int)value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (MediaAudioCodec)value;
        }
    }
}
