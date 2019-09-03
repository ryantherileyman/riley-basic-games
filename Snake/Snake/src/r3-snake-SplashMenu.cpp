
#include <assert.h>
#include "includes/r3-snake-splashscene.hpp"

namespace r3 {

	namespace snake {

		SplashMenu::SplashMenu(const std::map<int, SplashMenuItemDefn>& menuItemDefnList) {
			assert(menuItemDefnList.size() > 0);

			for (auto const& currMenuItemDefn : menuItemDefnList) {
				this->menuItemDefnMap[currMenuItemDefn.first] = currMenuItemDefn.second;

				switch (currMenuItemDefn.second.menuItemType) {
				case SplashMenuItemType::SLIDER:
					this->chosenValueMap[currMenuItemDefn.first] = currMenuItemDefn.second.sliderRange.minValue;
					break;
				case SplashMenuItemType::BUTTON_OPTIONS:
				case SplashMenuItemType::NAVIGABLE_OPTIONS:
					this->chosenValueMap[currMenuItemDefn.first] = currMenuItemDefn.second.buttonOptionDefnMap.begin()->first;
					break;
				}
			}

			this->currItemId = this->menuItemDefnMap.begin()->first;
		}

		std::map<int, SplashMenuItemDefn> SplashMenu::getMenuItemDefnMap() const {
			return this->menuItemDefnMap;
		}

		SplashMenuItemDefn SplashMenu::getItemDefn(int menuItemId) const {
			SplashMenuItemDefn result = this->menuItemDefnMap.at(menuItemId);
			return result;
		}

		int SplashMenu::getCurrItemId() const {
			return this->currItemId;
		}

		SplashMenuItemDefn SplashMenu::getCurrItemDefn() const {
			SplashMenuItemDefn result = this->menuItemDefnMap.at(this->currItemId);
			return result;
		}

		int SplashMenu::moveToFirstItem() {
			this->currItemId = this->menuItemDefnMap.begin()->first;

			return this->currItemId;
		}

		void SplashMenu::setCurrItem(int menuItemId) {
			assert(this->menuItemDefnMap.count(menuItemId) == 1);
			this->currItemId = menuItemId;
		}

		int SplashMenu::moveToNextItem() {
			std::map<int, SplashMenuItemDefn>::iterator nextIterator = ++this->menuItemDefnMap.find(this->currItemId);
			if (nextIterator == this->menuItemDefnMap.end()) {
				nextIterator = this->menuItemDefnMap.begin();
			}
			this->currItemId = nextIterator->first;

			return this->currItemId;
		}

		int SplashMenu::moveToPreviousItem() {
			std::map<int, SplashMenuItemDefn>::iterator prevIterator = --this->menuItemDefnMap.find(this->currItemId);
			if (prevIterator == this->menuItemDefnMap.end()) {
				--prevIterator;
			}
			this->currItemId = prevIterator->first;

			return this->currItemId;
		}

		int SplashMenu::getItemValue(int menuItemId) const {
			SplashMenuItemDefn menuItemDefn = this->assertMenuItemAllowsValue(menuItemId);

			int result = this->chosenValueMap.at(menuItemId);
			return result;
		}

		const wchar_t* SplashMenu::getItemValueDescriptiveText(int menuItemId) const {
			SplashMenuItemDefn menuItemDefn = this->assertMenuItemAllowsValue(menuItemId);

			const wchar_t* result = nullptr;

			switch (menuItemDefn.menuItemType) {
			case SplashMenuItemType::SLIDER:
			{
				int value = this->chosenValueMap.at(menuItemId);
				for (auto const& currSliderRangeDescriptionDefn : menuItemDefn.sliderRangeDescriptionDefnList) {
					if (currSliderRangeDescriptionDefn.sliderRange.inRange(value)) {
						assert(result == nullptr);
						result = currSliderRangeDescriptionDefn.descriptiveText;
					}
				}
			}
				break;
			case SplashMenuItemType::BUTTON_OPTIONS:
			case SplashMenuItemType::NAVIGABLE_OPTIONS:
			{
				int value = this->chosenValueMap.at(menuItemId);
				SplashButtonOptionDefn buttonOptionDefn = menuItemDefn.buttonOptionDefnMap.at(value);
				result = buttonOptionDefn.descriptiveText;
			}
				break;
			}

			return result;
		}

