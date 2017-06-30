#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "define.h"
#include "basic_sdl.h"
#include "event.h"
#include <iostream>

int w_window(480), h_window(128);
const std::string baseProgramPath(SDL_GetBasePath());

void afficherGui(SDL_Texture *gui, SDL_Texture *texte, SDL_Texture *volume, SDL_Renderer *render)
{
	// Gui
	texToRenderPart(gui, render, 100, 0, 0, 3, 32, 0, 0, 3, 32);
	for (int i = 3; i < (w_window); i++)
	{
		texToRenderPart(gui, render, 100, i, 0, 1, 32, 3, 0, 1, 32);
	}
	texToRenderPart(gui, render, 100, w_window - 3 - 32, 0, 3, 32, 4, 0, 3, 32);

	// Gui Longueur music
	texToRenderPart(gui, render, 100, 0, 32, 3, 32, 0, 0, 3, 32);
	for (int i = 3; i < (w_window); i++)
	{
		texToRenderPart(gui, render, 100, i, 32, 1, 32, 3, 0, 1, 32);
	}
	texToRenderPart(gui, render, 100, w_window - 3, 32, 3, 32, 4, 0, 3, 32);

	// Nom de la musique
	texToRender(texte, render, 100, 8, 6);

	// Croix Quitter
	texToRenderPart(gui, render, 100, w_window - 32, 0, 32, 32, 0, 32, 32, 32);

	// Play Pause
	texToRenderPart(gui, render, 100, 0, 64, 64, 64, 32, 0, 64, 64);

	// Volume 
	texToRenderPart(gui, render, 100, w_window - 64, 64, 64, 64, 96, 0, 64, 64);

	// Volume Number
	texToRender(volume, render, 100, w_window - 40, 85);

}

int main(int argc, char **argv)
{
	char nom_du_fichier[300] = "No_File_Loaded";
	if (argc > 1)
	{
		strcpy_s(nom_du_fichier, argv[1]);
	}
	char temp[20] = "";
	SDL_Window *window = nullptr;
	SDL_Renderer *render = nullptr;
	Input INPUT; inputInit(&INPUT);
	SDL_Color WHITE = { 255, 255 , 255 };
	int volume = 70;
	initVid(&window, &render, w_window, h_window);
	SDL_SetWindowTitle(window, "ColdJuke");

	TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);

	Mix_Music *music = nullptr;
	music = Mix_LoadMUS(nom_du_fichier);
	Mix_PlayMusic(music, -1);
	Mix_VolumeMusic(static_cast<double>(128 / 100)*volume);
	clearWithColor(render, 80, 40, 100, 255);
	SDL_Texture *gui = imgToTex((baseProgramPath + "gui.png").c_str(), render);
	
	sprintf_s(temp, "%d%%", volume);
	SDL_Texture *tex_volume = txtToTex(temp, render, WHITE, 14);
	SDL_Texture *texte = txtToTex(nom_du_fichier, render, WHITE, 14);
	afficherGui(gui, texte, tex_volume, render);

	SDL_RenderPresent(render);
	int FPS(60), TICKS(1000 / FPS);
	long start(SDL_GetTicks()), actual(SDL_GetTicks());
	INPUT.filedrop = "";
	while (!INPUT.quit && !INPUT.key[SDL_SCANCODE_ESCAPE] && !(INPUT.leftclic && INPUT.leftclic_position_x > w_window-32 && INPUT.leftclic_position_y < 32))
	{
		inputReturn(&INPUT);

		actual = SDL_GetTicks();
		if (actual - start > TICKS)
		{
			if (INPUT.key[SDL_SCANCODE_SPACE] || (INPUT.leftclic && INPUT.leftclic_position_x < 64 && INPUT.leftclic_position_y > 64)) // Pause Play
			{
				if (Mix_PausedMusic())
				{
					Mix_ResumeMusic();
					texToRenderPart(gui, render, 100, 0, 64, 64, 64, 32, 0, 64, 64);
					SDL_RenderPresent(render);

				}
				else
				{
					Mix_PauseMusic();
					texToRenderPart(gui, render, 100, 0, 64, 64, 64, 32, 64, 64, 64);
					SDL_RenderPresent(render);

				}
				SDL_Delay(250);
			}
			if (INPUT.wheelup)
			{
				volume += 2;
				if (volume > 100)
					volume = 100;
				Mix_VolumeMusic(static_cast<double>(128 / 100)*volume);
				// Volume 
				texToRenderPart(gui, render, 100, w_window - 64, 64, 64, 64, 96, 0, 64, 64);
				sprintf_s(temp, "%d%%", volume);
				SDL_DestroyTexture(tex_volume);
				tex_volume = txtToTex(temp, render, WHITE, 14);
				// Volume Number
				texToRender(tex_volume, render, 100, w_window - 40, 85);
				SDL_RenderPresent(render);


			}
			if (INPUT.wheeldown)
			{
				volume -= 2;
				if (volume < 0)
					volume = 0;
				Mix_VolumeMusic(static_cast<double>(128 / 100)*volume);
				// Volume 
				texToRenderPart(gui, render, 100, w_window - 64, 64, 64, 64, 96, 0, 64, 64);
				sprintf_s(temp, "%d%%", volume);
				SDL_DestroyTexture(tex_volume);
				tex_volume = txtToTex(temp, render, WHITE, 14);
				// Volume Number
				texToRender(tex_volume, render, 100, w_window - 40, 85);
				SDL_RenderPresent(render);

			}
			if (INPUT.rightclic)
			{
				int x_mouse_screen; int y_mouse_screen;
				SDL_GetGlobalMouseState(&x_mouse_screen,&y_mouse_screen);
				SDL_SetWindowPosition(window, x_mouse_screen-INPUT.rightclic_position_x, y_mouse_screen-INPUT.rightclic_position_y);
				inputReturn(&INPUT);
			}
			if (INPUT.filedrop != "")
			{
				Mix_PauseMusic();
				music = Mix_LoadMUS(INPUT.filedrop);
				strcpy_s(nom_du_fichier, INPUT.filedrop);
				Mix_PlayMusic(music, -1);
				sprintf_s(temp, "%d%%", volume);
				SDL_DestroyTexture(texte);
				texte = txtToTex(nom_du_fichier, render, WHITE, 14);
				afficherGui(gui, texte, tex_volume, render);
				SDL_RenderPresent(render);

			}
			start = actual;
			INPUT.filedrop = "";
			INPUT.wheeldown = 0;
			INPUT.wheelup = 0;
		}
		else
		{
			SDL_Delay(TICKS - (actual - start));
		}
	}

	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
	return 0;
}
