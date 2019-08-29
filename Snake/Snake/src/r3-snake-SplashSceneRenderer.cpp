
#include <math.h>
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-splashscene.hpp"

namespace r3 {

	namespace snake {

		const char* SPLASH_UI_FONT_PATH = "resources/fonts/SourceSansPro-Regular.otf";

		const char* SPLASH_IMAGE_PATH = "resources/textures/splash-screen.png";
		const sf::Color SPLASH_BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

		const float SPLASH_MENU_TOP = 450.0f;
		const float SPLASH_MENU_ITEM_HEIGHT = 80.0f;
		const float SPLASH_MENU_ITEM_BACKGROUND_HEIGHT = 80.0f;
		const float SPLASH_MENU_ITEM_BACKGROUND_MARGIN = 32.0f;
		const float SPLASH_MENU_ITEM_SLIDER_WIDTH = 300.0f;
		const float SPLASH_MENU_ITEM_SLIDER_HANDLE_WIDTH = 5.0f;
		const float SPLASH_MENU_ITEM_SLIDER_HANDLE_HEIGHT = 50.0f;
		const float SPLASH_MENU_BUTTON_OPTION_MARGIN = 8.0f;
		const float SPLASH_MENU_ITEM_DESCRIPTION_TOP = 900.0f;
		const float SPLASH_MENU_ITEM_VALUE_DESCRIPTION_TOP = 980.0f;

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

		SplashMenuMousePositionResult SplashSceneRenderer::resolveMenuMousePositionResult(const SplashMenu& menu, sf::Vector2f cursorPosition) {
			SplashMenuMousePositionResult result;
			result.overMenuItemFlag = false;
			result.overMenuItemId = 0;
			result.overSliderFlag = false;
			result.overSliderValue = 0;
			result.overButtonOptionFlag = false;
			result.overButtonOptionId = 0;

			float highestMenuItemWidth = this->resolveHighestMenuItemWidth(menu);
			float menuWidth = highestMenuItemWidth + (SPLASH_MENU_ITEM_BACKGROUND_MARGIN * 2.0f);
			float menuLeft = (ViewUtils::VIEW_SIZE.x / 2.0f) - (menuWidth / 2.0f);

			float currTop = SPLASH_MENU_TOP;

			std::map<int, SplashMenuItemDefn> menuItemDefnMap = menu.getMenuItemDefnMap();
			for (auto const& currMenuItemDefn : menuItemDefnMap) {
				sf::FloatRect menuItemBounds(menuLeft, currTop, menuWidth, SPLASH_MENU_ITEM_BACKGROUND_HEIGHT);
				if (menuItemBounds.contains(cursorPosition)) {
					result.overMenuItemFlag = true;
					result.overMenuItemId = currMenuItemDefn.first;
				}

				if (currMenuItemDefn.second.menuItemType == SplashMenuItemType::SLIDER) {
					sf::FloatRect sliderBounds = this->resolveSliderRect(menuWidth, currTop);
					if (sliderBounds.contains(cursorPosition)) {
						float sliderPercent = (cursorPosition.x - sliderBounds.left) / sliderBounds.width;
						int possibleValuesCount = currMenuItemDefn.second.sliderRange.maxValue - currMenuItemDefn.second.sliderRange.minValue + 1;

						result.overSliderFlag = true;
						result.overSliderValue = currMenuItemDefn.second.sliderRange.minValue + (int)floorf(sliderPercent * possibleValuesCount);
					}
				}

				if (currMenuItemDefn.second.menuItemType == SplashMenuItemType::BUTTON_OPTIONS) {
					SplashButtonOptionBoundsResult buttonOptionBoundsResult = this->resolveButtonOptionBounds(currMenuItemDefn.second);

					float buttonOptionsLeftPos = (ViewUtils::VIEW_SIZE.x / 2.0f) + (menuWidth / 2.0f) - buttonOptionBoundsResult.fullWidth - SPLASH_MENU_ITEM_BACKGROUND_MARGIN;
					for (auto const& currButtonOption : currMenuItemDefn.second.buttonOptionDefnMap) {
						sf::FloatRect buttonOptionBounds = buttonOptionBoundsResult.buttonOptionBoundsMap.at(currButtonOption.first);
						buttonOptionBounds.left += buttonOptionsLeftPos;
						buttonOptionBounds.top += currTop;

						if (buttonOptionBounds.contains(cursorPosition)) {
							result.overButtonOptionFlag = true;
							result.overButtonOptionId = currButtonOption.first;
						}
					}
				}

				currTop += SPLASH_MENU_ITEM_BACKGROUND_HEIGHT;
			}

			return result;
		}

