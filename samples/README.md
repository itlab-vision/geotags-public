# Запуск приложений

## Приложение для извлечения широты и долготы локации из Exif

```bash
./build/getcoordinates_app -i=<path/to/image.jpg>
```

Для запуска приложения необходимо передать путь до изображения (ключ `-i`).

Пример:

```bash
./build/getcoordinates_app -i="./test_data/images/probe.jpg"
```

## Приложение для определения текущей локации

```bash
./build/geotags_app -i=<path/to/image.jpg> \
                    -t=<reader_type> \
                    -f=<path/to/coordinates.csv> \
                    -d=<distance_type> \
                    -st=<search_type> \
                    -s
```

Для запуска приложения необходимо передать путь до изображения (ключ `-i`)
и до файла в формате `.csv` или `.csv.fb` (ключ `-f`) с координатами городов,
формат входных данных (ключ `-t`), а также тип метрики для оценки расстояния
между локациями (ключ `-d`) и алгоритм поиска ближайшей локации (ключ `-st`).

В результате работы выводятся данные по текущей локации и переданное изображение, для отключения вывода изображения используется ключ `-s`.

Возможные значения метрики:

- `haversine` - метрика гаверсинуса, которая напрямую рассчитывает длину дуги на сфере,
- `haversine_approx` - аппроксимация метрики `haversine`, которая может
  использоваться при небольших расстояниях между парой объектов.

Возможные алгоритмы поиска ближайшей локации:
- `linear` - линейный проход по всем локациям,
- `grid` - база локаций предварительно разбивается на сегменты с использованием равномерной
  сетки, выполняется линейный проход по целевому и соседним сегментам,
- `grid_binary` - база локаций предварительно разбивается на сегменты с использованием равномерной
  сетки, выполняется бинарный поиск в целевом и соседних сегментах.

Примеры запуска для разных версий проекта:

Версия 1.0
```bash
./build/geotags_app -d=haversine \
                    -f="./test_data/cities_dbs/russian_cities.csv" \
                    -i="./test_data/images/probe.jpg" \
                    -t=csv \
                    -st=linear \
                    -s
```

Версия 2.0 | 2.1
```bash
./build/geotags_app -d=haversine \
                    -f="./test_data/cities_dbs/russian_cities.csv" \
                    -i="./test_data/images/probe.jpg" \
                    -t=csv_fb \
                    -st=linear \
                    -s
```

Версия 3.0
```bash
./build/geotags_app -d=haversine \
                    -f="./test_data/cities_dbs/russian_cities.csv" \
                    -i="./test_data/images/probe.jpg" \
                    -t=csv_fb \
                    -st=grid|grid_binary \
                    -s
```

## Приложение для определения региона (для регионов РФ)

```bash
./build/search_districts_app -i=<path/to/image.jpg> \ 
                             -r=<path/to/regions.csv> \
                             -t=<reader_type/csv>
```

Для запуска приложения необходимо передать путь до изображения
(ключ `-i`) и до файла в формате `.csv`, содержащего базу регионов.

Пример:

```bash
./build/search_districts_app -i="./test_data/images/probe.jpg" \ 
                             -r="./test_data/admin_units_dbs/russian_admin_units.csv" \
                             -t=csv
```

## Приложение для определения города и региона (для РФ)

```bash
./build/search_admin_unit_app -i=<path/to/image.jpg> \ 
                              -c=<path/to/cities.csv> \
                              -r=<path/to/regions.csv> \
                              -t=<reader_type/csv>
```

Для запуска приложения необходимо передать путь до изображения
(ключ `-i`) и до файлов в формате `.csv`, содержащего базу городов и регионов.

Пример:

```bash
./build/search_admin_unit_app -i="./test_data/images/probe.jpg" \ 
                              -c="./test_data/cities_dbs/million_plus_cities.csv" \
                              -r="./test_data/admin_units_dbs/russian_admin_units.csv \
                              -t=csv
```

## Приложение для разбиения базы городов на сегменты

```bash
./build/cities_grid_app -f=<path/to/cities.csv> 
```

Для запуска приложения необходимо передать путь до файла в формате `.csv`, 
содержащего базу городов.

Пример:

```bash
./build/cities_grid_app -f="./test_data/cities_dbs/russian_cities.csv"
```

## Приложения для поиска соседних регионов (для регионов РФ)

```bash
./build/district_neighbors_app -d=<path/to/regions.csv> 
```

Для запуска приложения необходимо передать путь до файла в формате `.csv`, 
содержащего базу регионов.

Пример:

```bash
./build/district_neighbors_app -d="./test_data/admin_units_dbs/russian_admin_units.csv"
```
