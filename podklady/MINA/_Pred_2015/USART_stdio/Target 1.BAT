SET PATH=C:\Keil\ARM\BIN40\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\Microsoft SQL Server\90\Tools\binn\;D:\Keilc51v720\C51\Bin;C:\STR9\RIDE\Bin;c:\str9\gnuarm\bin;C:\Programs\Palasm\Exe;C:\Program Files\Microsoft Network Monitor 3\;%QUARTUS_ROOTDIR%\bin;C:\Program Files\QuickTime\QTSystem\;C:\RIDE7_ARMTools\Ride\bin;C:\RIDE7_ARMTools\Ride\arm-gcc\bin;C:\altera\71sp1\modelsim_ae\win32aloem;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\Microsoft SQL Server\90\Tools\binn\;D:\Keilc51v720\C51\Bin;C:\STR9\RIDE\Bin;c:\str9\gnuarm\bin;C:\Programs\Palasm\Exe;C:\Program Files\Microsoft Network Monitor 3\;%QUARTUS_ROOTDIR%\bin;C:\Program Files\QuickTime\QTSystem\;C:\RIDE7_ARMTools\Ride\bin;C:\RIDE7_ARMTools\Ride\arm-gcc\bin;C:\Cygnus\arm-tools\bin;C:\Cygnus\Bin
SET ARMCC41INC=C:\Keil\ARM\RV31\INC
SET ARMCC41LIB=C:\Keil\ARM\RV31\LIB
SET CPU_TYPE=STM32F107VC
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x017D7840
"C:\Keil\ARM\BIN40\ArmAsm" --Via ".\_out\startup_stm32f10x_cl._ia"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\system_stm32f10x.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\core_cm3.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_gpio.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_rcc.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_usart.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\main.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_it.__i"
"C:\Keil\ARM\BIN40\ArmLink" --Via ".\_Out\Test.lnp"
C:\Keil\ARM\BIN40\fromelf.exe ".\_Out\Test.axf" --i32combined --output ".\_Out\Test.hex"
