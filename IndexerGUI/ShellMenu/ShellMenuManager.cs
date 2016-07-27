// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Windows.Threading;

namespace Indexer
{
    internal class ShellMenuItem
    {
        public readonly string Text;
        public readonly int Index;
        public readonly Action Action;

        public ShellMenuItem(string text, int index, Action action)
        {
            Text = text;
            Index = index;
            Action = action;
        }
    }

    internal class ShellMenuManager
    {
        private const int FirstCmd = 0x8000;
        private readonly Control control;
        private readonly string filename;
        private readonly Point pos;
        private readonly List<ShellMenuItem> newItems;
        private readonly List<ShellMenuItem> replaceItems;
        private readonly Dictionary<int, Action> actions = new Dictionary<int, Action>();

        public ShellMenuManager(Control control, string filename, Point pos, List<ShellMenuItem> newItems,
            List<ShellMenuItem> replaceItems)
        {
            this.control = control;
            this.filename = filename;
            this.pos = control.PointToScreen(pos);
            this.newItems = (newItems ?? new List<ShellMenuItem>()).OrderBy(item => item.Index).ToList();
            this.replaceItems = replaceItems ?? new List<ShellMenuItem>();
        }

        // This function returns after the user canceled the menu or chose a menu item.
        public void Show()
        {
            var contextMenu = CreateShellMenu(filename);
            if (contextMenu == null) return;

            try
            {
                using (var menu = new ContextMenu())
                {
                    contextMenu.QueryContextMenu(menu.Handle, 0, FirstCmd, int.MaxValue, (CMF) Consts.CMF_EXPLORE);
                    actions.Clear();

                    SetReplaceItems(menu);
                    SetNewItems(menu);

                    var wnd = new Form();
                    Action showMenu = () =>
                    {
                        var command = WinApiFunctions.TrackPopupMenuEx(menu.Handle, TPM.TPM_RETURNCMD, pos.X, pos.Y,
                            wnd.Handle, IntPtr.Zero);
                        DispatchCommand(contextMenu, command);
                    };
                    if (Dispatcher.CurrentDispatcher.CheckAccess())
                        showMenu();
                    else
                        Dispatcher.CurrentDispatcher.BeginInvoke(showMenu);
                }
            }
            catch (Exception ex)
            {
                Log.Instance.Error("ShellMenuManager: " + ex.Message);
            }
        }

        private void SetReplaceItems(ContextMenu menu)
        {
            foreach (var item in replaceItems)
            {
                var count = WinApiFunctions.GetMenuItemCount(menu.Handle);
                var index = item.Index < 0 ? count + item.Index : item.Index;

                var itemInfo = new MENUITEMINFO();
                itemInfo.cbSize = Marshal.SizeOf(itemInfo);
                itemInfo.fMask = MIIM.MIIM_ID | MIIM.MIIM_SUBMENU;

                if (WinApiFunctions.GetMenuItemInfo(menu.Handle, index, true, ref itemInfo))
                    actions[itemInfo.wID] = item.Action;
            }
        }

        private void SetNewItems(ContextMenu menu)
        {
            foreach (var item in newItems)
            {
                var newItemInfo = new MENUITEMINFO();
                newItemInfo.cbSize = Marshal.SizeOf(newItemInfo);
                newItemInfo.fMask = MIIM.MIIM_CHECKMARKS | MIIM.MIIM_FTYPE | MIIM.MIIM_ID | MIIM.MIIM_STATE |
                                    MIIM.MIIM_STRING;
                newItemInfo.fType = (uint) Consts.MFT_STRING;
                newItemInfo.fState = (uint) Consts.MFS_ENABLED;
                newItemInfo.wID = actions.Count + 1;
                actions[newItemInfo.wID] = item.Action;
                //  newItemInfo.hbmpChecked = hBitmap;
                //  newItemInfo.hbmpUnchecked = hBitmap;
                newItemInfo.dwTypeData = item.Text;
                newItemInfo.cch = 8;
                WinApiFunctions.InsertMenuItem(menu.Handle, item.Index, true, ref newItemInfo);
            }
        }

        private void DispatchCommand(IContextMenu contextMenu, int command)
        {
            if (command == 0) return;

            if (actions.ContainsKey(command))
            {
                actions[command]();
                return;
            }
            var invoke = new CMINVOKECOMMANDINFO_ByIndex();
            invoke.cbSize = Marshal.SizeOf(invoke);
            invoke.iVerb = command - FirstCmd;
            invoke.nShow = 1 /*SW_SHOWNORMAL*/;
            var contextMenu2 = contextMenu as IContextMenu2;
            try
            {
                contextMenu2.InvokeCommand(ref invoke);
            }
            catch (COMException)
            {
                // Invocation of some commands raises exceptions. The base we can do is to catch them.
            }
        }

        private static IContextMenu CreateShellMenu(string fileName)
        {
            var shellItem = CreateShellItem(fileName);
            if (shellItem == null) return null;

            var shellFolder = CreateShellFolder(shellItem);
            return CreateContextMenu(shellItem, shellFolder);
        }

        private static IContextMenu CreateContextMenu(IShellItem shellItem, IShellFolder shellFolder)
        {
            var ids = new IntPtr[1] {ComHelpers.GetID(shellItem)};

            IntPtr res;
            shellFolder.GetUIObjectOf(IntPtr.Zero,
                1, ids,
                typeof (IContextMenu).GUID, 0, out res);
            return (IContextMenu) Marshal.GetTypedObjectForIUnknown(res, typeof (IContextMenu));
        }

        private static IShellFolder CreateShellFolder(IShellItem shellItem)
        {
            IShellItem parentShellItem;

            shellItem.GetParent(out parentShellItem);

            var result = parentShellItem.BindToHandler(IntPtr.Zero, BHID.SFObject, typeof (IShellFolder).GUID);

            return (IShellFolder) Marshal.GetTypedObjectForIUnknown(result, typeof (IShellFolder));
        }

        private static IShellItem CreateShellItem(string filename)
        {
            try
            {
                return WinApiFunctions.SHCreateItemFromParsingName(filename, IntPtr.Zero, typeof (IShellItem).GUID);
            }
            catch (ArgumentException)
            {
                return null;
            }
            // For system files like $MFT, $Boot etc.
            catch (UnauthorizedAccessException)
            {
                return null;
            }
            catch (FileNotFoundException)
            {
                return null;
            }
        }
    }
}
