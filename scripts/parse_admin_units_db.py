import argparse
import sys
import logging
import json
from typing import Any
from utils import write_csv_table
from shapely.geometry import mapping, shape
from shapely import coverage_simplify

logging.basicConfig(format='[ %(levelname)s ] %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

PRECISION = 7
ATTRIBUTES: list[str] = ['name_en', 'name']
def sort_key(row: dict[str, str]) -> tuple[str, str]:
    return (row['name_en'], row['name'])

def cli_argument_parser() -> argparse.Namespace:
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
    parser.add_argument('-t', '--tolerance',
                        help='Simplification tolerance. '
                             'If omitted or 0, no simplification is applied.',
                        required=False,
                        type=float,
                        default=None,
                        dest="tolerance")

    args = parser.parse_args()
    return args

def read_geojson(path: str) -> dict[str, Any]:
    logger.info(f'Reading GeoJSON file: {path}')

    with open(path, encoding='utf-8') as f:
        return json.load(f)

def extract_rows(data: dict[str, Any]) -> list[dict[str, Any]]:
    rows = []
    features = data.get('features', [])
    if not features:
        logger.warning("No features found in GeoJSON")
        return rows

    for feature in features:
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

def simplify_geojson_topology(geojson_data: dict, tolerance: float = None) -> dict:
    logger.info(f"Topological simplification of coverage with tolerance={tolerance}")
    try:
        attributes = [feature['properties'] for feature in geojson_data['features']]
        geoms = [shape(feature['geometry']) for feature in geojson_data['features']]

        simplified_geoms = coverage_simplify(geoms, tolerance=tolerance)

        new_features = []
        for attrs, geom in zip(attributes, simplified_geoms):
            new_features.append({
                'type': 'Feature',
                'properties': attrs,
                'geometry': mapping(geom)
            })

        logger.info("Simplification completed")
        return {'type': 'FeatureCollection', 'features': new_features}

    except Exception as e:
        logger.error(f"Error in simplification: {e}. Return the original data.")
        return geojson_data


def geojson_to_wkt(geometry: dict[str, Any]) -> str:
    geom_type = geometry.get('type')
    coords = geometry.get('coordinates', [])

    if geom_type == 'Polygon':
        return _polygon_to_wkt(coords)

    if geom_type == 'MultiPolygon':
        polygons = ', '.join(_polygon_to_wkt(poly, wrap=False) for poly in coords)
        return f'MULTIPOLYGON ({polygons})'

    logger.warning(f'Unsupported geometry type: {geom_type}')
    return ''

def _polygon_to_wkt(coords: list[list[list[float]]], wrap: bool = True) -> str:
    rings = ', '.join(
        f"({', '.join(f'{x} {y}' for x, y in ring)})"
        for ring in coords
    )
    return f'POLYGON ({rings})' if wrap else f'({rings})'

def main():
    args = cli_argument_parser()
    try:
        geojson_data = read_geojson(args.in_geojson_file)
        if args.tolerance is not None and args.tolerance > 0:
            geojson_data = simplify_geojson_topology(
                geojson_data,
                args.tolerance
            )
        else:
            logger.info("No simplification requested")

        rows = extract_rows(geojson_data)

        logger.info(f'Writing to CSV file: {args.out_csv_file}')
        sorted_rows = sorted(rows, key=sort_key, reverse=False)
        write_csv_table(
            sorted_rows,
            args.out_csv_file,
            fieldnames=['id', *ATTRIBUTES, 'wkt'],
            add_count_line=True
        )
        logger.info(f'Successfully wrote {len(sorted_rows)} rows to CSV')
    except Exception as ex:
        logger.exception(f'Unexpected error while parsing administrative units DB: {str(ex)}')

if __name__ == '__main__':
    sys.exit(main() or 0)
