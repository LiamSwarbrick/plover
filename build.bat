@echo off

SET EXENAME=plover

PUSHD src
SET SUPPRESSED_WARNINGS=-Wno-unused-parameter -Wno-unused-function -Wno-cast-function-type
SET COMMON_FLAGS=-g -Wall -Wextra -std=c99 %SUPPRESSED_WARNINGS%

gcc %COMMON_FLAGS% *.c -o ../%EXENAME%.exe -Iinclude -Llib -lopengl32 -lgdi32

POPD

REM generate pdb debug file from exe
cv2pdb -C %EXENAME%.exe

@echo on
