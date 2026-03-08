# me3_csv_ekstraktor
Достаёт из PCC файлов Mass Effect 3 координаты моделей для их разложения в 3D редакторе как на внутриигровых уровнях.  
Однако нужно всё же поправить и некоторые расширения для Blender'а, которые бы правильно расставили их (модели). Или написать своё.
  
Для сборки данных из нескольких файлов в один CSV нужно указать параметр -p (или -P) и прописать нужный путь, например:  
```./me3_csv_ekstraktor_Linux ./CookedPCConsole/Bio*_ProEar* -p```  

## Сборка программы
Для сборки применить команду `make`.  
Для сборки под Windows применить команду `make TARGET_OS=windows`.  

## Дополнительное инфо
Artur Ajvazjan (arbtttrn6), 2025-2026.

В качестве материалов-ориентиров были взяты:

* https://github.com/ME3Explorer/ME3Explorer/
* https://github.com/ME3Tweaks/LegendaryExplorer/
* https://me3explorer.fandom.com/wiki/

!!!ОСТОРОЖНО!!! НИЗКОКАЧЕСТВЕННЫЙ КОД! Написано относительно неумело.
