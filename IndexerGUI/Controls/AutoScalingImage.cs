// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Indexer.Controls
{
    public class AutoScalingImage : Image
    {
        public AutoScalingImage()
        {
            SnapsToDevicePixels = true;

            SetStretch();

            Loaded += AutoScalingImage_Loaded;
        }

        private void AutoScalingImage_Loaded(object sender, RoutedEventArgs e)
        {
            Loaded -= AutoScalingImage_Loaded;
            SetStretch();
        }

        private void SetStretch()
        {
            if (Source != null && Source.Height < Height && Source.Width < Width)
            {
                Stretch = Stretch.None;
            }
            else
            {
                Stretch = Stretch.Uniform;
            }
        }
    }
}