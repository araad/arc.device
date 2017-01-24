IF NOT DEFINED ArcOutputDir (setx ArcOutputDir %cd%\BUILD\NUCLEO_F401RE\GCC_ARM\)
IF NOT EXIST %cd%\mbed-os (mbed deploy)