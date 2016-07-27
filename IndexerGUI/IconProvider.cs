// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;
using CLIInterop;
using Shell;

namespace Shell
{
    public struct SHFILEINFO
    {
        public IntPtr hIcon;
        public int iIcon;
        public uint dwAttributes;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 254)] public string szDisplayName;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)] public string szTypeName;
    }

    public struct IMAGELISTDRAWPARAMS
    {
        public int cbSize;
        public IntPtr himl;
        public int i;
        public IntPtr hdcDst;
        public int x;
        public int y;
        public int cx;
        public int cy;
        public int xBitmap;
        public int yBitmap;
        public int rgbBk;
        public int rgbFg;
        public int fStyle;
        public int dwRop;
        public int fState;
        public int Frame;
        public int crEffect;
    }

    [ComImport]
    [Guid("46EB5926-582E-4017-9FDF-E8998DAA0950")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IImageList
    {
        [PreserveSig]
        int Add(IntPtr hbmImage, IntPtr hbmMask, ref int pi);

        [PreserveSig]
        int ReplaceIcon(int i, IntPtr hicon, ref int pi);

        [PreserveSig]
        int SetOverlayImage(int iImage, int iOverlay);

        [PreserveSig]
        int Replace(int i, IntPtr hbmImage, IntPtr hbmMask);

        [PreserveSig]
        int AddMasked(IntPtr hbmImage, int crMask, ref int pi);

        [PreserveSig]
        int Draw(ref IMAGELISTDRAWPARAMS pimldp);

        [PreserveSig]
        int Remove(int i);

        [PreserveSig]
        int GetIcon(int i, int flags, ref IntPtr picon);
    }
}

namespace Indexer
{
    public class IconProvider : IIconProvider
    {
        #region WinApi functions import.

        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(IntPtr handle, int Msg, IntPtr wParam, IntPtr lParam);

        [DllImport("shell32.dll")]
        public static extern int SHGetImageList(int iImageList, ref Guid riid, out IImageList ppv);

        [DllImport("shell32.dll")]
        public static extern int SHGetFileInfo(string pszPath, int dwFileAttributes, ref SHFILEINFO psfi,
            int cbFileInfo, uint uFlags);

        [DllImport("user32.dll")]
        public static extern int DestroyIcon(IntPtr hIcon);

        #endregion

        private const int WM_CLOSE = 0x0010;
        private const uint SHGFI_SYSICONINDEX = 0x4000;
        private const int FILE_ATTRIBUTE_NORMAL = 0x80;
        private const uint SHGFI_ICON = 0x000000100;
        private const uint SHGFI_USEFILEATTRIBUTES = 0x000000010;
        private const int FILE_ATTRIBUTE_DIRECTORY = 0x00000010;


        private static IconProvider instance;

        public static IconProvider Instance
        {
            get { return instance ?? (instance = new IconProvider()); }
        }

        private readonly Dictionary<IconSizeEnum, BitmapSource> FolderIcon;
        private readonly Dictionary<string, BitmapSource> SmallIcons;
        private readonly Dictionary<string, BitmapSource> MediumIcons;
        private readonly Dictionary<string, BitmapSource> LargeIcons;
        private readonly Dictionary<string, BitmapSource> JumboIcons;

        private readonly HashSet<string> DoNotCache;

        public IconSizeEnum IconSize = IconSizeEnum.SmallIcon16;

        private IconProvider()
        {
            FolderIcon = new Dictionary<IconSizeEnum, BitmapSource>();
            SmallIcons = new Dictionary<string, BitmapSource>();
            MediumIcons = new Dictionary<string, BitmapSource>();
            LargeIcons = new Dictionary<string, BitmapSource>();
            JumboIcons = new Dictionary<string, BitmapSource>();

            DoNotCache = new HashSet<string>
            {
                ".exe",
                ".lnk"
            };
        }

        public BitmapSource GetIcon(string filename)
        {
            if (Directory.Exists(filename))
            {
                if (!FolderIcon.ContainsKey(IconSize))
                {
                    FolderIcon[IconSize] = GetFolderIcon(filename);
                }

                return FolderIcon[IconSize];
            }

            if (!File.Exists(filename)) return null;

            var extension = Path.GetExtension(filename) ?? string.Empty;
            BitmapSource icon = null;

            switch (IconSize)
            {
                case IconSizeEnum.SmallIcon16:
                    if (SmallIcons.ContainsKey(extension))
                        return SmallIcons[extension];

                    icon = GetIconFromFilePath(filename);
                    if (!DoNotCache.Contains(extension))
                        SmallIcons[extension] = icon;

                    break;
                case IconSizeEnum.MediumIcon32:
                    if (!MediumIcons.ContainsKey(extension))
                    {
                        MediumIcons[extension] = GetIconFromFilePath(filename);
                    }
                    return MediumIcons[extension];
                case IconSizeEnum.LargeIcon48:
                    if (!LargeIcons.ContainsKey(extension))
                    {
                        LargeIcons[extension] = GetIconFromFilePath(filename);
                    }
                    return LargeIcons[extension];
                case IconSizeEnum.JumboIcon256:
                    if (!JumboIcons.ContainsKey(extension))
                    {
                        JumboIcons[extension] = GetIconFromFilePath(filename);
                    }
                    return JumboIcons[extension];
            }

            return icon;
        }

        private int GetIconSize()
        {
            switch (IconSize)
            {
                case IconSizeEnum.SmallIcon16:
                    return 0x1;
                case IconSizeEnum.MediumIcon32:
                    return 0x0;
                case IconSizeEnum.LargeIcon48:
                    return 0x2;
                case IconSizeEnum.JumboIcon256:
                    return 0x4;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        private BitmapSource GetIconFromFilePath(string filepath)
        {
            var handle = GetIconHandleFromFilePathWithFlags(filepath, FILE_ATTRIBUTE_NORMAL, SHGFI_SYSICONINDEX);

            return GetBitmapSourceFromIconHandle(handle);
        }

        private BitmapSource GetFolderIcon(string folderpath)
        {
            var flags = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;

            var handle = GetIconHandleFromFilePathWithFlags(folderpath, FILE_ATTRIBUTE_DIRECTORY, flags);

            return GetBitmapSourceFromIconHandle(handle);
        }

        private IntPtr GetIconHandleFromFilePathWithFlags(string filepath, int fileAttributeFlag, uint flags)
        {
            var info = new SHFILEINFO();

            var retval = SHGetFileInfo(filepath, fileAttributeFlag, ref info, Marshal.SizeOf(info), flags);
            if (retval == 0) return IntPtr.Zero;

            var iconIndex = info.iIcon;
            var iImageListGuid = new Guid("46EB5926-582E-4017-9FDF-E8998DAA0950");
            IImageList iml;

            SHGetImageList(GetIconSize(), ref iImageListGuid, out iml);

            var hIcon = IntPtr.Zero;
            const int ILD_TRANSPARENT = 1;
            iml.GetIcon(iconIndex, ILD_TRANSPARENT, ref hIcon);
            return hIcon;
        }

        private BitmapSource GetBitmapSourceFromIconHandle(IntPtr hIcon)
        {
            if (hIcon == IntPtr.Zero)
            {
                SendMessage(hIcon, WM_CLOSE, IntPtr.Zero, IntPtr.Zero);
                return null;
            }

            var myIcon = Icon.FromHandle(hIcon);
            var bitmapSource = myIcon.ToBitmapSource();
            myIcon.Dispose();
            DestroyIcon(hIcon);
            SendMessage(hIcon, WM_CLOSE, IntPtr.Zero, IntPtr.Zero);
            return bitmapSource;
        }
    }
}