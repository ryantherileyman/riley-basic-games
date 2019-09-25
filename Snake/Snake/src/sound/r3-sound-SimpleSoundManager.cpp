
#include "../includes/r3-sound-SimpleSoundManager.hpp"

namespace r3 {

	namespace sound {

		void SimpleSoundManager::play(const sf::SoundBuffer& soundBuffer) {
			printf("Playing sound in slot %d\n", this->nextSoundIndex);
			if (
				(this->soundArray[this->nextSoundIndex].getBuffer() != nullptr) &&
				(this->soundArray[this->nextSoundIndex].getStatus() != sf::Sound::Status::Stopped)
			) {
				printf("Previous sound in this slot did not stop yet!");
				this->soundArray[this->nextSoundIndex].stop();
			}

			this->soundArray[this->nextSoundIndex].setBuffer(soundBuffer);
			this->soundArray[this->nextSoundIndex].play();

			this->nextSoundIndex++;
			if (this->nextSoundIndex >= 128) {
				this->nextSoundIndex = 0;
			}
		}

		void SimpleSoundManager::stopAllSounds() {
			for (int index = 0; index < 128; index++) {
				if (
					(this->soundArray[index].getBuffer() != nullptr) &&
					(this->soundArray[index].getStatus() != sf::Sound::Status::Stopped)
				) {
					this->soundArray[index].stop();
				}
			}
		}

	}

}
