
#include <vector>
#include <map>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "r3-snake-gameoptions.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef enum class Snake_SplashMenuItemType {
			ACTION,
			SLIDER,
			BUTTON_OPTIONS,
		} SplashMenuItemType;

		typedef struct Snake_SplashSliderRange {
			int minValue;
			int maxValue;
			struct Snake_SplashSliderRange() {
				this->minValue = 0;
				this->maxValue = 0;
			}
			struct Snake_SplashSliderRange(int minValue, int maxValue) {
				this->minValue = minValue;
				this->maxValue = maxValue;
			}
			bool inRange(int value) const {
				bool result =
					(value >= this->minValue) &&
					(value <= this->maxValue);
				return result;
			}
		} SplashSliderRange;

		typedef struct Snake_SplashSliderRangeDescriptionDefn {
			SplashSliderRange sliderRange;
			const wchar_t* descriptiveText;
		} SplashSliderRangeDescriptionDefn;

		typedef struct Snake_SplashButtonOptionDefn {
			int buttonOptionId;
			const wchar_t* displayText;
			const wchar_t* descriptiveText;
		} SplashButtonOptionDefn;

		typedef struct Snake_SplashMenuItemDefn {
			int menuItemId;
			SplashMenuItemType menuItemType;
			const wchar_t* displayText;
			const wchar_t* descriptiveText;
			SplashSliderRange sliderRange;
			std::vector<SplashSliderRangeDescriptionDefn> sliderRangeDescriptionDefnList;
			std::map<int, SplashButtonOptionDefn> buttonOptionDefnMap;
		} SplashMenuItemDefn;

		namespace SplashMainMenuId {
			const int START_QUICK_GAME = 1;
			const int QUICK_GAME_OPTIONS = 2;
			const int EXIT_GAME = 3;
		}

		namespace SplashQuickGameOptionsMenuId {
			const int RETURN_TO_MAIN_MENU = 1;
			const int SNAKE_SPEED = 2;
			const int SNAKE_GROWTH = 3;
			const int FIELD_WIDTH = 4;
			const int FIELD_HEIGHT = 5;
		}

		namespace SplashQuickGameOptionValues {
			const SplashSliderRange SNAKE_SPEED(4, 30);
			const SplashSliderRange SNAKE_GROWTH(1, 10);
			const SplashSliderRange FIELD_WIDTH(12, 100);
			const SplashSliderRange FIELD_HEIGHT(12, 100);
		}

		namespace SplashMenuFactory {

			extern std::map<int, SplashMenuItemDefn> createMainMenuDefnMap();
			extern std::map<int, SplashMenuItemDefn> createQuickGameOptionsMenuDefnMap();

		}

		typedef enum class Snake_SplashSceneMode {
			MAIN_MENU,
			QUICK_GAME_OPTIONS_MENU,
		} SplashSceneMode;

		typedef struct Snake_SplashMenuKeypressResult {
			bool performedActionFlag;
			int actionMenuItemId;
		} SplashMenuKeypressResult;

		typedef struct Snake_SplashMenuMousePositionResult {
			bool overMenuItemFlag;
			int overMenuItemId;
			bool overSliderFlag;
			int overSliderValue;
		} SplashMenuMousePositionResult;

		typedef enum class Snake_SplashSceneClientRequest {
			NONE,
			EXIT_GAME,
			START_QUICK_GAME,
		} SplashSceneClientRequest;

		class SplashMenu;
		class SplashSceneController;
		class SplashSceneRenderer;

		class SplashMenu {

		private:
			std::map<int, SplashMenuItemDefn> menuItemDefnMap;
			std::unordered_map<int, int> chosenValueMap;

			int currItemId;

		public:
			SplashMenu(const std::map<int, SplashMenuItemDefn>& menuItemDefnList);

		public:
			std::map<int, SplashMenuItemDefn> getMenuItemDefnMap() const;
			SplashMenuItemDefn getItemDefn(int menuItemId) const;

		public:
			int getCurrItemId() const;
			SplashMenuItemDefn getCurrItemDefn() const;

		public:
			int moveToFirstItem();
			void setCurrItem(int menuItemId);
			int moveToNextItem();
			int moveToPreviousItem();

		public:
			int getItemValue(int menuItemId) const;
			const wchar_t* getItemValueDescriptiveText(int menuItemId) const;

		public:
			void setItemValue(int menuItemId, int value);
			int incrementItemValue(int menuItemId);
			int decrementItemValue(int menuItemId);

		private:
			SplashMenuItemDefn assertMenuItemAllowsValue(int menuItemId) const;

		};

		class SplashSceneController {

		private:
			sf::RenderWindow* window;
			SplashSceneRenderer* renderer;

		private:
			SplashSceneMode mode;

		private:
			sf::Music* music;
			bool musicLoaded;

		private:
			SplashMenu* mainMenu;
			SplashMenu* quickGameOptionsMenu;

		public:
			SplashSceneController(sf::RenderWindow& window);

		public:
			~SplashSceneController();

		public:
			QuickGameOptionsDefn getQuickGameOptions() const;

		public:
			SplashSceneClientRequest processEvent(sf::Event& event);
			void update();
			void render();

		private:
			SplashSceneClientRequest processKeypressEvent(sf::Event& event);
			SplashSceneClientRequest processMainMenuKeypressEvent(sf::Event& event);
			void processQuickGameOptionsKeypressEvent(sf::Event& event);

		private:
			void processMouseMovedEvent(sf::Event& event);
			SplashSceneClientRequest processLeftMouseButtonPressedEvent(sf::Event& event);

		private:
			SplashSceneClientRequest performMainMenuItemAction(int menuItemId);
			void performQuickGameOptionsMenuItemAction(int menuItemId);

		private:
			SplashMenuKeypressResult processMenuKeypressEvent(sf::Event& event, SplashMenu& menu);

		private:
			void ensureMusicLoaded();
			void freeMusic();

		private:
			void beginMusic();

		private:
			SplashMenu* getCurrentMenu();

		};

		typedef struct Snake_SplashMenuItemRenderState {
			const SplashMenu* menu;
			int menuItemId;
			float top;
			float backgroundWidth;
			SplashMenuItemDefn getMenuItemDefn() const {
				SplashMenuItemDefn result = menu->getItemDefn(menuItemId);
				return result;
			}
			bool isCurrItem() const {
				bool result = (menu->getCurrItemId() == menuItemId);
				return result;
			}

		} SplashMenuItemRenderState;

		class SplashSceneRenderer {

		private:
			sf::Font* uiFont;
			sf::Texture* splashTexture;

		private:
			sf::Sprite* splashSprite;

		public:
			SplashSceneRenderer();

		public:
			~SplashSceneRenderer();

		public:
			void render(sf::RenderTarget& renderTarget, const SplashMenu& menu);

		public:
			SplashMenuMousePositionResult resolveMenuMousePositionResult(const SplashMenu& menu, sf::Vector2f cursorPosition);

		private:
			void renderMenu(sf::RenderTarget& renderTarget, const SplashMenu& menu);
			float resolveHighestMenuItemWidth(const SplashMenu& menu);

		private:
			void renderMenuItem(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState);
			void renderFocusedMenuItemBackground(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState);
			void renderMenuItemLabel(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState);
			void renderMenuItemSlider(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState);
			void renderMenuItemDescriptiveText(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState);
			void renderMenuItemValueDescriptiveText(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState);

		private:
			sf::Text createMenuItemText(bool currItemFlag);
			sf::RectangleShape createMenuItemSliderShape(bool currItemFlag);
			sf::FloatRect resolveSliderRect(float menuWidth, float menuItemTop);

		};

	}

}
