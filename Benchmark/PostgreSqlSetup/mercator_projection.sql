create or replace function mercator_project_longitude(
    longitude float
) returns float as
$$
begin
    return 10000.0 * ((longitude * 3.14 / 180.0) - 0.0);
end;
$$ language plpgsql;

create or replace function mercator_project_latitude(
    latitude float
) returns float as
$$
begin
    return 10000.0 * ln(tan((3.14 / 4.0) +
                             ((latitude * 3.14 / 180.0) / 2.0)));
end;
$$ language plpgsql;

create or replace function mercator_project_point(
    point geometry
) returns geometry as
$$
begin
    return ST_Point(mercator_project_longitude(ST_X(point)),
                    mercator_project_latitude(ST_Y(point)));
end;
$$ language plpgsql;

create or replace function mercator_project_linestring(
    linestring geometry
) returns geometry as
$$
declare
    projected_points geometry[] = array(
            select mercator_project_point(ST_PointN(geo, generate_series(1, ST_NPoints(geo))))
            from (values (linestring)) as linestring(geo));
begin
    return ST_MakeLine(projected_points);
end;
$$ language plpgsql;

create or replace function mercator_projection(
    latlong geometry
) returns geometry as
$$
begin
    if ST_GeometryType(latlong) = 'ST_Point' then
        return mercator_project_point(latlong);
    elsif ST_GeometryType(latlong) = 'ST_LineString' then
        return mercator_project_linestring(latlong);
    end if;
end;
$$ language plpgsql;
