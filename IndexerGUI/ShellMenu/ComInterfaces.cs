using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Indexer
{
    public enum HResult
    {
        DragdropSCancel = 0x00040101,
        DragdropSDrop = 0x00040100,
        DragdropSUsedefaultcursors = 0x00040102,
        DataSSameformatetc = 0x00040130,
        Ok = 0,
        False = 1,
        ENointerface = unchecked((int) 0x80004002),
        ENotimpl = unchecked((int) 0x80004001),
        OleEAdvisenotsupported = 80040003,
        NoObject = unchecked((int) 0x800401E5),
        InvalidArguments = unchecked((int) 0x80070057),
        OutOfMemory = unchecked((int) 0x8007000E),
        Fail = unchecked((int) 0x80004005),
        ElementNotFound = unchecked((int) 0x80070490),
        TypeElementNotFound = unchecked((int) 0x8002802B),
        Win32ErrorCanceled = 1223,
        Canceled = unchecked((int) 0x800704C7),
        ResourceInUse = unchecked((int) 0x800700AA),
        AccessDenied = unchecked((int) 0x80030005)
    }

    [Flags]
    public enum CMF : uint
    {
        NORMAL = 0x00000000,
        DEFAULTONLY = 0x00000001,
        VERBSONLY = 0x00000002,
        EXPLORE = 0x00000004,
        NOVERBS = 0x00000008,
        CANRENAME = 0x00000010,
        NODEFAULT = 0x00000020,
        INCLUDESTATIC = 0x00000040,
        EXTENDEDVERBS = 0x00000100,
        RESERVED = 0xffff0000
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct CMINVOKECOMMANDINFO
    {
        public int cbSize;
        public int fMask;
        public IntPtr hwnd;
        public string lpVerb;
        public string lpParameters;
        public string lpDirectory;
        public int nShow;
        public int dwHotKey;
        public IntPtr hIcon;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct CMINVOKECOMMANDINFO_ByIndex
    {
        public int cbSize;
        public int fMask;
        public IntPtr hwnd;
        public int iVerb;
        public string lpParameters;
        public string lpDirectory;
        public int nShow;
        public int dwHotKey;
        public IntPtr hIcon;
    }

    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("000214e4-0000-0000-c000-000000000046")]
    public interface IContextMenu
    {
        [PreserveSig]
        HResult QueryContextMenu(IntPtr hMenu, uint indexMenu, int idCmdFirst, int idCmdLast, CMF uFlags);

        void InvokeCommand(ref CMINVOKECOMMANDINFO pici);

        [PreserveSig]
        HResult GetCommandString(int idcmd, uint uflags, int reserved,
            [MarshalAs(UnmanagedType.LPStr)] StringBuilder commandstring, int cch);
    }

    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("000214f4-0000-0000-c000-000000000046")]
    public interface IContextMenu2 : IContextMenu
    {
        [PreserveSig]
        new HResult QueryContextMenu(IntPtr hMenu, uint indexMenu, int idCmdFirst, int idCmdLast, CMF uFlags);

        void InvokeCommand(ref CMINVOKECOMMANDINFO_ByIndex pici);

        [PreserveSig]
        new HResult GetCommandString(int idcmd, uint uflags, int reserved,
            [MarshalAs(UnmanagedType.LPStr)] StringBuilder commandstring, int cch);

        [PreserveSig]
        HResult HandleMenuMsg(int uMsg, IntPtr wParam, IntPtr lParam);
    }

    public enum SIGDN : uint
    {
        NORMALDISPLAY = 0,
        PARENTRELATIVEPARSING = 0x80018001,
        PARENTRELATIVEFORADDRESSBAR = 0x8001c001,
        DESKTOPABSOLUTEPARSING = 0x80028000,
        PARENTRELATIVEEDITING = 0x80031001,
        DESKTOPABSOLUTEEDITING = 0x8004c000,
        FILESYSPATH = 0x80058000,
        URL = 0x80068000
    }

    [Flags]
    public enum SFGAO : uint
    {
        CANCOPY = 0x00000001,
        CANMOVE = 0x00000002,
        CANLINK = 0x00000004,
        STORAGE = 0x00000008,
        CANRENAME = 0x00000010,
        CANDELETE = 0x00000020,
        HASPROPSHEET = 0x00000040,
        DROPTARGET = 0x00000100,
        CAPABILITYMASK = 0x00000177,
        ENCRYPTED = 0x00002000,
        ISSLOW = 0x00004000,
        GHOSTED = 0x00008000,
        LINK = 0x00010000,
        SHARE = 0x00020000,
        READONLY = 0x00040000,
        HIDDEN = 0x00080000,
        DISPLAYATTRMASK = 0x000FC000,
        STREAM = 0x00400000,
        STORAGEANCESTOR = 0x00800000,
        VALIDATE = 0x01000000,
        REMOVABLE = 0x02000000,
        COMPRESSED = 0x04000000,
        BROWSABLE = 0x08000000,
        FILESYSANCESTOR = 0x10000000,
        FOLDER = 0x20000000,
        FILESYSTEM = 0x40000000,
        HASSUBFOLDER = 0x80000000,
        CONTENTSMASK = 0x80000000,
        STORAGECAPMASK = 0x70C50008
    }

    public enum SICHINT : uint
    {
        DISPLAY = 0x00000000,
        CANONICAL = 0x10000000,
        ALLFIELDS = 0x80000000
    }

    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("43826d1e-e718-42ee-bc55-a1e261c37bfe")]
    public interface IShellItem
    {
        IntPtr BindToHandler(IntPtr pbc, [MarshalAs(UnmanagedType.LPStruct)] Guid bhid,
            [MarshalAs(UnmanagedType.LPStruct)] Guid riid);

        [PreserveSig]
        HResult GetParent(out IShellItem ppsi);

        IntPtr GetDisplayName(SIGDN sigdnName);

        SFGAO GetAttributes(SFGAO sfgaoMask);

        int Compare(IShellItem psi, SICHINT hint);
    }

    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("000214F2-0000-0000-C000-000000000046")]
    public interface IEnumIDList
    {
        [PreserveSig]
        HResult Next(uint celt, out IntPtr rgelt, out uint pceltFetched);

        [PreserveSig]
        HResult Skip(uint celt);

        [PreserveSig]
        HResult Reset();

        IEnumIDList Clone();
    }

    [Flags]
    public enum SHCIDS : uint
    {
        ALLFIELDS = 0x80000000,
        CANONICALONLY = 0x10000000,
        BITMASK = 0xFFFF0000,
        COLUMNMASK = 0x0000FFFF
    }

    public enum SHCONTF
    {
        FOLDERS = 0x0020,
        NONFOLDERS = 0x0040,
        INCLUDEHIDDEN = 0x0080,
        INIT_ON_FIRST_NEXT = 0x0100,
        NETPRINTERSRCH = 0x0200,
        SHAREABLE = 0x0400,
        STORAGE = 0x0800
    }

    public enum SHGNO
    {
        NORMAL = 0x0000,
        INFOLDER = 0x0001,
        FOREDITING = 0x1000,
        FORADDRESSBAR = 0x4000,
        FORPARSING = 0x8000
    }

    [StructLayout(LayoutKind.Explicit, Size = 264)]
    public struct STRRET
    {
        [FieldOffset(0)] public uint uType;
        [FieldOffset(4)] public IntPtr pOleStr;
        [FieldOffset(4)] public IntPtr pStr;
        [FieldOffset(4)] public uint uOffset;
        [FieldOffset(4)] public IntPtr cStr;
    }

    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("000214E6-0000-0000-C000-000000000046")]
    public interface IShellFolder
    {
        void ParseDisplayName([In] IntPtr hwnd, [In] IntPtr pbc,
            [In, MarshalAs(UnmanagedType.LPWStr)] string pszDisplayName, [Out] out uint pchEaten,
            [Out] out IntPtr ppidl, [In, Out] ref uint pdwAttributes);

        [PreserveSig]
        HResult EnumObjects([In] IntPtr hwnd, [In] SHCONTF grfFlags, [Out] out IEnumIDList ppenumIDList);

        void BindToObject(IntPtr pidl, IntPtr pbc, [MarshalAs(UnmanagedType.LPStruct)] Guid riid, out IntPtr ppv);

        void BindToStorage(IntPtr pidl, IntPtr pbc, [MarshalAs(UnmanagedType.LPStruct)] Guid riid, out IntPtr ppv);

        [PreserveSig]
        short CompareIDs(SHCIDS lParam, IntPtr pidl1, IntPtr pidl2);

        IntPtr CreateViewObject(IntPtr hwndOwner, [MarshalAs(UnmanagedType.LPStruct)] Guid riid);

        void GetAttributesOf(uint cidl, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] IntPtr[] apidl,
            ref SFGAO rgfInOut);

        void GetUIObjectOf(IntPtr hwndOwner, uint cidl,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] IntPtr[] apidl,
            [MarshalAs(UnmanagedType.LPStruct)] Guid riid, uint rgfReserved, out IntPtr ppv);

        void GetDisplayNameOf(IntPtr pidl, SHGNO uFlags, out STRRET pName);

        void SetNameOf(IntPtr hwnd, IntPtr pidl, string pszName, SHCONTF uFlags, out IntPtr ppidlOut);
    }

    public enum Consts : uint
    {
        CMF_EXPLORE = 0x00000004,
        MFT_STRING = 0,
        MFS_ENABLED = 0
    }

    public static class ComHelpers
    {
        private static IntPtr SHGetIDListFromObject(IShellItem item)
        {
            // todo implement for XP
            return WinApiFunctions.SHGetIDListFromObject(item);
        }

        public static IntPtr GetID(IShellItem item)
        {
            var intermediateID = SHGetIDListFromObject(item);
            return WinApiFunctions.ILFindLastID(intermediateID);
        }
    }

    public class BHID
    {
        private static readonly Guid sfObject = new Guid("3981e224-f559-11d3-8e3a-00c04f6837d5");
        private static readonly Guid sfUIObject = new Guid("3981e225-f559-11d3-8e3a-00c04f6837d5");

        public static Guid SFObject
        {
            get { return sfObject; }
        }

        public static Guid SFUIObject
        {
            get { return sfUIObject; }
        }
    }
}
