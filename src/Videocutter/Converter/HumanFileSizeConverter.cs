using System;
using System.Globalization;
using System.Windows.Data;

namespace KCode.Videocutter.Converter
{
    [ValueConversion(typeof(long), typeof(string))]
    public class HumanFileSizeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ClassExtensions.FormatFileSize((long)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
