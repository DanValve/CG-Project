Computer Graphics Project 
Student:丹尼尔
StudentID:1820212018
Important:
*If you dont want to build the project, you can find  
the application at:
- Executable folder 
OR at
- bin/windows-Release-x86_64/Examples/Examples.exe (deleted by default so save space)
*Do not move the executable to another folder since it has resource
in other folders!!!!!!!!!!!!!!!!!
How to build?
*First Generate the solution and project files
If your system is Windows and you have visual studio 2022:
-just execute Project Generator.bat
Other Version of Visual studio?:
*Open the file with a text editor and change the script
FROM call "%CD%"\vendor\bin\premake\premake5.exe vs2022
TO call "%CD%"\vendor\bin\premake\premake5.exe vs{version}
*If everything went well, the necessary solution and project files will be created
Then you can use visual studio to build the application.
NOTE:
If you delete the the Executable folder, but you are able to build the project using the method above,
then all the necessary files and folders will be created upon building the application with VS