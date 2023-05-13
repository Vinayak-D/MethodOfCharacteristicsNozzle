# MethodOfCharacteristicsNozzle
A custom GUI app for rocket engine design using the Method of Characteristics in C++

Dependancies:
- C++
- [Qt Creator](https://www.qt.io/download)

Latest Updates:
- Created with mac OS Ventura (Qt 6.4.0 (Clang 13.0, arm64) and tested and updated on Windows 10 (Qt 6.3.1 MinGW 64-bit).
- You can see the configuration within rocketDesignApp.pro.user

After downloading:
- Go to methodofcharacteristics.cpp, change line 198 to the desired folder on your local machine, to output the text file containing all the nozzle points.

FAQ:
- If you are unable to open rocketDesignApp.pro from QtCreator ("no valid settings" error), simply delete **rocketDesignApp.pro.user**, and then reopen the .pro file, you will need to configure again and a fresh new ".pro.user" file one will be created on your local machine. See [this](https://forum.qt.io/topic/132250/trying-to-import-qt-creator-project-from-previous-version/6) for more information.
- If you get the error file qcustomplot.h not found in ui_methodofcharacteristics.h, click on the file and in the line, change it to src/qcustomplot (be sure to add the "src").
