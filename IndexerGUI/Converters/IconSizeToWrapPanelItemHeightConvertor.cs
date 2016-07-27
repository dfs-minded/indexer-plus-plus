// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Markup;

namespace Indexer
{
    // Converts IconSizeEnum to corresponding integer wrap panel item height value.
    internal class IconSizeToWrapPanelItemHeightConvertor : MarkupExtension, IValueConverter
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
                case IconSizeEnum.SmallIcon16: return 24;
                case IconSizeEnum.MediumIcon32: return 80;
                case IconSizeEnum.LargeIcon48: return 122;
                case IconSizeEnum.JumboIcon256: return 270;
                default: return 24;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
