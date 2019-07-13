using KCode.Videocutter.DataTypes;
using System;
using System.Globalization;
using System.Windows.Data;

namespace KCode.Videocutter.Converter
{
    [ValueConversion(typeof(MediaContainerFormat), typeof(int))]
    class MediaContainerFormatValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (int)value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (MediaContainerFormat)value;
        }
    }
}
