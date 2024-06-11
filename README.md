# Проект: Электронная таблица

Проект реализует функционал электронной таблицы в классе Sheet. Есть возможность добавлять значения в ячейки, получать текстовое значение ячейки, вычислять значения формульных ячеек.
Формульные ячейки.

## Сборка проекта

Проект использует ANTLR, поэтому для его работы понадобится [JDK](https://www.oracle.com/java/technologies/downloads/)

Инструкцию по установке ANTLR можно найти на сайте [antlr.org](https://www.antlr.org/). Более подробные рекомендации представлены в руководстве [Getting Started](https://github.com/antlr/antlr4/blob/master/doc/getting-started.md). Убедитесь, что JAR-файл antlr*.jar находится в переменной среды CLASSPATH. Это нужно для компиляции Java-кода. Если вы следовали инструкциям на сайте ANTLR, CLASSPATH уже должен быть правильным.

Для компиляции этого кода понадобится библиотека ANTLR4 C++ Runtime. Скачайте архив antlr4-cpp-runtime*.zip из раздела Download на сайте [antlr.org](https://www.antlr.org/download.html).

После скачивания необходимых файлов должна получиться следующая структура проекта:
```
spreadsheet/
├── antlr4_runtime/
│   └── Содержимое архива antlr4-cpp-runtime*.zip.
├── build/
├── antlr-4.13.1-complete.jar
├── CMakeLists.txt
├── FindANTLR.cmake
├── Formula.g4
├── Остальные файлы проекта
└── ...
```
В папке antlr4_runtime разместите содержимое архива antlr4-cpp-runtime*.zip.

В папке build выполняйте генерацию и сборку проекта.

Версия antlr-4.12.0-complete.jar может отличаться. В CMakeLists.txt замените версию JAR-файла на актуальную.

