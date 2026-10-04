# Скрипты для бенчмаркинга приложений

## Бенчмарки приложений

Скрипты предназначены для автоматического запуска соответствующих приложений
на различных комбинациях входных данных с целью сбора статистики по времени
выполнения ключевых этапов работы.

**Общие параметры:**
- `<path/to/geotags>` — путь до корня проекта.
- `[number_of_iterations]` — количество итераций (по умолчанию 100).


### Бенчмарк приложения geotags_app

```bash
./benchmarks/geotags_app.sh <path/to/geotags> [number_of_iterations]
```

Пример:

```bash
./benchmarks/geotags_app.sh . 50
```

### Бенчмарк приложения search_district_app

```bash
./benchmarks/search_district_app.sh <path/to/geotags> [number_of_iterations]
```

Пример:

```bash
./benchmarks/search_district_app.sh . 50
```

## Скрипт подсчета метрик производительности

```bash
python3 ./benchmarks/calculate_metrics.py -i <path/to/benchmark_result.txt> \
                                         [-d <discription>]
```

Этот вспомогательный скрипт используется бенчмарками для обработки времен,
накопленных при запуске соответствующих приложений.

Входные данные:

- путь до текстового файла с результатами экспериментов (ключ `-i`),
- возможное описание (ключ `-d`).

Алгоритм: читает текстовый файл и выводит среднее арифметическое, максимум и минимум времен
выполнения каждого этапа работы приложения.

Пример:

```bash
python3 ./benchmarks/calculate_metrics.py -i times_distance_haversine_grid_from_russian_cities.txt \
                                          -d "Calculating with haversine metric and grid search on database russian_cities"
```
