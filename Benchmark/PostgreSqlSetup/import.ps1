osm2pgsql `
    --create $args[0] `
    --host=localhost `
    --port=5432 `
    --database=gis `
    --user=gis `
    --password=gis `
    --schema=voronezh_center `
    --style=./default.style `
    --latlong
