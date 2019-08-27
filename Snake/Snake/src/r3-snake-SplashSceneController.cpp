
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-splashscene.hpp"

namespace r3 {

	namespace snake {

		const char* SPLASH_MUSIC_PATH = "resources/music/AtTheShore.ogg";

		SplashSceneController::SplashSceneController(sf::RenderWindow& window) {
			this->window = &window;
			this->window->setView(ViewUtils::createView(window.getSize().x, window.getSize().y));

			this->renderer = new SplashSceneRenderer();

			this->mode = SplashSceneMode::MAIN_MENU;

			this->music = { nullptr };
			this->musicLoaded = false;

			this->mainMenu = new SplashMenu(SplashMenuFactory::createMainMenuDefnMap());

			QuickGameOptionsDefn quickGameOptions;
			this->quickGameOptionsMenu = new SplashMenu(SplashMenuFactory::createQuickGameOptionsMenuDefnMap());
			this->quickGameOptionsMenu->setItemValue(SplashQuickGameOptionsMenuId::SNAKE_SPEED, quickGameOptions.snakeSpeedTilesPerSecond);
			this->quickGameOptionsMenu->setItemValue(SplashQuickGameOptionsMenuId::SNAKE_GROWTH, quickGameOptions.snakeGrowthPerApple);
			this->quickGameOptionsMenu->setItemValue(SplashQuickGameOptionsMenuId::FIELD_WIDTH, quickGameOptions.fieldSize.x);
			this->quickGameOptionsMenu->setItemValue(SplashQuickGameOptionsMenuId::FIELD_HEIGHT, quickGameOptions.fieldSize.y);

			SystemOptionsDefn systemOptions;
			this->systemOptionsMenu = new SplashMenu(SplashMenuFactory::createSystemOptionsMenuDefnMap());
			this->systemOptionsMenu->setItemValue(SplashSystemOptionsMenuId::MUSIC_VOLUME, systemOptions.musicVolume);
			this->systemOptionsMenu->setItemValue(SplashSystemOptionsMenuId::SOUND_EFFECTS_VOLUME, systemOptions.soundEffectsVolume);
		}

		SplashSceneController::~SplashSceneController() {
			delete this->renderer;
			if (this->music != nullptr) {
				delete this->music;
			}

			delete this->mainMenu;
			delete this->quickGameOptionsMenu;
			delete this->systemOptionsMenu;
		}

		void SplashSceneController::setWindow(sf::RenderWindow& window) {
			this->window = &window;
		}

		QuickGameOptionsDefn SplashSceneController::getQuickGameOptions() const {
			QuickGameOptionsDefn result;
			result.snakeSpeedTilesPerSecond = this->quickGameOptionsMenu->getItemValue(SplashQuickGameOptionsMenuId::SNAKE_SPEED);
			result.snakeGrowthPerApple = this->quickGameOptionsMenu->getItemValue(SplashQuickGameOptionsMenuId::SNAKE_GROWTH);
			result.fieldSize.x = this->quickGameOptionsMenu->getItemValue(SplashQuickGameOptionsMenuId::FIELD_WIDTH);
			result.fieldSize.y = this->quickGameOptionsMenu->getItemValue(SplashQuickGameOptionsMenuId::FIELD_HEIGHT);
			return result;
		}

		SystemOptionsDefn SplashSceneController::getSystemOptions() const {
			SystemOptionsDefn result;
			result.musicVolume = this->systemOptionsMenu->getItemValue(SplashSystemOptionsMenuId::MUSIC_VOLUME);
			result.soundEffectsVolume = this->systemOptionsMenu->getItemValue(SplashSystemOptionsMenuId::SOUND_EFFECTS_VOLUME);
			return result;
		}

		SplashSceneClientRequest SplashSceneController::processEvent(sf::Event& event) {
			SplashSceneClientRequest result = SplashSceneClientRequest::NONE;

			if (event.type == sf::Event::Closed) {
				result = SplashSceneClientRequest::EXIT_GAME;
				this->freeMusic();
			}
			else if (event.type == sf::Event::Resized) {
				this->window->setView(ViewUtils::createView(event.size.width, event.size.height));
			}
			else if (event.type == sf::Event::KeyPressed) {
				result = this->processKeypressEvent(event);
			}
			else if (event.type == sf::Event::MouseMoved) {
				this->processMouseMovedEvent(event);
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Button::Left) {
					result = this->processLeftMouseButtonPressedEvent(event);
				}
			}

			return result;
		}

		void SplashSceneController::update() {
			if (!this->musicLoaded) {
				this->beginMusic();
			}
		}

