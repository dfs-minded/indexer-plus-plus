// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Indexer
{
    public static class Helper
    {
        private static readonly Dictionary<string, int> TextToByteMap = new Dictionary<string, int>();

        static Helper()
        {
            TextToByteMap.Add("KB", 1);
            TextToByteMap.Add("MB", TextToByteMap["KB"]*1024);
            TextToByteMap.Add("GB", TextToByteMap["MB"]*1024);
            TextToByteMap.Add("TB", TextToByteMap["GB"]*1024);
        }

        public static bool TryParseSize(string text, out int result)
        {
            result = 0;

            if (string.IsNullOrWhiteSpace(text)) return true;

            text = text.Trim();
            var multiplier = 1; // If nothing specified, default is KB.
            var shortName = string.Empty;

            if (text.Length > 1)
            {
                shortName = text[text.Length - 1].ToString();
                var prevSymbol = text[text.Length - 2].ToString();
                int num;

                // If not a space or number, like "3 KB", "3kb".
                if (!string.IsNullOrWhiteSpace(prevSymbol) && !int.TryParse(prevSymbol, out num))
                {
                    // Supposing it could be "3 KB" or "25 mb" etc.
                    shortName = prevSymbol + shortName;
                }
            }

            if (TextToByteMap.ContainsKey(shortName.ToUpper()))
            {
                multiplier = TextToByteMap[shortName.ToUpper()];
                text = text.Replace(shortName, string.Empty);
            }

            int size;
            if (!int.TryParse(text, out size))
                return false;

            result = size*multiplier;
            return true;
        }

        public static bool MakeIndexerMainWndVisible()
        {
            var mainWindow = Application.Current.MainWindow;
            if (mainWindow == null)
            {
                Log.Instance.Error("MakeIndexerMainWndVisible: Main window is null.");
                return false;
            }
            mainWindow.Visibility = Visibility.Visible;
            return true;
        }

        public static void SetMainWndDirPathFilter(string dirPath)
        {
            var wnd = (MainWindow) Application.Current.MainWindow;
            if (wnd == null || string.IsNullOrWhiteSpace(dirPath)) return;

            wnd.DirFilterEnabled = true;
            wnd.SearchDirPath = dirPath;
            wnd.FiltersVisibility = UserSettings.Instance.FiltersVisibility = Visibility.Visible;
        }

        public static void OpenNewIndexerWnd()
        {
            var main = new MainWindow();
            Application.Current.MainWindow = main;
            main.Show();
        }

        public static void ExitApplication()
        {
            Application.Current.Shutdown();
        }

        public static ImageSource ToImageSource(this Icon icon)
        {
            ImageSource imageSource = Imaging.CreateBitmapSourceFromHIcon(
                icon.Handle,
                Int32Rect.Empty,
                BitmapSizeOptions.FromEmptyOptions());

            return imageSource;
        }

        public static BitmapSource ToBitmapSource(this Icon icon)
        {
            return icon.ToBitmap().ToBitmapSource();
        }

        public static BitmapSource ToBitmapSource(this Bitmap bmp)
        {
            if (bmp == null) return null;

            var handle = bmp.GetHbitmap();
            BitmapSource source;
            try
            {
                source = Imaging.CreateBitmapSourceFromHBitmap(handle, IntPtr.Zero, Int32Rect.Empty,
                    BitmapSizeOptions.FromEmptyOptions());
            }
            finally
            {
                WinApiFunctions.DeleteObject(handle);
            }

            return source;
        }


        [DllImport("shell32.dll")]
        public static extern bool IsUserAnAdmin();

        public static string GetNamePrefix()
        {
            var userName = WindowsIdentity.GetCurrent().Name.Replace("\\", "-");
            return @"Local\" + userName + "-Indexer++Pipe";
        }
    }
}