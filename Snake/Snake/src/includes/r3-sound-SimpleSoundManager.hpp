
#include <SFML/Audio.hpp>
#pragma once

namespace r3 {

	namespace sound {

		class SimpleSoundManager {

		private:
			int nextSoundIndex;
			sf::Sound soundArray[128];
			int volume;

		public:
			SimpleSoundManager();

		public:
			void setVolume(int volume);
			void play(const sf::SoundBuffer& soundBuffer);
			void stopAllSounds();

		};

	}

}
