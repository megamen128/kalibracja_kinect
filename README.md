# kinect_kalibracja

Program służy do kalibracji kamery głębi Kinecta v1 za pomocą obrazu z innej kamery kolorowej(W testach wykorzystano kamerę PS Eye). 

## Wymagania 
Do całkowitego wykorzystania programu wymagany jest kinect v1 oraz innna kolorowa kamera.
Wymagane jest zainstalowanie sterowników dla tych urządzeń. Dla Kinecta jest to pakiet
"Kinect for Windows SDK v1.8". Projekt tworzony był z użyciem Visual Studio 2017, biblioteki OpenCV 2.4.1 oraz modułu OpenNI2. Projekt został zbudowny za pomocą CMake w folderze build więc posiadanie tych bibliotek nie jest konieczne


## Budowa oraz korzystanie z programu
-Zbudowanie projektu z folderu Build (Możliwa jest edycja kodu w pliku main.c przed budową)
-Skopiowanie plików dll oraz folderów "Zdjecia" i "OpenNI2" z folderu "Projekt1" do zbudowanego folderu "bin/Debug"
-Program uruchamiamy za pomocą pliku "kalib.exe" w zbudowanym folderze "bin/Debug"
-Za pomocą menu wybieramy odpowiednią opcje.
	* Do zapisu zdjęć z kamer wybieramy opcje nr.1
	* Do kalibracji kamer z wcześniej zapisanych zdjęć wybieramy opcje nr.2
	* Do przeprowadzenia rektyfikacji wybieramy opcje nr.3
 

## Autorzy
Marcin Paul

##Licencja

Projekt dostępny jest na licencji MIT. Więcej szczegółów o licencji znajduje się w pliku [LICENSE.MD](LICENSE.md)

