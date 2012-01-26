# Building
## Prerequisites

1. Download and install _HTML Help Workshop_ from http://msdn.microsoft.com/en-us/library/ms669985(VS.85).aspx.
1. Download and install _InnoSetup's ISPack_ from http://innosetup.com/.
1. Download the latest snapshot of the _JamPlus_ binaries from http://jamplus.org. Unzip it into a directory.
1. Download _Wget_. One location to grab it from is http://users.ugent.be/~bpuype/wget/. Put the `wget.exe` in your PATH.
1. Download _unzip_ from http://www.info-zip.org/UnZip.html. Put the `unzip.exe` in your `PATH`.


## Building Workspace Whiz

1. Run the `CreateJamVS2010Workspace.bat` (or `CreateJamVS2008Workspace.bat` or `CreateJamVC6Workspace.bat`) script in the **Workspace Whiz** main directory after putting the `jamplus\bin\ntx86` directory in your `PATH` (or modifying the `CreateJamVS2010Workspace.bat` script to point to that directory).
1. Navigate to `build2010\_workspace.vs2010_\` and open `WorkspaceWhiz.sln` within Visual Studio 2010.
1. Build the solution.


## Building the Workspace Whiz installer

1. Build the **Installer** project in the solution.


# Debugging

It takes two instances of Visual Studio in order to debug Workspace Whiz. The first instance contains the Workspace Whiz solution, and it must not have the Workspace Whiz add-in active. The second instance is launched by the first instance and will be running Workspace Whiz.

Before launching the first instance of Visual Studio, run the following at the command line:

    [d:\workspacewhiz]cd build2010\win32-release\image
    [d:\workspacewhiz\build2010\win32-release\image]regsvr32 /u WorkspaceWhizD.dll
    [d:\workspacewhiz\build2010\win32-release\image]regsvr32 /u WorkspaceWhiz.dll

1. Launch Visual Studio.
1. Load the Workspace Whiz solution.
1. Build the solution. It will automatically register Workspace Whiz as an add-in within Visual Studio.
1. Right click on the **WorkspaceWhiz** project in the _Solution Explorer_. Choose **Properties**.
1. In the _Debugging_ tab, choose **Command** and press the down arrow. Choose **Browse**. Browse for `C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe`. Click **OK**.
1. Debug.

Building the solution will automatically register the Workspace Whiz add-in with Visual Studio.




