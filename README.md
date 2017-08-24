Windows Visual Studio build
---------------------------

1) download vulkan sdk
1) a) https://vulkan.lunarg.com/sdk/home
2) install vulkan sdk
3) ensure %VULKAN_SDK% environment variable is set correctly:
3) a) cmd.exe
3) b) dir /b %VULKAN_SDK%/Bin
3) c) dir /B %VULKAN_SDK%/Lib
3) d) should list executables and libraries for vulkan
4) visual studio solution (.sln) exists for vs2013
4) a) projects demonstrating various things are setup
5) press F6

Linux build
-----------

1) download vulkan sdk
1) a) https://vulkan.lunarg.com/sdk/home
2) follow instructions: https://vulkan.lunarg.com/doc/view/latest/linux/getting_started.html
3) ensure $VULKAN_SDK environment variable is set correctly
3) a) ls -1 $VULKAN_SDK/bin
3) b) ls -1 $VULKAN_SDK/lib
4) make all
5) run_parts *.exe
