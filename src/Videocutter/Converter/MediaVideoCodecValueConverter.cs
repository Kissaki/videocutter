using KCode.Videocutter.DataTypes;
using System;
using System.Globalization;
using System.Windows.Data;

namespace KCode.Videocutter.Converter
{
    [ValueConversion(typeof(MediaVideoCodec), typeof(int))]
    public class MediaVideoCodecValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (int)value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (MediaVideoCodec)value;
        }
    }
}
