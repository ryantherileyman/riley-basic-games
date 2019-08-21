
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main() {
	const float DESIRED_ASPECT_RATIO = 1.77777778f;

	sf::RenderWindow window(sf::VideoMode(800, 450), "Riley Entertainment - The Garden of Eating");
	window.setView(sf::View(sf::FloatRect(0.0f, 0.0f, 1920.0f, 1080.0f)));
	
	sf::Texture splashTexture;
	if (!splashTexture.loadFromFile("resources/textures/splash-screen.png")) {
		return EXIT_FAILURE;
	}
	sf::Sprite splashSprite;
	splashSprite.setTexture(splashTexture);

	sf::Music splashMusic;
	if (!splashMusic.openFromFile("resources/music/AtTheShore.ogg")) {
		return EXIT_FAILURE;
	}
	splashMusic.play();
	splashMusic.setLoop(true);

	while (window.isOpen()) {
		sf::Clock clock;

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::Resized) {
				float windowAspectRatio = (float)event.size.width / (float)event.size.height;
				if (windowAspectRatio > DESIRED_ASPECT_RATIO) {
					float desiredWidth = 1920.0f * (windowAspectRatio / DESIRED_ASPECT_RATIO);
					float leftPos = 0.0f - (desiredWidth - 1920.0f) / 2.0f;
					window.setView(sf::View(sf::FloatRect(leftPos, 0.0f, desiredWidth, 1080.0f)));
				}
				else if (windowAspectRatio < DESIRED_ASPECT_RATIO) {
					float desiredHeight = 1080.0f * (DESIRED_ASPECT_RATIO / windowAspectRatio);
					float topPos = 0.0f - (desiredHeight - 1080.0f) / 2.0f;
					window.setView(sf::View(sf::FloatRect(0.0f, topPos, 1920.0f, desiredHeight)));
				}
			}
		}

		if (window.isOpen()) {
			window.clear(sf::Color(0, 126, 3, 255));
			window.draw(splashSprite);
			window.display();
		}

		sf::Time elapsed = clock.getElapsedTime();
		sf::Int64 microsecondsElapsed = elapsed.asMicroseconds();
		printf("microseconds elapsed = %d\n", (int)microsecondsElapsed);
		if (microsecondsElapsed < 16667) {
			std::this_thread::sleep_for(std::chrono::microseconds(16667 - microsecondsElapsed));
		}
	}

	return 0;
}
