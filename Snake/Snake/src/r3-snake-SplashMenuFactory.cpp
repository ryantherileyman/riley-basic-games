
#include "includes/r3-snake-splashscene.hpp"

namespace r3 {

	namespace snake {

		namespace SplashMainMenuLabels {

			const wchar_t* START_QUICK_GAME_LABEL = L"Start Game";
			const wchar_t* START_QUICK_GAME_DESCRIPTION = L"Eat apples to become the longest snake!";

			const wchar_t* QUICK_GAME_OPTIONS_LABEL = L"Game Options";
			const wchar_t* QUICK_GAME_OPTIONS_DESCRIPTION = L"Customize your experience";

			const wchar_t* EXIT_GAME_LABEL = L"Exit Game";

		}

		namespace SplashQuickGameOptionsMenuLabels {

			const wchar_t* RETURN_TO_MAIN_MENU_LABEL = L"Return to Main Menu";

			const wchar_t* SNAKE_SPEED_LABEL = L"Snake Speed:";
			const wchar_t* SNAKE_SPEED_DESCRIPTION = L"How fast do you want to go?";

			const SplashSliderRange SNAKE_SPEED_SLOW_RANGE(4, 8);
			const wchar_t* SNAKE_SPEED_SLOW_DESCRIPTION = L"Slow - A leisurely pace";

			const SplashSliderRange SNAKE_SPEED_NORMAL_RANGE(9, 15);
			const wchar_t* SNAKE_SPEED_NORMAL_DESCRIPTION = L"Normal";

			const SplashSliderRange SNAKE_SPEED_FAST_RANGE(16, 24);
			const wchar_t* SNAKE_SPEED_FAST_DESCRIPTION = L"Fast - This snake can motor!";

			const SplashSliderRange SNAKE_SPEED_BLAZING_RANGE(25, 30);
			const wchar_t* SNAKE_SPEED_BLAZING_DESCRIPTION = L"Blazing - Slithering like a cheetah!";

			const wchar_t* SNAKE_GROWTH_LABEL = L"Growth Rate:";
			const wchar_t* SNAKE_GROWTH_DESCRIPTION = L"How much will your snake grow when eating an apple?";

			const SplashSliderRange SNAKE_GROWTH_SLOW_RANGE(1, 2);
			const wchar_t* SNAKE_GROWTH_SLOW_DESCRIPTION = L"Slow - You'll need to eat a lot of apples...";

			const SplashSliderRange SNAKE_GROWTH_NORMAL_RANGE(3, 4);
			const wchar_t* SNAKE_GROWTH_NORMAL_DESCRIPTION = L"Normal";

			const SplashSliderRange SNAKE_GROWTH_FAST_RANGE(5, 6);
			const wchar_t* SNAKE_GROWTH_FAST_DESCRIPTION = L"Fast - Careful not to hit your tail!";

			const SplashSliderRange SNAKE_GROWTH_EXTREME_RANGE(7, 10);
			const wchar_t* SNAKE_GROWTH_EXTREME_DESCRIPTION = L"Extreme - These are some seriously beefy apples";

			const wchar_t* FIELD_WIDTH_LABEL = L"Field Width:";
			const wchar_t* FIELD_HEIGHT_LABEL = L"Field Height:";

			const SplashSliderRange FIELD_SIZE_TINY_RANGE(12, 19);
			const wchar_t* FIELD_SIZE_TINY_DESCRIPTION = L"Not much room to move!";

			const SplashSliderRange FIELD_SIZE_HUGE_RANGE(81, 100);
			const wchar_t* FIELD_SIZE_HUGE_DESCRIPTION = L"You can almost go AFK getting to the next apple...";

		}

		namespace SplashMenuFactory {

			SplashMenuItemDefn createEmptyMenuItem(int menuItemId, SplashMenuItemType menuItemType) {
				SplashMenuItemDefn result;
				result.menuItemId = menuItemId;
				result.menuItemType = menuItemType;
				result.displayText = nullptr;
				result.descriptiveText = nullptr;
				return result;
			}

			SplashMenuItemDefn createActionMenuItem(int menuItemId, const wchar_t* displayText, const wchar_t* descriptiveText) {
				SplashMenuItemDefn result = createEmptyMenuItem(menuItemId, SplashMenuItemType::ACTION);
				result.displayText = displayText;
				result.descriptiveText = descriptiveText;
				return result;
			}

			SplashMenuItemDefn createEmptySliderMenuItem(int menuItemId, SplashSliderRange sliderRange) {
				SplashMenuItemDefn result = createEmptyMenuItem(menuItemId, SplashMenuItemType::SLIDER);
				result.sliderRange = sliderRange;
				return result;
			}

			SplashSliderRangeDescriptionDefn createSliderRangeDescription(SplashSliderRange sliderRange, const wchar_t* descriptiveText) {
				SplashSliderRangeDescriptionDefn result;
				result.sliderRange = sliderRange;
				result.descriptiveText = descriptiveText;
				return result;
			}

