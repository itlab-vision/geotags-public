import sys
import argparse
import time
import logging
import pandas as pd
import geopandas as gpd
from shapely import wkt
from utils import CRS, CSV_SEPARATOR

logging.basicConfig(format='[ %(levelname)s ] %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

def cli_argument_parser():
    parser = argparse.ArgumentParser(
        description='Script for joining cities database and database of administrative units'
    )
    parser.add_argument('-c', '--cities',
                        help='Cities database (in CSV format)',
                        required=True,
                        type=str,
                        dest='cities')
    parser.add_argument('-a', '--admin_units',
                        help='Databases of administrative units (in CSV format). ' \
                        'Can be specified multiple times.',
                        required=True,
                        nargs='+',
                        type=str,
                        dest="admin_units")
    parser.add_argument('-o', '--output',
                        help='Output database (in CSV format)',
                        required=True,
                        type=str,
                        dest="output")
    args = parser.parse_args()
    return args

def load_admin_units(path):
    logger.info(f'Reading administrative units from {path}')

    df = pd.read_csv(path, sep=CSV_SEPARATOR, quotechar='"')
    if 'id' not in df.columns:
        raise ValueError(f"File {path} does not contain 'id' column")

    geometry = df['wkt'].map(wkt.loads)
    gdf = gpd.GeoDataFrame(df, geometry=geometry, crs=CRS)
    return gdf

def load_cities(path):
    logger.info(f'Reading cities from {path}')

    df = pd.read_csv(path, sep=CSV_SEPARATOR, quotechar='"')

    geometry = gpd.points_from_xy(df['longitude'], df['latitude'])
    gdf = gpd.GeoDataFrame(df, geometry=geometry, crs=CRS)

    return gdf

def get_admin_ids_for_cities(cities_gdf, admin_units_gdf):
    logger.info('Performing spatial join...')

    joined = gpd.sjoin(
        cities_gdf[['geometry']],
        admin_units_gdf,
        how='left',
        predicate='within'
    )
    joined = joined.groupby(level=0).first()

    return joined['id']

def write_output(gdf, path):
    logger.info(f'Writing results to {path}')

    gdf.to_csv(
        path,
        index=False,
        encoding='utf-8',
        sep=CSV_SEPARATOR
    )

def main():
    args = cli_argument_parser()

    try:
        logger.info('Processing data...')
        start_time = time.time()

        cities_gdf = load_cities(args.cities)
        for i, admin_path in enumerate(args.admin_units, start=1):
            admin_gdf = load_admin_units(admin_path)
            admin_ids = get_admin_ids_for_cities(cities_gdf, admin_gdf)

            col_name = f'admin_id_{i}'
            cities_gdf[col_name] = admin_ids.astype('Int64')
            logger.info(f'Added column {col_name} from {admin_path}')

        cities_gdf = cities_gdf.drop(columns=['geometry'])
        if 'alt_names' in cities_gdf.columns:
            cols = [col for col in cities_gdf.columns if col != 'alt_names'] + ['alt_names']
            cities_gdf.columns = cols
        write_output(cities_gdf, args.output)

        end_time = time.time()
        logger.info(f'Processing completed in {end_time - start_time} seconds')
        logger.info(f'Processed cities: {len(cities_gdf)}')
    except Exception as ex:
        logger.exception(f'Unexpected while joining cities and administrative units: {str(ex)}')

if __name__ == '__main__':
    sys.exit(main() or 0)
