import argparse
import sys
import logging
import json
from utils import write_csv_table

logging.basicConfig(format='[ %(levelname)s ] %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

ATTRIBUTES = ['name_en', 'name']
def sort_key(row):
    return (row['name_en'], row['name'])

def cli_argument_parser():
    #pylint: disable=duplicate-code
    parser = argparse.ArgumentParser(
        description='Script to convert *.geojson to *.csv'
    )
    parser.add_argument('-i', '--in_geojson_file',
                        help='Input GeoJSON file name downloaded from '
                             'https://osm-boundaries.com',
                        required=True,
                        type=str,
                        dest='in_geojson_file')
    parser.add_argument('-o', '--out_csv_file',
                        help='Output csv file name',
                        required=True,
                        type=str,
                        dest="out_csv_file")

    args = parser.parse_args()
    return args

def read_geojson(path):
    logger.info(f'Reading GeoJSON file: {path}')

    with open(path, encoding='utf-8') as f:
        return json.load(f)

def extract_rows(data):
    rows = []

    for feature in data.get('features', []):
        props = feature.get('properties', {})
        geometry = feature.get('geometry', {})
        row = {
            attr: props.get(attr) or ''
            for attr in ATTRIBUTES
        }
        row['wkt'] = geojson_to_wkt(geometry)
        rows.append(row)

    rows.sort(key=sort_key)
    for idx, row in enumerate(rows, start=0):
        row['id'] = idx
    logger.info(f'Extracted {len(rows)} features from GeoJSON')
    return rows

def geojson_to_wkt(geometry):
    geom_type = geometry.get('type')
    coords = geometry.get('coordinates', [])

    if geom_type == 'Polygon':
        return _polygon_to_wkt(coords)

    if geom_type == 'MultiPolygon':
        polygons = ', '.join(_polygon_to_wkt(poly, wrap=False) for poly in coords)
        return f'MULTIPOLYGON ({polygons})'

    logger.warning(f'Unsupported geometry type: {geom_type}')
    return ''

def _polygon_to_wkt(coords, wrap = True):
    rings = ', '.join(
        f"({', '.join(f'{x} {y}' for x, y in ring)})"
        for ring in coords
    )
    return f'POLYGON ({rings})' if wrap else f'({rings})'

def main():
    args = cli_argument_parser()
    try:
        geojson_data = read_geojson(args.in_geojson_file)
        rows = extract_rows(geojson_data)
        write_csv_table(
            rows,
            args.out_csv_file,
            fieldnames=['id', *ATTRIBUTES, 'wkt'],
            sort_key=sort_key,
            logger=logger
        )
    except Exception as ex:
        logger.exception(f'Unexpected error while parsing administrative units DB: {str(ex)}')

if __name__ == '__main__':
    sys.exit(main() or 0)
