# Результаты бенчмаркинга приложения

## Результаты для тестовой инфраструктуры 1

**Параметры тестовой инфраструктуры:**

<table>
  <tr>
    <th>CPU, RAM</th>
    <th>Intel Core i5-12500, 6 ядер, 12 потоков, 16 Gb</th>
  </tr>
  <tr>
    <th>Операционная система</th>
    <th>Ubuntu 24.10</th>
  </tr>
</table>

**Полученные результаты:**

- **Базовая версия v1.0 (база в формате csv, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">7.271e-05</td>
      <td rowspan="3">0.00021037</td>
      <td rowspan="3">4.205e-05</td>
      <td>0.00094068</td>
      <td>0.00170204</td>
      <td>0.00033149</td>
      <td>5.163e-05</td>
      <td>9.489e-05</td>
      <td>1.869e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.04781947</td>
      <td>0.062821</td>
      <td>0.0405672</td>
      <td>0.00255855</td>
      <td>0.00317453</td>
      <td>0.00223182</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.29522334</td>
      <td>0.387115</td>
      <td>0.287941</td>
      <td>0.00647123</td>
      <td>0.00918577</td>
      <td>0.00631423</td>
    </tr>
  </tbody></table>

- **Версия v2.0 (база в формате FlatBuffers, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">7.269e-05</td>
      <td rowspan="3">0.00023161</td>
      <td rowspan="3">4.105e-05</td>
      <td>0.00010725</td>
      <td>0.00022271</td>
      <td>4.392e-05</td>
      <td>4.628e-05</td>
      <td>0.00010732</td>
      <td>1.847e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00673007</td>
      <td>0.0105563</td>
      <td>0.00482131</td>
      <td>0.00301991</td>
      <td>0.00399091</td>
      <td>0.00250214</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.05851494</td>
      <td>0.0665592</td>
      <td>0.0544475</td>
      <td>0.00701547</td>
      <td>0.00922732</td>
      <td>0.0063217</td>
    </tr>
  </tbody></table>

- **Версия v2.1 (база в формате FlatBuffers, внутренние структуры FlatBuffers для чтения, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">9.13e-05</td>
      <td rowspan="3">0.00022352</td>
      <td rowspan="3">4.218e-05</td>
      <td>5.031e-05</td>
      <td>9.065e-05</td>
      <td>1.555e-05</td>
      <td>6.643e-05</td>
      <td>0.00012523</td>
      <td>2.009e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00214824</td>
      <td>0.00482055</td>
      <td>0.00119271</td>
      <td>0.00451602</td>
      <td>0.0081901</td>
      <td>0.0025643</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.00958738</td>
      <td>0.0111331</td>
      <td>0.00869587</td>
      <td>0.0088826</td>
      <td>0.010092</td>
      <td>0.00768569</td>
    </tr>
  </tbody></table>

- **Версия v3.0 (база в формате FlatBuffers, внутренние структуры FlatBuffers для чтения, сегментированный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время чтения базы сегментов локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации <br>(сегментированный линейный алгоритм), с</th>
      <th colspan="3">Время поиска ближайшей локации <br>(сегментированный бинарный алгоритм), с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">0.00011146</td>
      <td rowspan="3">0.00027268</td>
      <td rowspan="3">4.313e-05</td>
      <td>5.997e-05</td>
      <td>0.00014669</td>
      <td>1.49e-05</td>
      <td>0.00013502</td>
      <td>0.00027615</td>
      <td>3.868e-05</td>
      <td>1.262e-05</td>
      <td>1.793e-05</td>
      <td>3.59e-06</td>
      <td>1.245e-05</td>
      <td>2.392e-05</td>
      <td>3.38e-06</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00266828</td>
      <td>0.00482686</td>
      <td>0.0012399</td>
      <td>0.00038397</td>
      <td>0.00071366</td>
      <td>0.00018826</td>
      <td>3.973e-05</td>
      <td>7.881e-05</td>
      <td>1.86e-05</td>
      <td>3.881e-05</td>
      <td>6.63e-05</td>
      <td>1.716e-05</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.01009807</td>
      <td>0.01332</td>
      <td>0.00880228</td>
      <td>0.00045063</td>
      <td>0.00052317</td>
      <td>0.00041632</td>
      <td>0.00011691</td>
      <td>0.00013124</td>
      <td>0.00010378</td>
      <td>0.00011687</td>
      <td>0.00012868</td>
      <td>0.00010296</td>
    </tr>
  </tbody></table>

## Результаты для тестовой инфраструктуры 2

**Параметры тестовой инфраструктуры:**

<table>
  <tr>
    <th>CPU, RAM</th>
    <th>Intel Core i7-6850k, 6 ядер, 12 потоков, 64 Gb</th>
  </tr>
  <tr>
    <th>Операционная система</th>
    <th>Ubuntu 24.04.2 LTS</th>
  </tr>
</table>

**Полученные результаты:**

- **Базовая версия v1.0 (база в формате csv, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">8.845e-05</td>
      <td rowspan="3">0.00013822</td>
      <td rowspan="3">6.504e-05</td>
      <td>0.00067589</td>
      <td>0.00091405</td>
      <td>0.00051729</td>
      <td>3.841e-05</td>
      <td>5.453e-05</td>
      <td>2.932e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.06895279</td>
      <td>0.0847142</td>
      <td>0.0651909</td>
      <td>0.00358051</td>
      <td>0.00403039</td>
      <td>0.00347348</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.43416941</td>
      <td>0.475699</td>
      <td>0.426339</td>
      <td>0.01068896</td>
      <td>0.0114133</td>
      <td>0.0104885</td>
    </tr>
  </tbody></table>

- **Версия v2.0 (база в формате FlatBuffers, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">9.201e-05</td>
      <td rowspan="3">0.00019186</td>
      <td rowspan="3">5.618e-05</td>
      <td>9.375e-05</td>
      <td>0.00014658</td>
      <td>6.398e-05</td>
      <td>3.813e-05</td>
      <td>7.572e-05</td>
      <td>2.517e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00838834</td>
      <td>0.0101176</td>
      <td>0.00722036</td>
      <td>0.00422812</td>
      <td>0.00476708</td>
      <td>0.00386377</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.07931077</td>
      <td>0.092923</td>
      <td>0.0765167</td>
      <td>0.01101422</td>
      <td>0.012834</td>
      <td>0.0105005</td>
    </tr>
  </tbody></table>

- **Версия v2.1 (база в формате FlatBuffers, внутренние структуры FlatBuffers для чтения, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">9.251e-05</td>
      <td rowspan="3">0.00021443</td>
      <td rowspan="3">5.853e-05</td>
      <td>3.708e-05</td>
      <td>7.533e-05</td>
      <td>2.491e-05</td>
      <td>4.117e-05</td>
      <td>0.00010739</td>
      <td>2.506e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.0022226</td>
      <td>0.0025643</td>
      <td>0.0017874</td>
      <td>0.00461199</td>
      <td>0.00624699</td>
      <td>0.00400093</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.01301313</td>
      <td>0.0163673</td>
      <td>0.0111701</td>
      <td>0.01416444</td>
      <td>0.0171227</td>
      <td>0.0127182</td>
    </tr>
  </tbody></table>

- **Версия v3.0 (база в формате FlatBuffers, внутренние структуры FlatBuffers для чтения, сегментированный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время извлечения метаданных из EXIF, с</th>
      <th colspan="3">Время чтения базы локаций, с</th>
      <th colspan="3">Время чтения базы сегментов локаций, с</th>
      <th colspan="3">Время поиска ближайшей локации <br>(сегментированный линейный алгоритм), с</th>
      <th colspan="3">Время поиска ближайшей локации <br>(сегментированный бинарный алгоритм), с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian cities</td>
      <td rowspan="3">9.158e-05</td>
      <td rowspan="3">0.00018285</td>
      <td rowspan="3">6.285e-05</td>
      <td>3.897e-05</td>
      <td>5.948e-05</td>
      <td>2.833e-05</td>
      <td>7.41e-05</td>
      <td>0.00014846</td>
      <td>5.546e-05</td>
      <td>8.46e-06</td>
      <td>1.765e-05</td>
      <td>6.72e-06</td>
      <td>8.66e-06</td>
      <td>2.011e-05</td>
      <td>6.93e-06</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00209016</td>
      <td>0.0030773</td>
      <td>0.00156751</td>
      <td>0.00034039</td>
      <td>0.00067976</td>
      <td>0.00024877</td>
      <td>2.942e-05</td>
      <td>7.496e-05</td>
      <td>1.974e-05</td>
      <td>2.818e-05</td>
      <td>0.00012592</td>
      <td>1.963e-05</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.0135751</td>
      <td>0.0160022</td>
      <td>0.0124166</td>
      <td>0.0007076</td>
      <td>0.00098211</td>
      <td>0.00065104</td>
      <td>0.00019115</td>
      <td>0.00024751</td>
      <td>0.00017788</td>
      <td>0.0001904</td>
      <td>0.0002383</td>
      <td>0.00017569</td>
    </tr>
  </tbody></table>

**Примечания:**
1. Результаты были получены с помощью скрипта `benchmarks/benchmark.sh`.
1. В скрипте `benchmarks/benchmark.sh` для вычисления метрик используется скрипт `benchmarks/calculate_metrics.py`, 
   на вход которому подается путь до `.txt` файла со значениями времен, полученных при работе 
   скрипта `benchmarks/benchmark.sh`.
1. В приведенных результатах представлены замеры времени поиска ближайшей локации с использованием формулы гаверсинуса,
   так как этот метод учитывает кривизну Земли и обеспечивает наибольшую точность.