		void SplashSceneController::render() {
			SplashMenu* currMenu = this->getCurrentMenu();
			this->renderer->render(*this->window, *currMenu);
			this->window->display();
		}

		SplashSceneClientRequest SplashSceneController::processKeypressEvent(sf::Event& event) {
			SplashSceneClientRequest result = SplashSceneClientRequest::NONE;

			switch (this->mode) {
			case SplashSceneMode::MAIN_MENU:
				result = this->processMainMenuKeypressEvent(event);
				break;
			case SplashSceneMode::QUICK_GAME_OPTIONS_MENU:
				this->processQuickGameOptionsKeypressEvent(event);
				break;
			case SplashSceneMode::SYSTEM_OPTIONS_MENU:
				this->processSystemOptionsKeypressEvent(event);
				break;
			}

			return result;
		}
		
		SplashSceneClientRequest SplashSceneController::processMainMenuKeypressEvent(sf::Event& event) {
			SplashSceneClientRequest result = SplashSceneClientRequest::NONE;

			SplashMenuKeypressResult menuKeypressResult = this->processMenuKeypressEvent(event, *this->mainMenu);

			if (menuKeypressResult.performedActionFlag) {
				result = this->performMainMenuItemAction(menuKeypressResult.actionMenuItemId);
			}

			return result;
		}

		void SplashSceneController::processQuickGameOptionsKeypressEvent(sf::Event& event) {
			SplashMenuKeypressResult menuKeypressResult = this->processMenuKeypressEvent(event, *this->quickGameOptionsMenu);

			if (menuKeypressResult.performedActionFlag) {
				this->performQuickGameOptionsMenuItemAction(menuKeypressResult.actionMenuItemId);
			}
		}

		void SplashSceneController::processSystemOptionsKeypressEvent(sf::Event& event) {
			SplashMenuKeypressResult menuKeypressResult = this->processMenuKeypressEvent(event, *this->systemOptionsMenu);

			if (menuKeypressResult.performedActionFlag) {
				this->performSystemOptionsMenuItemAction(menuKeypressResult.actionMenuItemId);
			}
			
			if (this->mode == SplashSceneMode::SYSTEM_OPTIONS_MENU) {
				this->performSystemOptionsSliderChange();
			}
		}

