import webbrowser
import sys
import argparse
import logging
import pandas as pd
import geopandas as gpd
import folium
from folium.features import GeoJsonTooltip
from folium.plugins import MarkerCluster
from shapely import wkt
from utils import CRS, CSV_SEPARATOR

logging.basicConfig(format='[ %(levelname)s ] %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)



def cli_argument_parser():
    parser = argparse.ArgumentParser(
        description='Script for visualize administrative units (with cities optionally) on map'
    )
    parser.add_argument('-a', '--admin_units',
                        help='Input csv file name with adminitration units',
                        required=True,
                        type=str,
                        dest='admin_units_csv')
    parser.add_argument('-c', '--cities',
                        help='Input csv file name with cities and admin_units',
                        required=False,
                        type=str,
                        dest='cities_csv')
    parser.add_argument('-o', '--output',
                        help='Output html file name',
                        required=True,
                        type=str,
                        dest="out_html_file")
    args = parser.parse_args()
    return args

def load_admin_units(path):
    logger.info(f'Loading administrative units from {path}')

    df = pd.read_csv(path, sep=CSV_SEPARATOR)

    if 'wkt' not in df.columns:
        raise ValueError('admin_units CSV must contain "wkt" column')

    geometry = df['wkt'].map(wkt.loads)
    gdf = gpd.GeoDataFrame(df, geometry=geometry, crs=CRS)

    logger.info(f'Loaded {len(gdf)} administrative units')
    return gdf


def load_cities(path):
    logger.info(f'Loading cities from {path}')

    df = pd.read_csv(path, sep=CSV_SEPARATOR)
    logger.info(f'Loaded {len(df)} cities')
    return df

def create_base_map(admin_units_gdf):
    centroid = admin_units_gdf.geometry.union_all("unary").centroid
    return folium.Map(
        location=[centroid.y, centroid.x],
        zoom_start=4
    )


def add_admin_layer(m, admin_units_gdf, name_column = 'name'):
    folium.GeoJson(
        admin_units_gdf.to_json(),
        style_function=lambda _: {
            'fillColor': 'blue',
            'color': 'black',
            'weight': 1,
            'fillOpacity': 0.3,
        },
        highlight_function=lambda _: {
            'weight': 3,
        },
        tooltip=GeoJsonTooltip(
            fields=[name_column],
            aliases=['Name:'],
            localize=True
        )
    ).add_to(m)


def add_cities_layer(m, cities_df):
    required_columns = {'city', 'latitude', 'longitude'}

    if not required_columns.issubset(cities_df.columns):
        logger.warning(
            f'Cities CSV missing required columns {str(", ".join(required_columns))}. ' \
            'Skipping cities layer.')
        return

    marker_cluster = MarkerCluster().add_to(m)

    for row in cities_df.itertuples(index=False):
        popup_text = f"City: {row.city}"

        folium.Marker(
            location=[row.latitude, row.longitude],
            popup=popup_text,
            icon=folium.Icon(icon='cloud')
        ).add_to(marker_cluster)


def main():
    args = cli_argument_parser()
    try:
        admin_units_gdf = load_admin_units(args.admin_units_csv)

        cities_df = None
        if args.cities_csv:
            cities_df = load_cities(args.cities_csv)

        logger.info('Building map...')
        m = create_base_map(admin_units_gdf)

        add_admin_layer(m, admin_units_gdf)

        if cities_df is not None:
            add_cities_layer(m, cities_df)

        output_path = args.out_html_file
        m.save(output_path)

        logger.info(f'Map saved to {output_path}')

        webbrowser.open(output_path)
    except Exception as ex:
        logger.exception(f'{str(ex)}')

if __name__ == '__main__':
    sys.exit(main() or 0)