		void SplashSceneRenderer::renderMenu(sf::RenderTarget& renderTarget, const SplashMenu& menu) {
			SplashMenuItemRenderState menuItemRenderState;
			menuItemRenderState.menu = &menu;
			menuItemRenderState.top = SPLASH_MENU_TOP;
			menuItemRenderState.backgroundWidth = this->resolveHighestMenuItemWidth(menu) + (SPLASH_MENU_ITEM_BACKGROUND_MARGIN * 2.0f);

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
			sf::Text menuItemText = this->createMenuItemText(false);

			for (auto const& currMenuItemDefn : menuItemDefnMap) {
				menuItemText.setString(currMenuItemDefn.second.displayText);
				float labelWidth = FontUtils::resolveTextWidth(menuItemText);

				float itemWidth = labelWidth;
				if (currMenuItemDefn.second.menuItemType == SplashMenuItemType::SLIDER) {
					itemWidth += SPLASH_MENU_ITEM_BACKGROUND_MARGIN + SPLASH_MENU_ITEM_SLIDER_WIDTH;
				}
				else if (currMenuItemDefn.second.menuItemType == SplashMenuItemType::BUTTON_OPTIONS) {
					SplashButtonOptionBoundsResult buttonOptionBoundsResult = this->resolveButtonOptionBounds(currMenuItemDefn.second);
					itemWidth += SPLASH_MENU_ITEM_BACKGROUND_MARGIN + buttonOptionBoundsResult.fullWidth;
				}

				if (itemWidth > result) {
					result = itemWidth;
				}
			}

			return result;
		}

		void SplashSceneRenderer::renderMenuItem(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			SplashMenuItemDefn menuItemDefn = menuItemRenderState.menu->getItemDefn(menuItemRenderState.menuItemId);

			if (menuItemRenderState.isCurrItem()) {
				this->renderFocusedMenuItemBackground(renderTarget, menuItemRenderState);
			}

			this->renderMenuItemLabel(renderTarget, menuItemRenderState);
			if (menuItemDefn.menuItemType == SplashMenuItemType::SLIDER) {
				this->renderMenuItemSlider(renderTarget, menuItemRenderState);
			}
			else if (menuItemDefn.menuItemType == SplashMenuItemType::BUTTON_OPTIONS) {
				this->renderMenuItemButtonOptions(renderTarget, menuItemRenderState);
			}

			if (menuItemRenderState.isCurrItem()) {
				if (menuItemDefn.descriptiveText != nullptr) {
					this->renderMenuItemDescriptiveText(renderTarget, menuItemRenderState);
				}

				if (menuItemDefn.menuItemType == SplashMenuItemType::SLIDER) {
					this->renderMenuItemValueDescriptiveText(renderTarget, menuItemRenderState);
				}
			}
		}

		void SplashSceneRenderer::renderFocusedMenuItemBackground(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			sf::RectangleShape backgroundShape(sf::Vector2f(menuItemRenderState.backgroundWidth, SPLASH_MENU_ITEM_BACKGROUND_HEIGHT));
			backgroundShape.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (menuItemRenderState.backgroundWidth / 2.0f), menuItemRenderState.top);
			backgroundShape.setFillColor(sf::Color(0, 0, 0, 64));
			backgroundShape.setOutlineColor(sf::Color(16, 16, 128, 128));
			backgroundShape.setOutlineThickness(2.0f);

			renderTarget.draw(backgroundShape);
		}

		void SplashSceneRenderer::renderMenuItemLabel(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			sf::Text menuItemText = this->createMenuItemText(menuItemRenderState.isCurrItem());
			menuItemText.setString(menuItemRenderState.getMenuItemDefn().displayText);
			if (menuItemRenderState.getMenuItemDefn().menuItemType == SplashMenuItemType::ACTION) {
				FontUtils::setCenteredPosition(menuItemText, menuItemRenderState.top);
			}
			else {
				menuItemText.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (menuItemRenderState.backgroundWidth / 2.0f) + SPLASH_MENU_ITEM_BACKGROUND_MARGIN, menuItemRenderState.top);
			}

