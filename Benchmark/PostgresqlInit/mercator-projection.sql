CREATE OR REPLACE FUNCTION mercator_project_longitude(
    longitude float
) RETURNS float AS
$$
BEGIN
    RETURN longitude * 20037508.34 / 180.0;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION mercator_project_latitude(
    latitude float
) RETURNS float AS
$$
DECLARE
    lat_rad float = latitude * pi() / 180.0;
BEGIN
    RETURN ln(tan(pi() / 4 + lat_rad / 2)) * 20037508.34 / pi();
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION mercator_project_point(
    point geometry
) RETURNS geometry AS
$$
BEGIN
    RETURN ST_SetSRID(
            ST_Point(
                    mercator_project_longitude(ST_X(point)),
                    mercator_project_latitude(ST_Y(point))
            ),
            3857
           );
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION mercator_project_linestring(
    linestring geometry
) RETURNS geometry AS
$$
DECLARE
    projected_points geometry[] = array(
            SELECT mercator_project_point(ST_PointN(linestring, generate_series(1, ST_NPoints(linestring))))
                                  );
BEGIN
    RETURN ST_SetSRID(ST_MakeLine(projected_points), 3857);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION mercator_projection(
    latlong geometry
) RETURNS geometry AS
$$
BEGIN
    IF ST_GeometryType(latlong) = 'ST_Point' THEN
        RETURN mercator_project_point(latlong);
    ELSIF ST_GeometryType(latlong) = 'ST_LineString' THEN
        RETURN mercator_project_linestring(latlong);
    END IF;
END;
$$ LANGUAGE plpgsql;
