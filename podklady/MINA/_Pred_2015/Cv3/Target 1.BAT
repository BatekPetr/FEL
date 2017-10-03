SET PATH=C:\Keil\ARM\BIN40\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\Microsoft SQL Server\90\Tools\binn\;D:\Keilc51v720\C51\Bin;C:\STR9\RIDE\Bin;C:\Programs\Palasm\Exe;C:\Program Files\Microsoft Network Monitor 3\;D:\altera\13.0sp1\quartus\bin;C:\Program Files\QuickTime\QTSystem\;C:\RIDE7_ARMTools\Ride\bin;C:\RIDE7_ARMTools\Ride\arm-gcc\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\Microsoft SQL Server\90\Tools\binn\;D:\Keilc51v720\C51\Bin;C:\STR9\RIDE\Bin;C:\Programs\Palasm\Exe;C:\Program Files\Microsoft Network Monitor 3\;D:\altera\13.0sp1\quartus\bin;C:\Program Files\QuickTime\QTSystem\;C:\RIDE7_ARMTools\Ride\bin;C:\RIDE7_ARMTools\Ride\arm-gcc\bin;C:\Cygnus\arm-tools\bin;C:\Cygnus\Bin;D:\altera\13.0sp1\modelsim_ase\win32aloem
SET ARMCC41INC=C:\Keil\ARM\RV31\INC
SET ARMCC41LIB=C:\Keil\ARM\RV31\LIB
SET CPU_TYPE=STM32F107VC
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x017D7840
"C:\Keil\ARM\BIN40\ArmAsm" --Via ".\_out\startup_stm32f10x_cl._ia"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\system_stm32f10x.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\core_cm3.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\misc.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_adc.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_bkp.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_can.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_cec.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_crc.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_dac.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_dbgmcu.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_dma.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_exti.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_flash.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_fsmc.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_gpio.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_i2c.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_iwdg.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_pwr.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_rcc.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_rtc.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_sdio.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_spi.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_tim.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_usart.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_wwdg.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\main.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via ".\_out\stm32f10x_it.__i"
"C:\Keil\ARM\BIN40\ArmLink" --Via ".\_Out\Test.lnp"
C:\Keil\ARM\BIN40\fromelf.exe ".\_Out\Test.axf" --i32combined --output ".\_Out\Test.hex"
