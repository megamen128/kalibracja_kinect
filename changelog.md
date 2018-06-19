# Changelog
All notable changes to this project will be documented in this file.

## [0.0.4] - 06-19
### Added
- Dodano folder BUILD storzony za pomocą programu CMake. Umożliwia on modyfikacje kodu i budowe programu na nowo z wbudowanymi już bibliotekami

## [0.0.4] - 06-09 
### Added
- Kalibracja kamery PS Eye oraz zapisanie ich wyników w pliku "kalibracja_kamera"
- Przeprowadzenie kalibracji stereo w celu znalezienia zależności między kamerą kinecta a kamerą PS Eye. Zapisanie jej wyników w pliku "kalibracja_stereo"
- Rektyfikacja zdjęć kamery kinecta i kamery PS Eye na podstawie wyników kalibracji stereo
- Przekształcenie zdjęć kamery PS Eye do macierzy kamery Kinecta, wyświetlenie tych zdjęć oraz zapisanie ich w folderze "Zdjecia/Rektyf"

## [0.0.4] - 06-09
### Added
- Zwiększenie liczby zapisywanych zdjęć do 30 w celu poprawy wyników kalibracji
 oraz zmiana sposobu ich zapisu. Teraz do zapisu zdjęć wystarczy wciśnięcie klawisza "S" lub "s"
- Przerysowanie punktów z kamery kinecta na kamere głębi, wyświetlenie ich oraz zapisanie w folderze "Zdj_szare_z_punktami"
- Przygotowanie trybu 3 w którym ładowane będą zdjęcia w celu ich rektyfikacji


## [0.0.3] - 06-07
### Added
- Dla opcji pierwszej w menu dodano wyświetlanie kamery głębi oraz kamery ps Eye
- Zmieniono miejsce zapisu kolorowych zdjęć kinecta na "Zdjecia/Kinect"
- Zapisywanie zdjęć głębi w folderze "Zdjecia/Depth"
- Zapisywanie zdjęć kamery PS Eye w folderze "Zdjecia/Kamera"
- Kalibracja kamery kolorowej kinecta i zapisanie jej wyników w pliku "kalibracja_kinect"
- Uaktualnienie pliku gitignore i pliki z rozszerzeniem .jpg
## [0.0.2] - 05-15
### Added
- Dodanie prostego menu :
  * Po wciśnięciu klawisza '1' możliwy jest podgląd kamery kolorowej kinecta oraz zapisywanie obrazów z niej
  * Po wciśnięciu klawisza '2' zrobione wcześniej zdjęcia są wczytywane oraz na ich podstawie wyznaczane są punkty przecięcia szachownicy potrzebne do późniejszej kalibracji. Dodatkowo zapisywane są obrazy z nałożonymi punktami.
  - Wyświetlanie obrazów z dorysowanymi kątami szachownicy
  - Przechowywanie współrzędnych punktów obiektu szachownicy oraz tych samych punktów wyznaczonych ze zdjęć przy wciśnięciu klawisza "S" przy aktywnym oknie wyświetlania

## 0.0.1 - 05-08
### Added
- Wyświetlenie obrazu z kamery kolorowej kinecta
- Wyjście z z programu przez wciśnięcie klawisza "ESC"
- Zabezpieczenie programu w przypadku niepodłączonego kinecta
- Zapisanie wyświetlanych obrazów klawiszami 1-9 w folderze "Images/img1-9.jpg"
- Dodano plik gitignore