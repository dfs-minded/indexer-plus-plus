using System;
using System.Runtime.InteropServices;

namespace Indexer
{
    internal class WinApiFunctions
    {
        [DllImport("shell32.dll", CharSet = CharSet.Unicode, PreserveSig = false)]
        public static extern IShellItem SHCreateItemFromParsingName([In] string path, [In] IntPtr pbc,
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid riid);

        [DllImport("shell32.dll", PreserveSig = false)]
        public static extern IntPtr SHGetIDListFromObject([In, MarshalAs(UnmanagedType.IUnknown)] object punk);

        [DllImport("shell32.dll", EntryPoint = "#16")]
        public static extern IntPtr ILFindLastID(IntPtr pidl);

        [DllImport("user32.dll")]
        public static extern int TrackPopupMenuEx(IntPtr hmenu, TPM fuFlags, int x, int y, IntPtr hwnd, IntPtr lptpm);

        [DllImport("user32.dll")]
        public static extern int GetMenuItemCount(IntPtr hMenu);

        [DllImport("user32.dll")]
        public static extern bool GetMenuItemInfo(IntPtr hMenu, int uItem, bool fByPosition, ref MENUITEMINFO lpmii);

        [DllImport("user32.dll")]
        public static extern bool InsertMenuItem(IntPtr hMenu, int uItem, bool fByPosition, ref MENUITEMINFO lpmii);

        [DllImport("gdi32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool DeleteObject(IntPtr hObject);
    }

    [Flags]
    public enum TPM
    {
        TPM_LEFTBUTTON = 0x0000,
        TPM_RIGHTBUTTON = 0x0002,
        TPM_LEFTALIGN = 0x0000,
        TPM_CENTERALIGN = 0x000,
        TPM_RIGHTALIGN = 0x000,
        TPM_TOPALIGN = 0x0000,
        TPM_VCENTERALIGN = 0x0010,
        TPM_BOTTOMALIGN = 0x0020,
        TPM_HORIZONTAL = 0x0000,
        TPM_VERTICAL = 0x0040,
        TPM_NONOTIFY = 0x0080,
        TPM_RETURNCMD = 0x0100,
        TPM_RECURSE = 0x0001,
        TPM_HORPOSANIMATION = 0x0400,
        TPM_HORNEGANIMATION = 0x0800,
        TPM_VERPOSANIMATION = 0x1000,
        TPM_VERNEGANIMATION = 0x2000,
        TPM_NOANIMATION = 0x4000,
        TPM_LAYOUTRTL = 0x8000
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct MENUITEMINFO
    {
        public int cbSize;
        public MIIM fMask;
        public uint fType;
        public uint fState;
        public int wID;
        public IntPtr hSubMenu;
        public int hbmpChecked;
        public int hbmpUnchecked;
        public int dwItemData;
        public string dwTypeData;
        public uint cch;
        public int hbmpItem;
    }

    [Flags]
    public enum MIIM : uint
    {
        MIIM_STATE = 0x00000001,
        MIIM_ID = 0x00000002,
        MIIM_SUBMENU = 0x00000004,
        MIIM_CHECKMARKS = 0x00000008,
        MIIM_TYPE = 0x00000010,
        MIIM_DATA = 0x00000020,
        MIIM_STRING = 0x00000040,
        MIIM_BITMAP = 0x00000080,
        MIIM_FTYPE = 0x00000100
    }
}