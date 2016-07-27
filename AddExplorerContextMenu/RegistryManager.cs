using Microsoft.Win32;

namespace AddExplorerContextMenu
{
    internal class RegistryManager
    {
        private const string KeyFolderName = "Indexer++";

        private const string FilterDirPathArgName = "filterDirPath";

        public void Register(string exePath, string iconPath)
        {
            using (RegistryKey shellKey = Registry.ClassesRoot.OpenSubKey("Directory\\shell", true))
            {
                if (shellKey != null)
                {
                    RegistryKey keyFolder = shellKey.CreateSubKey(KeyFolderName);
                    if (keyFolder != null)
                    {
                        keyFolder.SetValue("Icon", iconPath);
                        keyFolder.CreateSubKey("command")
                            .SetValue("", exePath + " " + FilterDirPathArgName + "=\"%1\"");
                    }
                }
            }
        }

        public void Unregister()
        {
            using (RegistryKey shellKey = Registry.ClassesRoot.OpenSubKey("Directory\\shell", true))
            {
                using (var subKey = shellKey.OpenSubKey(KeyFolderName))
                {
                    if (subKey != null)
                        shellKey.DeleteSubKeyTree(KeyFolderName);
                }
            }
        }
    }
}
