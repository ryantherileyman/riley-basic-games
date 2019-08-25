
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-splashscene.hpp"

namespace r3 {

	namespace snake {

		const char* SPLASH_UI_FONT_PATH = "resources/fonts/SourceSansPro-Regular.otf";

		const char* SPLASH_IMAGE_PATH = "resources/textures/splash-screen.png";
		const sf::Color SPLASH_BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

		const float SPLASH_MENU_TOP = 500.0f;
		const float SPLASH_MENU_ITEM_HEIGHT = 80.0f;
		const float SPLASH_MENU_ITEM_BACKGROUND_HEIGHT = 80.0f;
		const float SPLASH_MENU_ITEM_VERTICAL_MARGIN = 3.0f;
		const float SPLASH_MENU_ITEM_HORIZONTAL_MARGIN = 32.0f;
		const float SPLASH_MENU_ITEM_DESCRIPTION_TOP = 920.0f;

		SplashSceneRenderer::SplashSceneRenderer() {
			this->uiFont = nullptr;
			this->splashTexture = new sf::Texture();
			this->splashSprite = nullptr;

			this->uiFont = new sf::Font();
			if (!this->uiFont->loadFromFile(SPLASH_UI_FONT_PATH)) {
				throw "Could not load user interface font";
			}

			if (!this->splashTexture->loadFromFile(SPLASH_IMAGE_PATH)) {
				throw "Could not load splash screen";
			}

			this->splashSprite = new sf::Sprite();
			this->splashSprite->setTexture(*this->splashTexture);
		}

		SplashSceneRenderer::~SplashSceneRenderer() {
			delete this->uiFont;

			if (this->splashSprite != nullptr) {
				delete this->splashSprite;
			}
			delete this->splashTexture;
		}

		void SplashSceneRenderer::render(sf::RenderTarget& renderTarget, const SplashMenu& menu) {
			renderTarget.clear(SPLASH_BACKGROUND_COLOR);
			renderTarget.draw(*this->splashSprite);
			this->renderMenu(renderTarget, menu);
		}

		SplashMenuMouseCollisionResult SplashSceneRenderer::resolveMenuMouseCollisionResult(const SplashMenu& menu, sf::Vector2f cursorPosition) {
			SplashMenuMouseCollisionResult result;
			result.overMenuItemFlag = false;
			result.overMenuItemId = 0;

			float highestMenuItemWidth = this->resolveHighestMenuItemWidth(menu);
			float menuWidth = highestMenuItemWidth + (SPLASH_MENU_ITEM_HORIZONTAL_MARGIN * 2.0f);
			float menuLeft = (ViewUtils::VIEW_SIZE.x / 2.0f) - (menuWidth / 2.0f);

			float currTop = SPLASH_MENU_TOP;

			std::map<int, SplashMenuItemDefn> menuItemDefnMap = menu.getMenuItemDefnMap();
			for (auto const& currMenuItemDefn : menuItemDefnMap) {
				sf::FloatRect menuItemBounds(menuLeft, currTop, menuWidth, currTop + SPLASH_MENU_ITEM_BACKGROUND_HEIGHT);
				if (menuItemBounds.contains(cursorPosition)) {
					result.overMenuItemFlag = true;
					result.overMenuItemId = currMenuItemDefn.first;
				}

				currTop += SPLASH_MENU_ITEM_BACKGROUND_HEIGHT;
			}

			return result;
		}

		void SplashSceneRenderer::renderMenu(sf::RenderTarget& renderTarget, const SplashMenu& menu) {
			SplashMenuItemRenderState menuItemRenderState;
			menuItemRenderState.menu = &menu;
			menuItemRenderState.top = SPLASH_MENU_TOP;
			menuItemRenderState.backgroundWidth = this->resolveHighestMenuItemWidth(menu) + (SPLASH_MENU_ITEM_HORIZONTAL_MARGIN * 2.0f);

			std::map<int, SplashMenuItemDefn> menuItemDefnMap = menu.getMenuItemDefnMap();

			for (auto const& currMenuItemDefn : menuItemDefnMap) {
				menuItemRenderState.menuItemId = currMenuItemDefn.first;
				this->renderMenuItem(renderTarget, menuItemRenderState);

				menuItemRenderState.top += SPLASH_MENU_ITEM_HEIGHT;
			}
		}

		float SplashSceneRenderer::resolveHighestMenuItemWidth(const SplashMenu& menu) {
			float result = 0.0f;

			std::map<int, SplashMenuItemDefn> menuItemDefnMap = menu.getMenuItemDefnMap();
			sf::Text menuItemText = this->createMenuItemText();

			for (auto const& currMenuItemDefn : menuItemDefnMap) {
				menuItemText.setString(currMenuItemDefn.second.displayText);
				float labelWidth = FontUtils::resolveTextWidth(menuItemText);
				if (labelWidth > result) {
					result = labelWidth;
				}
			}

			return result;
		}

		void SplashSceneRenderer::renderMenuItem(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			SplashMenuItemDefn menuItemDefn = menuItemRenderState.menu->getItemDefn(menuItemRenderState.menuItemId);

			if (menuItemRenderState.menu->getCurrItemId() == menuItemRenderState.menuItemId) {
				sf::RectangleShape backgroundShape(sf::Vector2f(menuItemRenderState.backgroundWidth, SPLASH_MENU_ITEM_BACKGROUND_HEIGHT));
				backgroundShape.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (menuItemRenderState.backgroundWidth / 2.0f), menuItemRenderState.top);
				backgroundShape.setFillColor(sf::Color(0, 0, 0, 64));
				backgroundShape.setOutlineColor(sf::Color(16, 16, 128, 128));
				backgroundShape.setOutlineThickness(2.0f);

				renderTarget.draw(backgroundShape);
			}

			sf::Text menuItemText = this->createMenuItemText();
			if (menuItemRenderState.menu->getCurrItemId() == menuItemRenderState.menuItemId) {
				menuItemText.setFillColor(sf::Color::Yellow);
				menuItemText.setOutlineColor(sf::Color::Yellow);
			}
			menuItemText.setString(menuItemDefn.displayText);
			float labelWidth = FontUtils::resolveTextWidth(menuItemText);
			menuItemText.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (labelWidth / 2.0f), menuItemRenderState.top);
			renderTarget.draw(menuItemText);

			if (menuItemRenderState.menu->getCurrItemId() == menuItemRenderState.menuItemId) {
				if (menuItemDefn.descriptiveText != nullptr) {
					menuItemText.setString(menuItemDefn.descriptiveText);
					float descriptionWidth = FontUtils::resolveTextWidth(menuItemText);
					menuItemText.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (descriptionWidth / 2.0f), SPLASH_MENU_ITEM_DESCRIPTION_TOP);
					renderTarget.draw(menuItemText);
				}
			}
		}

		sf::Text SplashSceneRenderer::createMenuItemText() {
			sf::Text result;
			result.setFont(*this->uiFont);
			result.setCharacterSize(64);
			result.setOutlineColor(sf::Color::White);
			return result;
		}

	}

}
