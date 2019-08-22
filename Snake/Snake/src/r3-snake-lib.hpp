
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#pragma once

namespace r3 {

	namespace snake {

		typedef enum Snake_SplashSceneClientRequest {
			NONE,
			EXIT_GAME,
		} SplashSceneClientRequest;

		typedef enum Snake_ClientMode {
			SPLASH_SCREEN,
		} ClientMode;

		class SplashSceneController;
		class SplashSceneRenderer;
		class GameClient;

		class SplashSceneController {

		private:
			sf::RenderWindow* window;
			SplashSceneRenderer* renderer;

		private:
			sf::Music* music;
			bool musicLoaded;

		public:
			SplashSceneController(sf::RenderWindow& window);

		public:
			~SplashSceneController();

		public:
			void start();
			void finish();

		public:
			SplashSceneClientRequest processEvent(sf::Event& event);
			void render();

		};

		class SplashSceneRenderer {

		private:
			sf::Texture* splashTexture;
			sf::Sprite* splashSprite;

		public:
			SplashSceneRenderer();

		public:
			~SplashSceneRenderer();

		public:
			void render(sf::RenderTarget& renderTarget);

		};

		class GameClient {

		private:
			ClientMode mode;
			sf::RenderWindow window;
			
		private:
			SplashSceneController* splashSceneController;

		public:
			GameClient();

		public:
			~GameClient();

		public:
			void run();

		private:
			void processSplashScreenEvent(sf::Event& event);

		};

	}

}