		void SplashSceneController::processMouseMovedEvent(sf::Event& event) {
			sf::Vector2f mousePosition = this->window->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

			SplashMenu* currMenu = this->getCurrentMenu();

			SplashMenuMousePositionResult mousePositionResult = this->renderer->resolveMenuMousePositionResult(*currMenu, mousePosition);
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				if (mousePositionResult.overMenuItemFlag) {
					currMenu->setCurrItem(mousePositionResult.overMenuItemId);
				}
			}
			else {
				if (mousePositionResult.overSliderFlag) {
					currMenu->setItemValue(mousePositionResult.overMenuItemId, mousePositionResult.overSliderValue);
					if (this->mode == SplashSceneMode::SYSTEM_OPTIONS_MENU) {
						this->performSystemOptionsSliderChange();
					}
				}
			}
		}

		SplashSceneClientRequest SplashSceneController::processLeftMouseButtonPressedEvent(sf::Event& event) {
			SplashSceneClientRequest result = SplashSceneClientRequest::NONE;

			sf::Vector2f mousePosition = this->window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			SplashMenu* currMenu = this->getCurrentMenu();
			SplashMenuMousePositionResult mousePositionResult = this->renderer->resolveMenuMousePositionResult(*currMenu, mousePosition);

			if (mousePositionResult.overMenuItemFlag) {
				if (currMenu->getItemDefn(mousePositionResult.overMenuItemId).menuItemType == SplashMenuItemType::ACTION) {
					switch (this->mode) {
					case SplashSceneMode::MAIN_MENU:
						result = this->performMainMenuItemAction(mousePositionResult.overMenuItemId);
						break;
					case SplashSceneMode::QUICK_GAME_OPTIONS_MENU:
						this->performQuickGameOptionsMenuItemAction(mousePositionResult.overMenuItemId);
						break;
					case SplashSceneMode::SYSTEM_OPTIONS_MENU:
						this->performSystemOptionsMenuItemAction(mousePositionResult.overMenuItemId);
						break;
					}
				}
				if (
					(currMenu->getItemDefn(mousePositionResult.overMenuItemId).menuItemType == SplashMenuItemType::SLIDER) &&
					mousePositionResult.overSliderFlag
				) {
					currMenu->setItemValue(mousePositionResult.overMenuItemId, mousePositionResult.overSliderValue);
				}
			}

			return result;
		}

		SplashSceneClientRequest SplashSceneController::performMainMenuItemAction(int menuItemId) {
			SplashSceneClientRequest result = SplashSceneClientRequest::NONE;

			switch (menuItemId) {
			case SplashMainMenuId::START_QUICK_GAME:
				result = SplashSceneClientRequest::START_QUICK_GAME;
				this->freeMusic();
				break;
			case SplashMainMenuId::QUICK_GAME_OPTIONS:
				this->mode = SplashSceneMode::QUICK_GAME_OPTIONS_MENU;
				this->quickGameOptionsMenu->moveToFirstItem();
				break;
			case SplashMainMenuId::SYSTEM_OPTIONS:
				this->mode = SplashSceneMode::SYSTEM_OPTIONS_MENU;
				this->systemOptionsMenu->moveToFirstItem();
				break;
			case SplashMainMenuId::EXIT_GAME:
				result = SplashSceneClientRequest::EXIT_GAME;
				this->freeMusic();
				break;
			}

			return result;
		}

		void SplashSceneController::performQuickGameOptionsMenuItemAction(int menuItemId) {
			switch (menuItemId) {
			case SplashQuickGameOptionsMenuId::RETURN_TO_MAIN_MENU:
				this->mode = SplashSceneMode::MAIN_MENU;
				this->mainMenu->moveToFirstItem();
				break;
			}
		}

		void SplashSceneController::performSystemOptionsMenuItemAction(int menuItemId) {
			switch (menuItemId) {
			case SplashSystemOptionsMenuId::RETURN_TO_MAIN_MENU:
				this->mode = SplashSceneMode::MAIN_MENU;
				this->mainMenu->moveToFirstItem();
				break;
			}
		}

		void SplashSceneController::performSystemOptionsSliderChange() {
			SystemOptionsDefn systemOptions = this->getSystemOptions();
			this->music->setVolume((float)systemOptions.musicVolume);
		}

		SplashMenuKeypressResult SplashSceneController::processMenuKeypressEvent(sf::Event& event, SplashMenu& menu) {
			SplashMenuKeypressResult result;
			result.performedActionFlag = false;
			result.actionMenuItemId = 0;

			switch (event.key.code) {

			case sf::Keyboard::Key::Enter:
			case sf::Keyboard::Key::Space:
				if (menu.getCurrItemDefn().menuItemType == SplashMenuItemType::ACTION) {
					result.performedActionFlag = true;
					result.actionMenuItemId = menu.getCurrItemId();
				}
				break;

			case sf::Keyboard::Key::Up:
				menu.moveToPreviousItem();
				break;

			case sf::Keyboard::Key::Down:
				menu.moveToNextItem();
				break;

			case sf::Keyboard::Key::Left:
				if (menu.getCurrItemDefn().menuItemType == SplashMenuItemType::SLIDER) {
					menu.decrementItemValue(menu.getCurrItemId());
				}
				break;

			case sf::Keyboard::Key::Right:
				if (menu.getCurrItemDefn().menuItemType == SplashMenuItemType::SLIDER) {
					menu.incrementItemValue(menu.getCurrItemId());
				}
				break;

			}

			return result;
		}

		void SplashSceneController::ensureMusicLoaded() {
			if (!this->musicLoaded) {
				if (this->music == nullptr) {
					this->music = new sf::Music();
				}
				this->musicLoaded = this->music->openFromFile(SPLASH_MUSIC_PATH);
			}
		}

		void SplashSceneController::freeMusic() {
			if (this->musicLoaded) {
				this->music->stop();

				delete this->music;
				this->music = nullptr;
				this->musicLoaded = false;
			}
		}

		void SplashSceneController::beginMusic() {
			this->ensureMusicLoaded();

			this->music->setVolume((float)this->systemOptionsMenu->getItemValue(SplashSystemOptionsMenuId::MUSIC_VOLUME));
			this->music->play();
			this->music->setLoop(true);
		}

		SplashMenu* SplashSceneController::getCurrentMenu() {
			SplashMenu* result = nullptr;

			switch (this->mode) {
			case SplashSceneMode::MAIN_MENU:
				result = this->mainMenu;
				break;
			case SplashSceneMode::QUICK_GAME_OPTIONS_MENU:
				result = this->quickGameOptionsMenu;
				break;
			case SplashSceneMode::SYSTEM_OPTIONS_MENU:
				result = this->systemOptionsMenu;
				break;
			}

			return result;
		}

	}

}
