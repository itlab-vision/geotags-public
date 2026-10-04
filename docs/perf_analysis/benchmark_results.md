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
      <td rowspan="3">8.255e-05</td>
      <td rowspan="3">0.00022701</td>
      <td rowspan="3">4.339e-05</td>
      <td>0.00047559</td>
      <td>0.00072333</td>
      <td>0.00018908</td>
      <td>5.92e-05</td>
      <td>9.436e-05</td>
      <td>2.364e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.01839141</td>
      <td>0.0230108</td>
      <td>0.0171956</td>
      <td>0.00230428</td>
      <td>0.00260771</td>
      <td>0.002148</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.1073015</td>
      <td>0.132277</td>
      <td>0.101651</td>
      <td>0.00623509</td>
      <td>0.00874522</td>
      <td>0.00584814</td>
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
      <td rowspan="3">9.685e-05</td>
      <td rowspan="3">0.00024512</td>
      <td rowspan="3">4.173e-05</td>
      <td>0.00017538</td>
      <td>0.00030993</td>
      <td>4.295e-05</td>
      <td>6.958e-05</td>
      <td>0.00010293</td>
      <td>1.734e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00691601</td>
      <td>0.0102132</td>
      <td>0.00379542</td>
      <td>0.00340604</td>
      <td>0.00449261</td>
      <td>0.00228902</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.03167941</td>
      <td>0.0367308</td>
      <td>0.0294685</td>
      <td>0.0067497</td>
      <td>0.0088649</td>
      <td>0.00591648</td>
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
      <td rowspan="3">0.00010729</td>
      <td rowspan="3">0.00023413</td>
      <td rowspan="3">4.117e-05</td>
      <td>3.41e-05</td>
      <td>6.647e-05</td>
      <td>1.429e-05</td>
      <td>6.791e-05</td>
      <td>0.00012115</td>
      <td>2.793e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00031428</td>
      <td>0.00069791</td>
      <td>0.00011907</td>
      <td>0.00529284</td>
      <td>0.00956303</td>
      <td>0.00236075</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.00090636</td>
      <td>0.00121193</td>
      <td>0.00067791</td>
      <td>0.01033219</td>
      <td>0.013264</td>
      <td>0.00744789</td>
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
      <td rowspan="3">0.00011184</td>
      <td rowspan="3">0.00028591</td>
      <td rowspan="3">4.01e-05</td>
      <td>3.886e-05</td>
      <td>5.526e-05</td>
      <td>1.153e-05</td>
      <td>3.871e-05</td>
      <td>5.317e-05</td>
      <td>1.368e-05</td>
      <td>1.581e-05</td>
      <td>2.22e-05</td>
      <td>7.99e-06</td>
      <td>1.896e-05</td>
      <td>0.00011106</td>
      <td>6.21e-06</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00016768</td>
      <td>0.00020707</td>
      <td>0.00016523</td>
      <td>2.684e-05</td>
      <td>3.088e-05</td>
      <td>2.577e-05</td>
      <td>4.565e-05</td>
      <td>7.132e-05</td>
      <td>1.436e-05</td>
      <td>1.582e-05</td>
      <td>1.858e-05</td>
      <td>1.421e-05</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.00130712</td>
      <td>0.00349278</td>
      <td>0.00062899</td>
      <td>7.328e-05</td>
      <td>0.00024619</td>
      <td>3.96e-05</td>
      <td>7.456e-05</td>
      <td>0.00014162</td>
      <td>5.942e-05</td>
      <td>9.674e-05</td>
      <td>0.00025326</td>
      <td>6.376e-05</td>
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
      <td rowspan="3">9.366e-05</td>
      <td rowspan="3">0.00020896</td>
      <td rowspan="3">6.524e-05</td>
      <td>0.00031068</td>
      <td>0.00054876</td>
      <td>0.00023805</td>
      <td>3.968e-05</td>
      <td>8.008e-05</td>
      <td>2.707e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.0313763</td>
      <td>0.0361484</td>
      <td>0.0287485</td>
      <td>0.00349866</td>
      <td>0.00580018</td>
      <td>0.00325329</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.15807387</td>
      <td>0.169423</td>
      <td>0.154713</td>
      <td>0.01069503</td>
      <td>0.0120957</td>
      <td>0.0103217</td>
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
      <td rowspan="3">9.306e-05</td>
      <td rowspan="3">0.0001873</td>
      <td rowspan="3">6.573e-05</td>
      <td>0.00010574</td>
      <td>0.00016232</td>
      <td>8.049e-05</td>
      <td>3.834e-05</td>
      <td>6.385e-05</td>
      <td>2.989e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00781567</td>
      <td>0.00887305</td>
      <td>0.00663172</td>
      <td>0.00406635</td>
      <td>0.00466209</td>
      <td>0.00369171</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.0467723</td>
      <td>0.0539097</td>
      <td>0.0439946</td>
      <td>0.01108264</td>
      <td>0.0126307</td>
      <td>0.010365</td>
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
      <td rowspan="3">9.223e-05</td>
      <td rowspan="3">0.00018102</td>
      <td rowspan="3">6.583e-05</td>
      <td>2.609e-05</td>
      <td>6.729e-05</td>
      <td>1.874e-05</td>
      <td>4.077e-05</td>
      <td>0.00013671</td>
      <td>2.793e-05</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00027409</td>
      <td>0.00033217</td>
      <td>0.00022253</td>
      <td>0.00446661</td>
      <td>0.00552866</td>
      <td>0.00375693</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.00151683</td>
      <td>0.00183967</td>
      <td>0.00114646</td>
      <td>0.015728</td>
      <td>0.0185278</td>
      <td>0.0139648</td>
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
      <td rowspan="3">9.445e-05</td>
      <td rowspan="3">0.0001837</td>
      <td rowspan="3">7.331e-05</td>
      <td>2.772e-05</td>
      <td>9.149e-05</td>
      <td>2.139e-05</td>
      <td>2.478e-05</td>
      <td>4.133e-05</td>
      <td>1.846e-05</td>
      <td>1.053e-05</td>
      <td>1.607e-05</td>
      <td>8.17e-06</td>
      <td>1.084e-05</td>
      <td>2.207e-05</td>
      <td>8.77e-06</td>
    </tr>
    <tr>
      <td>World cities</td>
      <td>0.00030319</td>
      <td>0.00067053</td>
      <td>0.00022554</td>
      <td>5.339e-05</td>
      <td>0.00010987</td>
      <td>3.93e-05</td>
      <td>3.406e-05</td>
      <td>8.359e-05</td>
      <td>2.577e-05</td>
      <td>3.269e-05</td>
      <td>5.545e-05</td>
      <td>2.501e-05</td>
    </tr>
    <tr>
      <td>cities_with_a_population_1000</td>
      <td>0.00154344</td>
      <td>0.00171154</td>
      <td>0.00121675</td>
      <td>9.471e-05</td>
      <td>0.0001381</td>
      <td>7.291e-05</td>
      <td>0.00013066</td>
      <td>0.00018777</td>
      <td>9.519e-05</td>
      <td>0.000127</td>
      <td>0.00017926</td>
      <td>9.625e-05</td>
    </tr>
  </tbody></table>

**Примечания:**
1. Результаты были получены с помощью скрипта `benchmarks/benchmark.sh`.
1. В скрипте `benchmarks/benchmark.sh` для вычисления метрик используется скрипт `benchmarks/calculate_metrics.py`, 
   на вход которому подается путь до `.txt` файла со значениями времен, полученных при работе 
   скрипта `benchmarks/benchmark.sh`.
1. В приведенных результатах представлены замеры времени поиска ближайшей локации с использованием формулы гаверсинуса,
   так как этот метод учитывает кривизну Земли и обеспечивает наибольшую точность.
