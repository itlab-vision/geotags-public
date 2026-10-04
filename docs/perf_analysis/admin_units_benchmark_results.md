# Результаты бенчмаркинга приложения административных единиц

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
      <th colspan="3">Время чтения административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian admin units</td>
      <td>0.18891609</td>
      <td>0.225942</td>
      <td>0.183163</td>
      <td>1.175e-05</td>
      <td>2.046e-05</td>
      <td>9.94e-06</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.01460389</td>
      <td>0.0174397</td>
      <td>0.0135034</td>
      <td>7.16e-06</td>
      <td>1.23e-05</td>
      <td>5.97e-06</td>
    </tr>
  </tbody></table>

- **Версия v2.0 (база в формате FlatBuffers, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время чтения административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian admin units</td>
      <td>0.01593369</td>
      <td>0.0181346</td>
      <td>0.0148555</td>
      <td>1.141e-05</td>
      <td>1.702e-05</td>
      <td>9.84e-06</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.00252074</td>
      <td>0.00502655</td>
      <td>0.00098063</td>
      <td>1.848e-05</td>
      <td>3.684e-05</td>
      <td>6.79e-06</td>
    </tr>
  </tbody></table>

- **Версия v2.1 (база в формате FlatBuffers, база соседей административных единиц в формете csv, поиск по соседям):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время чтения базы административных единиц, с</th>
      <th colspan="3">Время чтения базы соседей административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
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
      <td>Russian admin units</td>
      <td>0.01579159</td>
      <td>0.0178406</td>
      <td>0.0148659</td>
      <td rowspan="3">0.00010188</td>
      <td rowspan="3">0.00015658</td>
      <td rowspan="3">9.508e-05</td>
      <td>5.01e-06</td>
      <td>7.25e-06</td>
      <td>4.42e-06</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.0027253</td>
      <td>0.00513788</td>
      <td>0.00097338</td>
      <td>8.96e-06</td>
      <td>9.333e-05</td>
      <td>3.01e-06</td>
    </tr>
  </tbody></table>

- **Версия v2.2 (база в формате FlatBuffers, база соседей административных единиц в формете bin, поиск по соседям):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время чтения базы административных единиц, с</th>
      <th colspan="3">Время чтения базы соседей административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
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
      <td>Russian admin units</td>
      <td>0.01578305</td>
      <td>0.0176859</td>
      <td>0.0148637</td>
      <td rowspan="3">3.294e-05</td>
      <td rowspan="3">9.51e-05</td>
      <td rowspan="3">1.16e-05</td>
      <td>5.06e-06</td>
      <td>8.74e-06</td>
      <td>4.3e-06</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.00231734</td>
      <td>0.00451427</td>
      <td>0.00087864</td>
      <td>7.57e-06</td>
      <td>3.341e-05</td>
      <td>2.9e-06</td>
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
      <th colspan="3">Время чтения административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian admin units</td>
      <td>0.34833556</td>
      <td>0.427414</td>
      <td>0.345011</td>
      <td>2.089e-05</td>
      <td>3.845e-05</td>
      <td>1.816e-05</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.02625511</td>
      <td>0.0317111</td>
      <td>0.0238427</td>
      <td>8.82e-06</td>
      <td>3.009e-05</td>
      <td>7.36e-06</td>
    </tr>
  </tbody></table>

- **Версия v2.0 (база в формате FlatBuffers, линейный поиск):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время чтения административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
    </tr>
    <tr>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
      <th>Mean</th>
      <th>Max</th>
      <th>Min</th>
    </tr></thead>
  <tbody>
    <tr>
      <td>Russian admin units</td>
      <td>0.02532114</td>
      <td>0.0302228</td>
      <td>0.0225059</td>
      <td>2.145e-05</td>
      <td>5.329e-05</td>
      <td>1.746e-05</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.00204592</td>
      <td>0.00271819</td>
      <td>0.00147872</td>
      <td>1.269e-05</td>
      <td>2.295e-05</td>
      <td>8.53e-06</td>
    </tr>
  </tbody></table>


- **Версия v2.1 (база в формате FlatBuffers, база соседей административных единиц в формете csv, поиск по соседям):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время чтения базы административных единиц, с</th>
      <th colspan="3">Время чтения базы соседей административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
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
      <td>Russian admin units</td>
      <td>0.02514806</td>
      <td>0.0304875</td>
      <td>0.0224798</td>
      <td rowspan="3">0.00015802</td>
      <td rowspan="3">0.00045174</td>
      <td rowspan="3">0.00013955</td>
      <td>7.7e-06</td>
      <td>1.599e-05</td>
      <td>6.34e-06</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.00197028</td>
      <td>0.00235714</td>
      <td>0.00140424</td>
      <td>5.22e-06</td>
      <td>4.851e-05</td>
      <td>3.41e-06</td>
    </tr>
  </tbody></table>

- **Версия v2.2 (база в формате FlatBuffers, база соседей административных единиц в формете bin, поиск по соседям):**
  <table><thead>
    <tr>
      <th rowspan="2">База данных</th>
      <th colspan="3">Время чтения базы административных единиц, с</th>
      <th colspan="3">Время чтения базы соседей административных единиц, с</th>
      <th colspan="3">Время поиска административной единицы, с</th>
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
      <td>Russian admin units</td>
      <td>0.02555993</td>
      <td>0.0314684</td>
      <td>0.0225537
</td>
      <td rowspan="3">4.99e-05</td>
      <td rowspan="3">6.947e-05</td>
      <td rowspan="3">3.536e-05</td>
      <td>8.01e-06</td>
      <td>2.468e-05</td>
      <td>6.39e-06</td>
    </tr>
    <tr>
      <td>Russian admin units (simplify 1km)</td>
      <td>0.00205553</td>
      <td>0.00249048</td>
      <td>0.00156258</td>
      <td>5.38e-06</td>
      <td>5.121e-05</td>
      <td>3.99e-06</td>
    </tr>
  </tbody></table>

**Примечания:**
1. Результаты были получены с помощью скрипта `benchmarks/search_admin_unit_by_coords_app.sh`.
1. В скрипте `benchmarks/search_admin_unit_by_coords_app.sh` для вычисления метрик используется скрипт `benchmarks/calculate_metrics.py`, 
   на вход которому подается путь до `.txt` файла со значениями времен, полученных при работе бенчмарка.
