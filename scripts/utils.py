import csv
import math
import unicodedata

CRS: str = "EPSG:4326"   # coordinate reference system
CSV_SEPARATOR: str = ';'

EARTH_R: float = 6371.0

def write_csv_table(
    rows: list[dict[str, str]],
    filename: str,
    fieldnames: list[str],
    add_count_line: bool = False
) -> None:
    with open(filename, 'w', encoding='utf-8', newline='') as out_csv_file:
        if add_count_line:
            out_csv_file.write(f"{len(rows)}\n")

        csv.register_dialect('row_writer', delimiter=';', quoting=csv.QUOTE_ALL)
        writer = csv.DictWriter(out_csv_file, fieldnames=fieldnames, dialect='row_writer')
        writer.writeheader()
        writer.writerows(rows)


def normalize_text(text: str) -> str:
    text = text.strip().casefold()
    return ''.join(
        c for c in unicodedata.normalize('NFKD', text)
        if not unicodedata.combining(c)
    )


def split_clean(text: str | None, sep: str = ',') -> list[str]:
    return [item.strip() for item in (text or "").split(sep) if item.strip()]


def join_unique(items: list[str], sep: str = ',') -> str:
    return sep.join(dict.fromkeys(items))


def haversine_km(lat1: float, lon1: float, lat2: float, lon2: float) -> float:
    dlat = math.radians(lat2 - lat1)
    dlon = math.radians(lon2 - lon1)
    a = (math.sin(dlat / 2) ** 2
         + math.cos(math.radians(lat1))
         * math.cos(math.radians(lat2))
         * math.sin(dlon / 2) ** 2)
    return 2 * EARTH_R * math.asin(math.sqrt(a))
