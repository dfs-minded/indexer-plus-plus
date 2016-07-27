using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Markup;

namespace Indexer
{
    // Converts IconSizeEnum to corresponding integer value.
    internal class IconSizeToIntegerConverter : MarkupExtension, IValueConverter
    {
        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var size = (IconSizeEnum)value;
            switch (size)
            {
                case IconSizeEnum.SmallIcon16:  return 16;
                case IconSizeEnum.MediumIcon32: return 32;
                case IconSizeEnum.LargeIcon48:  return 48;
                case IconSizeEnum.JumboIcon256: return 256;
                default:                        return 16;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

}
