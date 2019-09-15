
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

#include "includes/r3-snake-client.hpp"
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-splashscene.hpp"
#include "includes/r3-snake-quickgamescene.hpp"
#include "includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		const wchar_t* WINDOW_TITLE = L"Riley Entertainment - The Garden of Eating";
		const unsigned int WINDOW_INITIAL_WIDTH = 800;
		const unsigned int WINDOW_INITIAL_HEIGHT = 450;

		const sf::Int64 MICROSECONDS_PER_FRAME = 1000000 / 60;

		static const struct {
			unsigned int 	 width;
			unsigned int 	 height;
			unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
			unsigned char	 pixel_data[32 * 32 * 4 + 1];
		} SnakeIcon = {
		  32, 32, 4,
		  "\000\211\003\377\000\207\003\377\000\210\003\377\000\211\003\377\000\206\003\377\000\177\003\377"
		  "\000\206\003\377\000\211\003\377\000\210\003\377\000\211\003\377\000\207\003\377\000|\002\377\000\202"
		  "\002\377\000\211\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\211\003\377\000\212\003\377\000\210"
		  "\003\377\000\200\002\377\000\203\002\377\000\211\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\211\003\377\000\207\003\377\000\211\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\211\003\377\000\210\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\207\003\377\000\213"
		  "\003\377\000\230\003\377\000\213\003\377\000\233\003\377\000\225\003\377\000\206\003\377\000\210"
		  "\003\377\000\210\003\377\000\205\003\377\000\206\003\377\000\207\003\377\000\210\003\377\000\207"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\212\003\377\000\214\003\377\000\211"
		  "\003\377\000\210\003\377\000\211\003\377\000\212\003\377\000\210\003\377\000\207\003\377\000\210"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\211\003\377\000\205\003\377\000\177\002\377\000\204\003\377\000{\002\377\000\177\002\377"
		  "\000\211\003\377\000\207\003\377\000\212\003\377\000\234\003\377\000\223\003\377\000\210\003\377"
		  "\000\210\003\377\000\210\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\216\003\377"
		  "\000\223\003\377\000\210\003\377\000\207\003\377\000\213\003\377\000\226\003\377\000\225\003\377"
		  "\000\250\003\377\000\220\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\210\003\377"
		  "\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\206\003\377\000\210\003\377"
		  "\000\210\003\377\000\207\003\377\000\210\003\377\000\207\003\377\000\204\002\377\000\213\003\377"
		  "\000\211\003\377\000\207\003\377\000\210\003\377\000\207\003\377\000\210\003\377\000\207\003\377"
		  "\000\211\003\377\000\201\002\377\000|\002\377\000\207\003\377\000\211\003\377\000\205\003\377\000~"
		  "\002\377\000\205\002\377\000\206\002\377\000\203\002\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\207\003\377\000\213\003\377\000\234"
		  "\004\377\000\216\002\377\000\202\003\377\000\207\003\377\000\206\003\377\000\207\003\377\000\207"
		  "\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\207\003\377\000\207\003\377\000\207"
		  "\003\377\000\207\003\377\000\210\003\377\000\177\003\377\000y\003\377\000\204\003\377\000\207\003\377"
		  "\000\210\003\377\000\210\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\210\003\377"
		  "\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\207\003\377"
		  "\000\214\003\377\000\243\004\377\000\253\004\377\000\224\003\377\000\210\003\377\000\210\003\377"
		  "\000\210\003\377\000\207\003\377\000\207\003\377\000\206\003\377\000\211\003\377\000\223\003\377"
		  "\000\211\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\211\003\377\000\212\003\377"
		  "\000\213\003\377\000\214\003\377\000\210\003\377\000\211\003\377\000\214\003\377\000\213\003\377"
		  "\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377"
		  "\000\210\003\377\000\211\003\377\000\201\002\377\000m\002\377\000j\002\377\000\177\003\377\000\211"
		  "\003\377\000\227\003\377\000\212\003\377\000\206\003\377\000\207\003\377\000\207\003\377\000\207"
		  "\003\377\000\214\002\377\000\206\003\377\000\223\003\377\000\222\003\377\000\206\003\377\000\207"
		  "\003\377\000\207\003\377\000\215\003\377\000\223\003\377\000\210\003\377\000\210\003\377\000\223"
		  "\003\377\000\216\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210"
		  "\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\207\003\377\000\205"
		  "\003\377\000\206\003\377\000\204\003\377\000\177\002\377\000\205\003\377\000\210\003\377\000\207"
		  "\003\377\000\210\003\377\000\205\003\377\000~\003\377\000\205\003\377\000\201\002\377\000\201\002\377"
		  "\000\210\003\377\000\207\003\377\000\210\003\377\000\201\002\377\000|\002\377\000\210\003\377\000\207"
		  "\003\377\000|\002\377\000\201\003\377\000\211\003\377\000\207\003\377\000\210\003\377\000\210\003\377"
		  "\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\210\003\377"
		  "\000\211\003\377\000\210\003\377\000\207\003\377\000\205\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\206\003\377"
		  "\000\206\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\210\003\377\000\210\003\377"
		  "\000\207\003\377\000\207\003\377\000\211\003\377\000\207\003\377\000\207\003\377\000\210\003\377"
		  "\000\210\003\377\000\210\003\377\000\210\003\377\000\207\003\377\000\210\003\377\000\210\003\377"
		  "\000\207\003\377\000\210\003\377\000\210\003\377\000\210\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\206\003\377\000\206\003\377\000\210\003\377"
		  "\000\206\003\377\000\206\003\377\000\207\003\377\000\210\003\377\000\210\003\377\000\210\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377"
		  "\000\207\003\377\000\207\003\377\000\207\003\377\000\206\003\377\000\211\003\377\000\225\003\377"
		  "\000\217\003\377\000\205\003\377\000\217\003\377\000\226\003\377\000\211\003\377\000\206\003\377"
		  "\000\207\003\377\000\207\003\377lcQ\377lcQ\377lcQ\377lcQ\377lcQ\377lcQ\377eeM\377"
		  "Sk@\377\066u*\377\021\201\017\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207"
		  "\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\203\003\377\000\177"
		  "\003\377\000\205\003\377\005\200\003\377\023|\005\377\000\204\002\377\000t\002\377\004M\002\377\015"
		  "\066\001\377\015\066\001\377\006M\002\377\000w\003\377\000\207\003\377\000\207\003\377k\\R\377l"
		  "[Q\377l\\Q\377l\\R\377l[Q\377k\\Q\377m]R\377m]S\377l\\R\377d_L\377<n.\377"
		  "\011\203\011\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377\000\207\003\377"
		  "\001Y\002\377'&\001\377@\036\001\377\035+\001\377\013X\003\377&_\010\377\001U\002\377b\023\000\377"
		  "\277\001\000\377\320\001\000\377\321\001\000\377\304\004\003\377V\025\001\377\000c\002\377\000\207"
		  "\003\377aSI\377cTJ\377cUK\377cTJ\377cTJ\377eUK\377dUL\377`RI\377gWM\377dTK"
		  "\377cTJ\377V[B\377\025{\022\377\000\207\003\377\000\207\003\377\000\207\003\377\010N\002\377"
		  "\222\004\000\377\363\001\000\377\376\001\000\377\360\001\000\377q\021\000\377\034)\005\377\220"
		  "\010\001\377\377\000\000\377\377\001\000\377\377\001\000\377\377\000\000\377\377\021\020\377\376"
		  "\"#\377S\024\000\377\000r\003\377YMD\377[MD\377[NE\377[ME\377]PG\377VH@\377\037\032"
		  "\027\377\013\012\011\377\060)%\377]MF\377\\ME\377[ND\377JY\070\377\010\202\010\377"
		  "\000\207\003\377\000k\002\377z\016\000\377\377\001\000\377\377\001\000\377\377\001\000\377\377\001"
		  "\000\377\374\000\000\377\305\000\000\377\370\001\000\377\377\001\000\377\375\001\000\377\375\001"
		  "\000\377\375\001\000\377\375\000\000\377\377VV\377\313\027\027\377\015\064\001\377SG?\377"
		  "SF?\377RG>\377RF>\377SG@\377NC<\377!\033\030\377\006\005\004\377!\034\031\377SG>\377"
		  "SG?\377SF>\377SG>\377-c#\377\000\207\003\377\010B\001\377\304\001\000\377\377\001\000\377"
		  "\375\001\000\377\375\001\000\377\375\001\000\377\376\001\000\377\377\001\000\377\376\001\000\377"
		  "\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\000\000\377\377NL\377\363"
		  "ON\377\065\026\000\377H<\065\377K@\071\377J?\070\377J?\070\377J?\070\377K?\071\377"
		  "OB;\377J>\070\377LA:\377J@\071\377J?\070\377K?\070\377K?\070\377BG\062\377\000\207"
		  "\003\377\014\070\001\377\313\001\000\377\377\001\000\377\375\001\000\377\375\001\000\377\375\001"
		  "\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001"
		  "\000\377\375\001\000\377\375\000\000\377\377\061\060\377\365VV\377\070\025\000\377cXQ\377"
		  "@\065\060\377B\070\061\377B\070\061\377B\070\062\377B\070\062\377B\070\062\377C\071\063"
		  "\377C\070\061\377B\070\062\377B\070\062\377B\070\062\377B\070\062\377>=/\377\000\207"
		  "\003\377\005K\002\377\275\001\000\377\377\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377"
		  "\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377"
		  "\375\001\000\377\375\000\000\377\377\064\063\377\336\071\071\377\033%\001\377ne]\377?\065"
		  "/\377B\067\062\377B\067\061\377B\070\061\377B\067\062\377B\070\062\377D\071\062\377B"
		  "\067\062\377B\067\061\377B\070\062\377B\070\061\377B\070\061\377==/\377\000\207\003\377"
		  "\000g\002\377\216\011\000\377\377\001\000\377\376\001\000\377\375\001\000\377\375\001\000\377\375"
		  "\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375"
		  "\001\000\377\375\000\000\377\377\035\035\377\264\014\014\377\004D\002\377D\071\062\377I>\067"
		  "\377H=\067\377I>\067\377I>\067\377J?\070\377MB:\377I=\067\377K?\071\377I=\067\377"
		  "I>\067\377I>\067\377I>\066\377CD\063\377\000\210\003\377\000~\003\377=\037\001\377\371\001"
		  "\000\377\377\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001"
		  "\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\376\001\000\377\377\000"
		  "\000\377q\014\000\377\000m\002\377QF>\377QE>\377QE=\377QE=\377TG?\377K?\070\377\032"
		  "\027\024\377\006\005\004\377\037\032\027\377RF=\377RF>\377QE=\377QE>\377\060_&\377\000"
		  "\206\003\377\000\207\003\377\000I\002\377\261\001\000\377\377\001\000\377\375\001\000\377\375\001"
		  "\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001"
		  "\000\377\375\001\000\377\377\001\000\377\350\001\000\377\034\064\001\377\000\207\003\377YKC\377"
		  "YLC\377YLD\377YLC\377[NE\377TF?\377\037\032\027\377\013\012\010\377\061*%\377[N"
		  "E\377ZLD\377YLC\377NT;\377\013~\013\377\000\206\003\377\000\206\003\377\000\200\003\377"
		  "\060$\001\377\370\001\000\377\377\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375"
		  "\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\375\001\000\377\377\001\000\377s\012"
		  "\000\377\000j\002\377\000\210\003\377`QH\377aRI\377aSJ\377aSJ\377aSJ\377cUK\377bTK"
		  "\377_QH\377eWL\377bTJ\377aSJ\377XWC\377\035v\030\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\207\003\377\000f\002\377p\013\000\377\377\001\000\377\377\001\000\377\377\001"
		  "\000\377\377\001\000\377\377\001\000\377\376\001\000\377\376\001\000\377\377\001\000\377\377\001"
		  "\000\377\266\001\000\377\007K\002\377\000\207\003\377\000\206\003\377j[Q\377j[Q\377jZP\377"
		  "jZQ\377j[Q\377j[P\377k[Q\377l\\R\377jZP\377k[P\377Of=\377\017\200\016\377\000"
		  "\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\205\003\377\000\207\003\377\004"
		  "c\002\377\212\011\000\377\363\001\000\377\377\001\000\377\353\001\000\377\317\001\000\377\373"
		  "\001\000\377\377\001\000\377\367\001\000\377\252\003\000\377\016A\001\377\000\204\003\377\000\206"
		  "\003\377\000\207\003\377kcQ\377lcQ\377kcQ\377lcQ\377lcQ\377kcQ\377efM\377Rl?\377"
		  "\071z,\377\040\177\032\377\006\205\010\377\000\226\003\377\000\211\003\377\000\205\003\377"
		  "\000\206\003\377\000\206\003\377\000\207\003\377\000\204\003\377\000|\002\377\001f\002\377\062\035"
		  "\001\377q\013\000\377\071\036\001\377\021\062\001\377e\020\000\377|\013\000\377?\025\000\377\004"
		  "R\002\377\000\205\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206"
		  "\003\377\000\206\003\377\000\206\003\377\000\207\003\377\000\177\002\377\000{\002\377\000\203\002\377"
		  "\000z\002\377\000\200\003\377\000\204\003\377\000\177\002\377\000\204\003\377\000\207\003\377\000\206"
		  "\003\377\000\206\003\377\000\206\003\377\000\207\003\377\000\206\003\377\000{\002\377\000\177\002\377"
		  "\000\207\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\207\003\377"
		  "\000\206\003\377\000\207\003\377\000\206\003\377\000\205\003\377\000\203\003\377\000\205\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\207\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\205\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\207\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\207\003\377\000\206\003\377\000\205\003\377\000\206\003\377"
		  "\000\205\003\377\000\211\003\377\000\225\003\377\000\210\003\377\000\205\003\377\000\206\003\377"
		  "\000\205\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\205\003\377\000\206\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\205\003\377\000\204\003\377\000\205\003\377"
		  "\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\205\003\377"
		  "\000\205\003\377\000\206\003\377\000\206\003\377\000\203\003\377\000~\002\377\000\204\003\377\000\207"
		  "\003\377\000\210\003\377\000\216\003\377\000\210\003\377\000\206\003\377\000\206\003\377\000\206"
		  "\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\207"
		  "\003\377\000\206\003\377\000\206\003\377\000\205\003\377\000\204\003\377\000\210\003\377\000\225"
		  "\003\377\000\216\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206"
		  "\003\377\000\210\003\377\000\226\003\377\000\210\003\377\000\204\003\377\000\204\003\377\000\202"
		  "\003\377\000\205\003\377\000\205\003\377\000\206\003\377\000\215\003\377\000\206\003\377\000\205"
		  "\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\206\003\377\000\205"
		  "\003\377\000\222\003\377\000\231\003\377\000\206\003\377\000\205\003\377\000\215\003\377\000\222"
		  "\003\377\000\216\003\377\000\246\004\377\000\232\003\377\000\205\003\377\000\205\003\377\000\206"
		  "\003\377\000\206\003\377\000\206\003\377",
		};

		GameClient::GameClient() {
			this->mode = ClientMode::SPLASH_SCREEN;

			this->window.create(sf::VideoMode(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT), WINDOW_TITLE);
			this->window.setIcon(SnakeIcon.width, SnakeIcon.height, SnakeIcon.pixel_data);

			this->splashSceneController = new SplashSceneController(this->window);
			this->quickGameController = new QuickGameController(this->window);
			this->storyGameController = new StoryGameController(this->window);
		}

		GameClient::~GameClient() {
			delete this->splashSceneController;
			delete this->quickGameController;
			delete this->storyGameController;
		}

		void GameClient::run() {
			sf::Clock clock;

			while (window.isOpen()) {
				clock.restart();

				sf::Event event;
				while (window.pollEvent(event)) {
					switch (this->mode) {
					case ClientMode::SPLASH_SCREEN:
						this->processSplashScreenEvent(event);
						break;
					case ClientMode::QUICK_GAME:
						this->processQuickGameEvent(event);
						break;
					case ClientMode::STORY_GAME:
						this->processStoryGameEvent(event);
						break;
					}
				}

				if (window.isOpen()) {
					switch (this->mode) {
					case ClientMode::SPLASH_SCREEN:
						this->splashSceneController->update();
						this->splashSceneController->render();
						break;
					case ClientMode::QUICK_GAME:
						this->quickGameController->update();
						this->quickGameController->render();
						break;
					case ClientMode::STORY_GAME:
						this->storyGameController->update();
						this->storyGameController->render();
						break;
					}

				}

				sf::Int64 microSecondsElapsed = clock.getElapsedTime().asMicroseconds();
				printf("microseconds elapsed this frame = %lld\n", microSecondsElapsed);
				if (microSecondsElapsed < MICROSECONDS_PER_FRAME) {
					std::this_thread::sleep_for(std::chrono::microseconds(MICROSECONDS_PER_FRAME - microSecondsElapsed));
				}
			}
		}

		void GameClient::processSplashScreenEvent(sf::Event& event) {
			SplashSceneClientRequest request = this->splashSceneController->processEvent(event);

			switch (request) {
			case SplashSceneClientRequest::EXIT_GAME:
				window.close();
				break;
			case SplashSceneClientRequest::START_QUICK_GAME:
				this->quickGameController->setQuickGameOptions(this->splashSceneController->getQuickGameOptions());
				this->quickGameController->setSystemOptions(this->splashSceneController->getSystemOptions());
				this->mode = ClientMode::QUICK_GAME;
				break;
			case SplashSceneClientRequest::START_STORY_GAME:
				this->storyGameController->setCampaignFolder(this->splashSceneController->getStoryCampaignFolder());
				this->storyGameController->setSystemOptions(this->splashSceneController->getSystemOptions());
				this->mode = ClientMode::STORY_GAME;
				break;
			case SplashSceneClientRequest::SWITCH_TO_WINDOW:
				this->window.close();
				this->window.create(sf::VideoMode(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT), WINDOW_TITLE);
				this->window.setIcon(SnakeIcon.width, SnakeIcon.height, SnakeIcon.pixel_data);
				this->window.setView(ViewUtils::createView(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT));
				break;
			case SplashSceneClientRequest::SWITCH_TO_FULLSCREEN:
				sf::VideoMode fullScreenVideoMode = sf::VideoMode::getDesktopMode();

				this->window.close();
				this->window.create(fullScreenVideoMode, WINDOW_TITLE, sf::Style::Fullscreen);
				this->window.setView(ViewUtils::createView(fullScreenVideoMode.width, fullScreenVideoMode.height));
				break;
			}
		}

		void GameClient::processQuickGameEvent(sf::Event& event) {
			QuickGameSceneClientRequest request = this->quickGameController->processEvent(event);

			switch (request) {
			case QuickGameSceneClientRequest::EXIT_GAME:
				window.close();
				break;
			case QuickGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN:
				this->mode = ClientMode::SPLASH_SCREEN;
				break;
			}
		}

		void GameClient::processStoryGameEvent(sf::Event& event) {
			StoryGameSceneClientRequest request = this->storyGameController->processEvent(event);

			switch (request) {
			case StoryGameSceneClientRequest::EXIT_GAME:
				window.close();
				break;
			case StoryGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN:
				this->mode = ClientMode::SPLASH_SCREEN;
				break;
			}
		}

	}

}
