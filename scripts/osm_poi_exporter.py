import argparse
import csv
import logging
import sys
import osmnx as ox
import pandas as pd
import geopandas as gpd
from shapely.geometry import Polygon, MultiPolygon
from shapely.geometry.base import BaseGeometry
from utils import CSV_SEPARATOR

logging.basicConfig(format='[ %(levelname)s ] %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def cli_argument_parser() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description='Script for export POI from OpenStreetMap'
    )
    parser.add_argument('-c', '--city',
                        help='City name',
                        required=True,
                        type=str,
                        dest='city')
    parser.add_argument('-t', '--tags',
                        help='OSM tags in key=value format',
                        required=True,
                        nargs='+',
                        type=str,
                        dest='tags')
    parser.add_argument('-o', '--output',
                        help='Output database (in CSV format)',
                        required=True,
                        type=str,
                        dest='output')
    return parser.parse_args()

def parse_tags(raw_tags: list[str]) -> dict[str, str]:
    tags = {}

    for tag in raw_tags:
        if '=' not in tag:
            logger.warning(f'Invalid tag format: {tag}')
            continue

        key, value = tag.split('=', 1)
        tags[key] = value

    if not tags:
        raise ValueError('No valid tags specified. Example: station=subway')

    return tags

def load_city_polygon(city_name: str) -> Polygon | MultiPolygon:
    logger.info(f'Loading city: {city_name}')

    city_gdf = ox.geocode_to_gdf(city_name, which_result=1)
    if city_gdf.empty:
        raise RuntimeError(f'City \'{city_name}\' not found')

    polygon = city_gdf.geometry.iloc[0]

    logger.info(f'City boundary loaded. Area={polygon.area:.4f}')
    return polygon

def load_pois(city_polygon: Polygon | MultiPolygon, tags: dict[str, str]) -> gpd.GeoDataFrame:
    logger.info(f'Loading POI for tags: {tags}')

    pois = ox.features.features_from_polygon(city_polygon, tags)

    if pois.empty:
        raise RuntimeError('No objects found for specified tags')
    if 'name' not in pois.columns:
        raise RuntimeError('Objects do not contain \'name\' attribute')

    pois = pois[pois['name'].notnull()].copy()
    if pois.empty:
        raise RuntimeError('Objects with names were not found')

    logger.info(f'Found {len(pois)} named objects')
    return pois

def get_coordinates(geometry: BaseGeometry | None) -> tuple[float | None, float | None]:
    if geometry is None:
        return None, None
    if geometry.geom_type == 'Point':
        return geometry.x, geometry.y
    centroid = geometry.centroid
    return centroid.x, centroid.y

def classify_poi(row: pd.Series) -> str:
    amenity = row.get('amenity')
    if amenity in ['cafe', 'restaurant']:
        return amenity

    # --- transport ---
    if row.get('railway') == 'station':
        return 'subway' if row.get('station') == 'subway' else 'train_station'

    # --- shop ---
    shop = row.get('shop')
    if pd.notna(shop) and shop != '':
        return 'shop'

    return 'other'

def build_dataframe(pois: gpd.GeoDataFrame) -> pd.DataFrame:
    coords = pois.geometry.apply(get_coordinates)
    return pd.DataFrame({
        'longitude': coords.apply(lambda c: c[0]),
        'latitude': coords.apply(lambda c: c[1]),
        'type': pois.apply(classify_poi, axis=1),
        'name': pois['name']
    })

def write_dataframe(df: pd.DataFrame, output_file: str) -> None:
    with open(output_file, 'w', encoding='utf-8', newline='') as f:
        f.write(f'{len(df)}\n')
        df.to_csv(
            f,
            sep=CSV_SEPARATOR,
            index=False,
            quoting=csv.QUOTE_ALL
        )

def export_poi(city_name: str, tags: dict[str, str], output_file: str) -> None:
    city_polygon = load_city_polygon(city_name)
    pois = load_pois(city_polygon, tags)
    df = build_dataframe(pois)
    write_dataframe(df, output_file)

    logger.info(f'Saved {len(df)} rows to {output_file}')

def main():
    args = cli_argument_parser()
    try:
        tags = parse_tags(args.tags)
        export_poi(args.city, tags, args.output)
    except Exception as ex:
        logger.exception(f'{str(ex)}')


if __name__ == '__main__':
    sys.exit(main() or 0)