			SplashMenuItemDefn createEmptyButtonOptionsMenuItem(int menuItemId, const wchar_t* displayText, const wchar_t* descriptiveText) {
				SplashMenuItemDefn result = createEmptyMenuItem(menuItemId, SplashMenuItemType::BUTTON_OPTIONS);
				result.displayText = displayText;
				result.descriptiveText = descriptiveText;
				return result;
			}

			std::map<int, SplashMenuItemDefn> createMainMenuDefnMap() {
				std::map<int, SplashMenuItemDefn> result;

				result[SplashMainMenuId::START_QUICK_GAME] = createActionMenuItem(SplashMainMenuId::START_QUICK_GAME, SplashMainMenuLabels::START_QUICK_GAME_LABEL, SplashMainMenuLabels::START_QUICK_GAME_DESCRIPTION);
				result[SplashMainMenuId::QUICK_GAME_OPTIONS] = createActionMenuItem(SplashMainMenuId::QUICK_GAME_OPTIONS, SplashMainMenuLabels::QUICK_GAME_OPTIONS_LABEL, SplashMainMenuLabels::QUICK_GAME_OPTIONS_DESCRIPTION);
				result[SplashMainMenuId::EXIT_GAME] = createActionMenuItem(SplashMainMenuId::EXIT_GAME, SplashMainMenuLabels::EXIT_GAME_LABEL, nullptr);

				return result;
			}

			SplashMenuItemDefn createSnakeSpeedMenuItem() {
				SplashMenuItemDefn result = createEmptySliderMenuItem(SplashQuickGameOptionsMenuId::SNAKE_SPEED, SplashQuickGameOptionValues::SNAKE_SPEED);

				result.displayText = SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_LABEL;
				result.descriptiveText = SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_DESCRIPTION;

				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_SLOW_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_SLOW_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_NORMAL_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_NORMAL_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_FAST_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_FAST_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_BLAZING_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_SPEED_BLAZING_DESCRIPTION));

				return result;
			}

			SplashMenuItemDefn createFieldWidthMenuItem() {
				SplashMenuItemDefn result = createEmptySliderMenuItem(SplashQuickGameOptionsMenuId::FIELD_WIDTH, SplashQuickGameOptionValues::FIELD_WIDTH);

				result.displayText = SplashQuickGameOptionsMenuLabels::FIELD_WIDTH_LABEL;

				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::FIELD_SIZE_TINY_RANGE, SplashQuickGameOptionsMenuLabels::FIELD_SIZE_TINY_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::FIELD_SIZE_HUGE_RANGE, SplashQuickGameOptionsMenuLabels::FIELD_SIZE_HUGE_DESCRIPTION));

				return result;
			}

			SplashMenuItemDefn createFieldHeightMenuItem() {
				SplashMenuItemDefn result = createEmptySliderMenuItem(SplashQuickGameOptionsMenuId::FIELD_HEIGHT, SplashQuickGameOptionValues::FIELD_HEIGHT);

				result.displayText = SplashQuickGameOptionsMenuLabels::FIELD_HEIGHT_LABEL;

				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::FIELD_SIZE_TINY_RANGE, SplashQuickGameOptionsMenuLabels::FIELD_SIZE_TINY_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::FIELD_SIZE_HUGE_RANGE, SplashQuickGameOptionsMenuLabels::FIELD_SIZE_HUGE_DESCRIPTION));

				return result;
			}

			SplashMenuItemDefn createSnakeGrowthMenuItem() {
				SplashMenuItemDefn result = createEmptySliderMenuItem(SplashQuickGameOptionsMenuId::SNAKE_GROWTH, SplashQuickGameOptionValues::SNAKE_GROWTH);

				result.displayText = SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_LABEL;
				result.descriptiveText = SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_DESCRIPTION;

				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_SLOW_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_SLOW_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_NORMAL_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_NORMAL_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_FAST_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_FAST_DESCRIPTION));
				result.sliderRangeDescriptionDefnList.push_back(createSliderRangeDescription(SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_EXTREME_RANGE, SplashQuickGameOptionsMenuLabels::SNAKE_GROWTH_EXTREME_DESCRIPTION));

				return result;
			}

			std::map<int, SplashMenuItemDefn> createQuickGameOptionsMenuDefnMap() {
				std::map<int, SplashMenuItemDefn> result;

				result[SplashQuickGameOptionsMenuId::RETURN_TO_MAIN_MENU] = createActionMenuItem(SplashQuickGameOptionsMenuId::RETURN_TO_MAIN_MENU, SplashQuickGameOptionsMenuLabels::RETURN_TO_MAIN_MENU_LABEL, nullptr);
				result[SplashQuickGameOptionsMenuId::SNAKE_SPEED] = createSnakeSpeedMenuItem();
				result[SplashQuickGameOptionsMenuId::SNAKE_GROWTH] = createSnakeGrowthMenuItem();
				result[SplashQuickGameOptionsMenuId::FIELD_WIDTH] = createFieldWidthMenuItem();
				result[SplashQuickGameOptionsMenuId::FIELD_HEIGHT] = createFieldHeightMenuItem();

				return result;
			}

		}

	}

}
