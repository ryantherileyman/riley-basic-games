
#include <SFML/Audio.hpp>
#pragma once

namespace r3 {

	namespace sound {

		class SimpleSoundManager {

		private:
			int nextSoundIndex;
			sf::Sound soundArray[128];

		public:
			void play(const sf::SoundBuffer& soundBuffer);
			void stopAllSounds();

		};

	}

}
