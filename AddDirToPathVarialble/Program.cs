using System;
using System.IO;

namespace AddDirToPathVarialble
{
    class Program
    {
        private const string AddNewValueAction = "/a";
        private const string RemoveValueAction = "/r";

        private const string CurrentUserEnvironmentConfig = "/u";
        private const string LocalMachineEnvironmentConfig = "/m";

        /// <summary>
        /// Adds and removes directory from the PATH environment variable for current user.
        /// </summary>
        /// <param name="args">
        /// First argument is /a or /r (add and remove actions corespondingly).
        /// Second argument is the directory path.
        /// Third argument is an environment config target: current user or local machine: /u and /m correspondingly.
        /// </param>
        static void Main(string[] args)
        {
            if (args.Length != 3) throw new ArgumentException("The arguments number is incorrect.");

            // Get action (add or remove).

            var action = args[0];
            if(action != AddNewValueAction && action != RemoveValueAction)
                throw new ArgumentException("The action argument is not valid. Available options are /a and /r.");
            var addNewVal = (action == AddNewValueAction);


            // Get directory to alter the PATH enviroment variable.

            var installDirectory = args[1];
            if (!Directory.Exists(installDirectory))
                throw new DirectoryNotFoundException("Specified directory was not found.");
            var installDirectoryText = ";" + installDirectory;


            // Get environment config target.

            var configTarget = args[2];
            if(configTarget != CurrentUserEnvironmentConfig && configTarget != LocalMachineEnvironmentConfig)
                throw new ArgumentException(
                    "Environment config target is not valid. Available options are /u and /m.");
            
            var environmentConfigTarget = (args[2] == CurrentUserEnvironmentConfig
                ? EnvironmentVariableTarget.User
                : EnvironmentVariableTarget.Machine);

            var currentPathVal = Environment.GetEnvironmentVariable("PATH", environmentConfigTarget);

            if (String.IsNullOrEmpty(currentPathVal)) return;

            if (addNewVal)
            {
                // If already added - no action required.
                if(currentPathVal.IndexOf(installDirectoryText, StringComparison.CurrentCulture) != -1)
                    return;
                
                Environment.SetEnvironmentVariable("PATH", currentPathVal + installDirectoryText,
                    environmentConfigTarget);
            }
            else // Find existing (even when for some reason multiple were added) and remove from PATH:
            {
                var newPathVal = currentPathVal;
                while (true) // Remove all entries of the target directory from newPathVal.
                {
                    var startPos = newPathVal.IndexOf(installDirectoryText, StringComparison.CurrentCulture);
                    if(startPos == -1) break; // No more value found.

                    newPathVal = newPathVal.Remove(startPos, installDirectoryText.Length);
                }

                Environment.SetEnvironmentVariable("PATH", newPathVal, environmentConfigTarget);
            }
        }
    }
}
