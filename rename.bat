setlocal enabledelayedexpansion
set /a n=0
for %%i in (dataset\IMG_*_*.jpg) do (
	set /a n+=1
	echo !n!
	rename "%cd%\%%i" "IMG_!n!.jpg"
)