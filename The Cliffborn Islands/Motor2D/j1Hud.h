#include "j1Timer.h"
#include "p2SString.h"
#include "p2List.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2Animation.h"

class j1Button;
class j1Label;

struct _TTF_Font;
struct SDL_Texture;

class j1Hud
{
public:
	j1Hud();
	~j1Hud();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:
	SDL_Texture* sprites = nullptr;
	SDL_Texture* lives_tex = nullptr;
	j1Timer	startup_time;
	
	p2SString time_text;
	p2SString min_text;
	p2SString min_text_left = "0";
	p2SString score;

	j1Label* seconds;
	j1Label* minutes;
	j1Label* score_label = nullptr;

	_TTF_Font* text = nullptr;
	
	int time;
	int min = 00;

	Animation* animation = nullptr;
	Animation idle;

	p2List<j1Label*> labels_list;
};

