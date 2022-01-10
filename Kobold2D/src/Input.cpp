#include "Input.h"
#include <SDL.h>

Input::Input() :
	rawKeyMap
	{
		{ SDLK_UP, Keys::UP },
		{ SDLK_DOWN, Keys::DOWN },
		{ SDLK_LEFT, Keys::LEFT },
		{ SDLK_RIGHT, Keys::RIGHT },
		
		{ SDLK_SPACE, Keys::SPACE },
		
		{ SDLK_1, Keys::ONE },
		{ SDLK_2, Keys::TWO },
		{ SDLK_3, Keys::THREE },
		{ SDLK_4, Keys::FOUR },
		{ SDLK_5, Keys::FIVE },
		{ SDLK_6, Keys::SIX },
		{ SDLK_7, Keys::SEVEN },
		{ SDLK_8, Keys::EIGHT },
		{ SDLK_9, Keys::NINE },
		{ SDLK_0, Keys::ZERO },

		{ SDLK_q, Keys::Q },
		{ SDLK_w, Keys::W },
		{ SDLK_e, Keys::E },
		{ SDLK_r, Keys::R },
		{ SDLK_t, Keys::T },
		{ SDLK_y, Keys::Y },
		{ SDLK_u, Keys::U },
		{ SDLK_i, Keys::I },
		{ SDLK_o, Keys::O },
		{ SDLK_p, Keys::P },

		{ SDLK_a, Keys::A },
		{ SDLK_s, Keys::S },
		{ SDLK_d, Keys::D },
		{ SDLK_f, Keys::F },
		{ SDLK_g, Keys::G },
		{ SDLK_h, Keys::H },
		{ SDLK_j, Keys::J },
		{ SDLK_k, Keys::K },
		{ SDLK_l, Keys::L },

		{ SDLK_z, Keys::Z },
		{ SDLK_x, Keys::X },
		{ SDLK_c, Keys::C },
		{ SDLK_v, Keys::V },
		{ SDLK_b, Keys::B },
		{ SDLK_n, Keys::N },
		{ SDLK_m, Keys::M },

		{ SDLK_F1, Keys::F1 },
		{ SDLK_F2, Keys::F2 },
		{ SDLK_F3, Keys::F3 },
		{ SDLK_F4, Keys::F4 },
		{ SDLK_F5, Keys::F5 },
		{ SDLK_F6, Keys::F6 },
		{ SDLK_F7, Keys::F7 },
		{ SDLK_F8, Keys::F8 },
		{ SDLK_F9, Keys::F9 },
		{ SDLK_F10, Keys::F10 },
		{ SDLK_F11, Keys::F11 },
		{ SDLK_F12, Keys::F12 },
	}
{}

Keys Input::GetKeyMapping(SDL_Keycode keycode) const
{
	std::map<SDL_Keycode, Keys>::const_iterator it = rawKeyMap.find(keycode);

	if (it != rawKeyMap.end())
		return it->second;
	else
		return Keys::INVALID;
}
