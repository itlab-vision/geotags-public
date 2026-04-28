import csv
import logging
import math
import unicodedata

CRS = "EPSG:4326"   # coordinate reference system
CSV_SEPARATOR = ';'

EARTH_R = 6371.0

def write_csv_table(rows, filename, fieldnames=None,
                    sort_key=None,
                    logger=None):
    if logger is None:
        logger = logging.getLogger(__name__)
        logger.setLevel(logging.WARNING)

    logger.info('Writing to CSV file: %s', filename)

    if not fieldnames and rows:
        fieldnames = list(rows[0].keys())

    if sort_key:
        sorted_rows = sorted(rows, key=sort_key, reverse=False)
    else:
        sorted_rows = rows

    with open(filename, 'w', encoding='utf-8', newline='') as out_csv_file:
        csv.register_dialect('row_writer', delimiter=';', quoting=csv.QUOTE_ALL)
        writer = csv.DictWriter(out_csv_file, fieldnames=fieldnames, dialect='row_writer')
        writer.writeheader()
        writer.writerows(sorted_rows)

    logger.info('Successfully wrote %d rows to CSV', len(sorted_rows))


def normalize_text(text):
    text = text.strip().casefold()
    return ''.join(
        c for c in unicodedata.normalize('NFKD', text)
        if not unicodedata.combining(c)
    )


def split_clean(text, sep=','):
    return [item.strip() for item in (text or "").split(sep) if item.strip()]


def join_unique(items, sep=','):
    return sep.join(dict.fromkeys(items))


def haversine_km(lat1, lon1, lat2, lon2):
    dlat = math.radians(lat2 - lat1)
    dlon = math.radians(lon2 - lon1)
    a = (math.sin(dlat / 2) ** 2
         + math.cos(math.radians(lat1))
         * math.cos(math.radians(lat2))
         * math.sin(dlon / 2) ** 2)
    return 2 * EARTH_R * math.asin(math.sqrt(a))