		void SplashMenu::setItemValue(int menuItemId, int value) {
			SplashMenuItemDefn menuItemDefn = this->assertMenuItemAllowsValue(menuItemId);

			switch (menuItemDefn.menuItemType) {
			case SplashMenuItemType::SLIDER:
				assert(value >= menuItemDefn.sliderRange.minValue);
				assert(value <= menuItemDefn.sliderRange.maxValue);
				break;
			case SplashMenuItemType::BUTTON_OPTIONS:
			case SplashMenuItemType::NAVIGABLE_OPTIONS:
				assert(menuItemDefn.buttonOptionDefnMap.count(value) == 1);
				break;
			}

			this->chosenValueMap[menuItemId] = value;
		}

		int SplashMenu::incrementItemValue(int menuItemId) {
			SplashMenuItemDefn menuItemDefn = this->assertMenuItemAllowsValue(menuItemId);

			int result = 0;

			switch (menuItemDefn.menuItemType) {
			case SplashMenuItemType::SLIDER:
			{
				int currValue = this->chosenValueMap.at(menuItemId);

				result = currValue;
				if (result < menuItemDefn.sliderRange.maxValue) {
					result++;
					this->chosenValueMap[menuItemId] = result;
				}
			}
				break;
			case SplashMenuItemType::BUTTON_OPTIONS:
			case SplashMenuItemType::NAVIGABLE_OPTIONS:
			{
				int currValue = this->chosenValueMap.at(menuItemId);

				std::map<int, SplashButtonOptionDefn>::iterator nextButtonOptionDefnIterator = ++menuItemDefn.buttonOptionDefnMap.find(currValue);

				result = currValue;
				if (nextButtonOptionDefnIterator != menuItemDefn.buttonOptionDefnMap.end()) {
					result = nextButtonOptionDefnIterator->first;
				} else {
					result = menuItemDefn.buttonOptionDefnMap.begin()->first;
				}
				this->chosenValueMap[menuItemId] = result;
			}
				break;
			}

			return result;
		}

		int SplashMenu::decrementItemValue(int menuItemId) {
			SplashMenuItemDefn menuItemDefn = this->assertMenuItemAllowsValue(menuItemId);

			int result = 0;

			switch (menuItemDefn.menuItemType) {
			case SplashMenuItemType::SLIDER:
			{
				int currValue = this->chosenValueMap.at(menuItemId);

				result = currValue;
				if (result > menuItemDefn.sliderRange.minValue) {
					result--;
					this->chosenValueMap[menuItemId] = result;
				}
			}
			break;
			case SplashMenuItemType::BUTTON_OPTIONS:
			case SplashMenuItemType::NAVIGABLE_OPTIONS:
			{
				int currValue = this->chosenValueMap.at(menuItemId);

				std::map<int, SplashButtonOptionDefn>::iterator prevButtonOptionDefnIterator = --menuItemDefn.buttonOptionDefnMap.find(currValue);

				result = currValue;
				if (prevButtonOptionDefnIterator != menuItemDefn.buttonOptionDefnMap.end()) {
					result = prevButtonOptionDefnIterator->first;
				} else {
					result = (--menuItemDefn.buttonOptionDefnMap.end())->first;
				}
				this->chosenValueMap[menuItemId] = result;
			}
			break;
			}

			return result;
		}

		SplashMenuItemDefn SplashMenu::assertMenuItemAllowsValue(int menuItemId) const {
			SplashMenuItemDefn menuItemDefn = this->menuItemDefnMap.at(menuItemId);

			bool menuItemHasValue = (menuItemDefn.menuItemType != SplashMenuItemType::ACTION);
			assert(menuItemHasValue);

			return menuItemDefn;
		}

	}

}
