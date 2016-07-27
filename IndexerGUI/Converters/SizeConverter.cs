﻿using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Markup;

namespace Indexer
{
    [ValueConversion(typeof(ulong), typeof(string))]
    internal class SizeConverter : MarkupExtension, IValueConverter
    {
        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var formattedRes = System.Convert.ToInt32(value).ToString("##,#0 KB", CultureInfo.CurrentCulture);
            return formattedRes;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