			renderTarget.draw(menuItemText);
		}

		void SplashSceneRenderer::renderMenuItemSlider(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			sf::FloatRect sliderBounds = this->resolveSliderRect(menuItemRenderState.backgroundWidth, menuItemRenderState.top);

			sf::RectangleShape sliderShape = this->createMenuItemSliderShape(menuItemRenderState.isCurrItem());
			sliderShape.setSize(sf::Vector2f(sliderBounds.width, SPLASH_MENU_ITEM_SLIDER_HANDLE_WIDTH));
			sliderShape.setPosition(sliderBounds.left, sliderBounds.top + (sliderBounds.height / 2.0f) - (SPLASH_MENU_ITEM_SLIDER_HANDLE_WIDTH / 2.0f));

			renderTarget.draw(sliderShape);

			int itemValue = menuItemRenderState.menu->getItemValue(menuItemRenderState.menuItemId);
			float widthPerSegment = SPLASH_MENU_ITEM_SLIDER_WIDTH / (menuItemRenderState.getMenuItemDefn().sliderRange.maxValue - menuItemRenderState.getMenuItemDefn().sliderRange.minValue + 1);
			float handleXCenter = sliderBounds.left + (widthPerSegment * (itemValue * 2.0f + 1.0f - menuItemRenderState.getMenuItemDefn().sliderRange.minValue * 2.0f)) / 2.0f;

			sf::RectangleShape sliderHandleShape = this->createMenuItemSliderShape(menuItemRenderState.isCurrItem());
			sliderHandleShape.setSize(sf::Vector2f(SPLASH_MENU_ITEM_SLIDER_HANDLE_WIDTH, SPLASH_MENU_ITEM_SLIDER_HANDLE_HEIGHT));
			sliderHandleShape.setPosition(handleXCenter - (SPLASH_MENU_ITEM_SLIDER_HANDLE_WIDTH / 2.0f), sliderBounds.top);

			renderTarget.draw(sliderHandleShape);
		}

		void SplashSceneRenderer::renderMenuItemButtonOptions(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			SplashButtonOptionBoundsResult buttonOptionBoundsResult = this->resolveButtonOptionBounds(menuItemRenderState.getMenuItemDefn());

			float buttonOptionsLeftPos = (ViewUtils::VIEW_SIZE.x / 2.0f) + (menuItemRenderState.backgroundWidth / 2.0f) - buttonOptionBoundsResult.fullWidth - SPLASH_MENU_ITEM_BACKGROUND_MARGIN;
			int itemValue = menuItemRenderState.menu->getItemValue(menuItemRenderState.menuItemId);

			for (auto const& currButtonOption : menuItemRenderState.getMenuItemDefn().buttonOptionDefnMap) {
				sf::FloatRect buttonOptionBounds = buttonOptionBoundsResult.buttonOptionBoundsMap.at(currButtonOption.first);

				sf::Text buttonOptionText = this->createButtonOptionText(currButtonOption.first == itemValue);
				buttonOptionText.setString(currButtonOption.second.displayText);
				buttonOptionText.setPosition(buttonOptionsLeftPos + buttonOptionBounds.left + SPLASH_MENU_BUTTON_OPTION_MARGIN, menuItemRenderState.top + SPLASH_MENU_BUTTON_OPTION_MARGIN);

				sf::RectangleShape buttonBorderShape(sf::Vector2f(buttonOptionBounds.width, buttonOptionBounds.height));
				buttonBorderShape.setPosition(buttonOptionsLeftPos + buttonOptionBounds.left, menuItemRenderState.top + (SPLASH_MENU_BUTTON_OPTION_MARGIN / 2.0f));
				buttonBorderShape.setFillColor(sf::Color(0, 32, 0, 64));
				buttonBorderShape.setOutlineColor(sf::Color(16, 16, 128, 128));
				buttonBorderShape.setOutlineThickness(1.0f);

				renderTarget.draw(buttonBorderShape);
				renderTarget.draw(buttonOptionText);
			}
		}

		void SplashSceneRenderer::renderMenuItemDescriptiveText(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			sf::Text descriptionText = this->createMenuItemText(menuItemRenderState.isCurrItem());
			descriptionText.setString(menuItemRenderState.getMenuItemDefn().descriptiveText);
			FontUtils::setCenteredPosition(descriptionText, SPLASH_MENU_ITEM_DESCRIPTION_TOP);

			renderTarget.draw(descriptionText);
		}

		void SplashSceneRenderer::renderMenuItemValueDescriptiveText(sf::RenderTarget& renderTarget, const SplashMenuItemRenderState menuItemRenderState) {
			const wchar_t* itemValueDescriptiveText = menuItemRenderState.menu->getItemValueDescriptiveText(menuItemRenderState.menuItemId);

			wchar_t itemValueDescriptiveString[100];
			if (itemValueDescriptiveText == nullptr) {
				swprintf_s(itemValueDescriptiveString, L"%d", menuItemRenderState.menu->getItemValue(menuItemRenderState.menuItemId));
			}
			else {
				swprintf_s(itemValueDescriptiveString, L"%d - %s", menuItemRenderState.menu->getItemValue(menuItemRenderState.menuItemId), itemValueDescriptiveText);
			}

			sf::Text descriptionText = this->createMenuItemText(menuItemRenderState.isCurrItem());
			descriptionText.setString(itemValueDescriptiveString);
			FontUtils::setCenteredPosition(descriptionText, SPLASH_MENU_ITEM_VALUE_DESCRIPTION_TOP);

			renderTarget.draw(descriptionText);
		}

		sf::FloatRect SplashSceneRenderer::resolveSliderRect(float menuWidth, float menuItemTop) {
			sf::FloatRect result;
			result.left = (ViewUtils::VIEW_SIZE.x / 2.0f) + (menuWidth / 2.0f) - SPLASH_MENU_ITEM_BACKGROUND_MARGIN - SPLASH_MENU_ITEM_SLIDER_WIDTH;
			result.top = menuItemTop + (SPLASH_MENU_ITEM_BACKGROUND_HEIGHT / 2.0f) - (SPLASH_MENU_ITEM_SLIDER_HANDLE_HEIGHT / 2.0f);
			result.width = SPLASH_MENU_ITEM_SLIDER_WIDTH;
			result.height = SPLASH_MENU_ITEM_SLIDER_HANDLE_HEIGHT;
			return result;
		}

		SplashButtonOptionBoundsResult SplashSceneRenderer::resolveButtonOptionBounds(const SplashMenuItemDefn& menuItemDefn) {
			SplashButtonOptionBoundsResult result;
			result.fullWidth = 0.0f;

			float currLeftPos = 0.0f;
			bool isFirstButtonOption = true;
			sf::Text buttonOptionText = createButtonOptionText(false);

			for (auto const& currButtonOptionDefn : menuItemDefn.buttonOptionDefnMap) {
				if (!isFirstButtonOption) {
					result.fullWidth += SPLASH_MENU_ITEM_BACKGROUND_MARGIN;
				}

				buttonOptionText.setString(currButtonOptionDefn.second.displayText);

				sf::FloatRect currRelativeBounds;
				currRelativeBounds.left = currLeftPos;
				currRelativeBounds.top = SPLASH_MENU_BUTTON_OPTION_MARGIN / 2.0f;
				currRelativeBounds.width = FontUtils::resolveTextWidth(buttonOptionText) + (SPLASH_MENU_BUTTON_OPTION_MARGIN * 2.0f);
				currRelativeBounds.height = SPLASH_MENU_ITEM_HEIGHT - SPLASH_MENU_BUTTON_OPTION_MARGIN;

				result.buttonOptionBoundsMap[currButtonOptionDefn.first] = currRelativeBounds;

				currLeftPos += currRelativeBounds.width + SPLASH_MENU_ITEM_BACKGROUND_MARGIN;

				result.fullWidth += currRelativeBounds.width;
			}

			return result;
		}

		sf::Text SplashSceneRenderer::createMenuItemText(bool currItemFlag) {
			sf::Text result;
			result.setFont(*this->uiFont);
			result.setCharacterSize(64);
			result.setOutlineColor(sf::Color::White);
			if (currItemFlag) {
				result.setFillColor(sf::Color::Yellow);
				result.setOutlineColor(sf::Color::Yellow);
			}
			return result;
		}

		sf::Text SplashSceneRenderer::createButtonOptionText(bool currOptionFlag) {
			sf::Text result = this->createMenuItemText(currOptionFlag);
			result.setCharacterSize(48);
			return result;
		}

		sf::RectangleShape SplashSceneRenderer::createMenuItemSliderShape(bool currItemFlag) {
			sf::RectangleShape result;
			result.setOutlineColor(sf::Color::White);
			result.setFillColor(sf::Color::White);
			if (currItemFlag) {
				result.setOutlineColor(sf::Color::Yellow);
				result.setFillColor(sf::Color::Yellow);
			}
			return result;
		}

	}

}
