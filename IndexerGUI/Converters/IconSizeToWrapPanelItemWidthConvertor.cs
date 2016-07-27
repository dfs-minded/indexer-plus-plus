using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Markup;

namespace Indexer
{   
    // Converts IconSizeEnum to corresponding integer wrap panel item width value.
    internal class IconSizeToWrapPanelItemWidthConvertor : MarkupExtension, IValueConverter
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
                case IconSizeEnum.SmallIcon16: return 236;
                case IconSizeEnum.MediumIcon32: return 72;
                case IconSizeEnum.LargeIcon48: return 108;
                case IconSizeEnum.JumboIcon256: return 270;
                default: return 236;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}