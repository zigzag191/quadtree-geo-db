#include <iostream>
#include <vector>
#include <unordered_map>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"

#include "../GeoDb/Database.h"

namespace
{
    auto WindowWidth = 800u;
    auto WindowHeight = 800u;
    auto CameraOffsetX = 0.0f;
    auto CameraOffsetY = 0.0f;
    auto Zoom = 10.0f;

    sf::Transform CalculateMapTransform(const geodb::Database& database)
    {
        auto transform = sf::Transform{};

        const auto& indexedArea = database.GetIndexedArea();

        const auto topLeftX = indexedArea.GetCenterX() - indexedArea.GetHalfWidth();
        const auto topLeftY = indexedArea.GetCenterY() + indexedArea.GetHalfHeight();

        transform.scale({ 1.0f, static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight) });
        transform.scale({ Zoom, Zoom });
        transform.translate({ -CameraOffsetX, -CameraOffsetY });

        transform.scale({ 1.0f, -1.0f });
        transform.translate({
            static_cast<float>(-topLeftX),
            static_cast<float>(-topLeftY)
        });

        return transform;
    }
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Osm file path in xml formal must be passed as the only argument\n";
		return EXIT_FAILURE;
	}

	auto database = geodb::Database::FromFile(argv[1]);
    const auto result = database.Query(database.GetIndexedArea());

    auto window = sf::RenderWindow{ sf::VideoMode({ WindowWidth, WindowHeight }), "Voronezh" };

    auto addNode = [&](std::size_t i, const geodb::Way& way, std::vector<sf::Vertex>& projectedWay, bool insertTwice = false)
        {
            const auto& nodeIndex = way.GetNodes()[i];
            const auto& node = database.GetMap().GetNodes()[nodeIndex];
            const auto position = sf::Vector2f{ static_cast<float>(node.GetX()), static_cast<float>(node.GetY()) };
            projectedWay.push_back(sf::Vertex{ position, sf::Color::Black });
            if (insertTwice)
            {
                projectedWay.push_back(sf::Vertex{ position, sf::Color::Black });
            }
        };

    auto sfmlIdToDatabaseId = std::unordered_map<std::size_t, std::size_t>{};
    auto ways = std::vector<std::vector<sf::Vertex>>{};
    for (std::size_t wayId = 0; wayId < database.GetMap().GetWays().size(); ++wayId)
    {
        const auto& way = database.GetMap().GetWays()[wayId];

        auto projectedWay = std::vector<sf::Vertex>{};

        addNode(0, way, projectedWay);

        for (std::size_t i = 1; i < way.GetNodes().size() - 1; ++i)
        {
            addNode(i, way, projectedWay, true);
        }

        if (way.GetNodes().size() != 1)
        {
            addNode(way.GetNodes().size() - 1, way, projectedWay);
        }

        ways.push_back(std::move(projectedWay));
        sfmlIdToDatabaseId[ways.size() - 1] = wayId;
    }

    auto clock = sf::Clock{};

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto resizedEvent = event->getIf<sf::Event::Resized>())
            {
                WindowWidth = resizedEvent->size.x;
                WindowHeight = resizedEvent->size.y;
            }
        }

        const auto dt = clock.restart().asSeconds();

        static constexpr auto cameraSpeed = 100.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            CameraOffsetY -= cameraSpeed / Zoom * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            CameraOffsetX -= cameraSpeed / Zoom * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            CameraOffsetY += cameraSpeed / Zoom * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            CameraOffsetX += cameraSpeed / Zoom * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        {
            Zoom += Zoom * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            Zoom -= Zoom * dt;
        }

        window.clear(sf::Color::White);

        const auto cameraTransform = CalculateMapTransform(database);
        for (std::size_t sfmlWayId = 0; sfmlWayId < ways.size(); ++sfmlWayId)
        {
            const auto& way = ways[sfmlWayId];
            const auto& databaseWayId = sfmlIdToDatabaseId[sfmlWayId];
            const auto& databaseWay = database.GetMap().GetWays()[databaseWayId];

            if ((databaseWay.GetBoundingBox().GetArea() * Zoom * Zoom) / (WindowWidth * WindowHeight) < 0.0004)
            {
                continue;
            }

            window.draw(way.data(), way.size(), sf::PrimitiveType::Lines, cameraTransform);
        }

        window.display();
    }

	return EXIT_SUCCESS;
}
