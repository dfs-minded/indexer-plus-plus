// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using System.Windows.Media.Imaging;

namespace Indexer
{
    internal static class TrayIconManager
    {
        private static NotifyIcon trayIcon;
        private static ContextMenu trayMenu;

        public static void CreateIcon()
        {
            try
            {
                trayMenu = new ContextMenu();
                // trayMenu.MenuItems.Add("New Search Window", OnNewWindow);
                trayMenu.MenuItems.Add("Exit", OnExit);

                trayIcon = new NotifyIcon();
                trayIcon.Text = "Indexer++";

                var iconUri = new Uri("pack://application:,,,/Icons/icon_white_bg.ico", UriKind.RelativeOrAbsolute);
                // Uri iconUri = new Uri("pack://application:,,,/icon_v3_3.ico", UriKind.RelativeOrAbsolute);          

                var wpfIcon = BitmapFrame.Create(iconUri);
                trayIcon.Icon = ConvertFromBitmapFrame(wpfIcon);
                trayIcon.ContextMenu = trayMenu;
                trayIcon.Visible = true;
                trayIcon.DoubleClick += RestoreMainWnd;
            }

            catch (Exception ex)
            {
                Log.Instance.Error("TrayIconManager: Cannot create icon." + ex.Message);
            }
        }

        private static Icon ConvertFromBitmapFrame(BitmapFrame bitmapFrame)
        {
            // copy paste from Internet
            var ms = new MemoryStream();
            var encoder = new PngBitmapEncoder();
            encoder.Frames.Add(bitmapFrame);
            encoder.Save(ms);
            ms.Seek(0, SeekOrigin.Begin);
            var bmp = new Bitmap(ms);
            return Icon.FromHandle(bmp.GetHicon());
        }

        public static void Dispose()
        {
            if (trayIcon == null) return;
            trayIcon.DoubleClick -= RestoreMainWnd;
            trayIcon.Visible = false;
            trayIcon.Dispose();
            trayIcon = null;
        }

        private static void OnExit(object sender, EventArgs e)
        {
            Dispose();
            Helper.ExitApplication();
        }

        private static void RestoreMainWnd(object sender, EventArgs e)
        {
            Helper.MakeIndexerMainWndVisible();
        }
    }
}
