/*
 * Copyright (C) 2010-2011 Dmitry Marakasov
 *
 * This file is part of glosm.
 *
 * glosm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * glosm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with glosm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GlosmViewer.hh"

#include <glosm/util/gl.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_keysym.h>

#include <cstdio>

class GlosmViewerImpl : public GlosmViewer {
protected:
	bool ignoremouse_;

public:
	GlosmViewerImpl() : ignoremouse_(true) {
	}

	virtual void MouseMove(int x, int y) {
		if (!ignoremouse_)
			GlosmViewer::MouseMove(x, y);
	}

protected:
	virtual void WarpCursor(int x, int y) {
		ignoremouse_ = true;
		SDL_WarpMouse(x, y);
		ignoremouse_ = false;
	}

	virtual void Flip() {
		SDL_GL_SwapBuffers();
	}
};

GlosmViewerImpl app;

void Reshape(int w, int h) {
	SDL_SetVideoMode(w, h, 0, SDL_OPENGL | SDL_RESIZABLE | SDL_HWSURFACE);

	app.Resize(w, h);
}

void KeyDown(SDLKey key) {
	if (key < 0x100)
		app.KeyDown(key);
	else
		switch(key) {
		case SDLK_UP: app.KeyDown(GlosmViewer::UP); break;
		case SDLK_DOWN: app.KeyDown(GlosmViewer::DOWN); break;
		case SDLK_LEFT: app.KeyDown(GlosmViewer::LEFT); break;
		case SDLK_RIGHT: app.KeyDown(GlosmViewer::RIGHT); break;
		case SDLK_KP_PLUS: app.KeyDown('+'); break;
		case SDLK_KP_MINUS: app.KeyDown('-'); break;
		case SDLK_LSHIFT: case SDLK_RSHIFT: app.KeyDown(GlosmViewer::SHIFT); break;
		case SDLK_LCTRL: case SDLK_RCTRL: app.KeyDown(GlosmViewer::CTRL); break;
		default: break;
		}
}

void KeyUp(SDLKey key) {
	if (key < 0x100)
		app.KeyUp(key);
	else
		switch(key) {
		case SDLK_UP: app.KeyUp(GlosmViewer::UP); break;
		case SDLK_DOWN: app.KeyUp(GlosmViewer::DOWN); break;
		case SDLK_LEFT: app.KeyUp(GlosmViewer::LEFT); break;
		case SDLK_RIGHT: app.KeyUp(GlosmViewer::RIGHT); break;
		case SDLK_KP_PLUS: app.KeyUp('+'); break;
		case SDLK_KP_MINUS: app.KeyUp('-'); break;
		case SDLK_LSHIFT: case SDLK_RSHIFT: app.KeyUp(GlosmViewer::SHIFT); break;
		case SDLK_LCTRL: case SDLK_RCTRL: app.KeyUp(GlosmViewer::CTRL); break;
		default: break;
		}
}

int GetEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		int ret = 1;
		switch(event.type) {
		case SDL_QUIT:
			return 0;
		case SDL_KEYDOWN:
			KeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			KeyUp(event.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			app.MouseMove(event.motion.x, event.motion.y);
			break;
		case SDL_VIDEORESIZE:
			Reshape(event.resize.w, event.resize.h);
			break;
		default:
			break;
		}

		if (ret == 0)
			return 0;
	}

	return 1;
}

int real_main(int argc, char** argv) {
	app.Init(argc, argv);

	/* glut init */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw Exception() << "Couldn't initialize SDL: " << (const char*)SDL_GetError();

	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	Reshape(800, 600);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_EnableKeyRepeat(0, 0);

	app.InitGL();

	/* main loop */
	while (GetEvents())
		app.Render();

	return 0;
}

int main(int argc, char** argv) {
	try {
		return real_main(argc, argv);
	} catch (std::exception &e) {
		fprintf(stderr, "Exception: %s\n", e.what());
	} catch (...) {
		fprintf(stderr, "Unknown exception\n");
	}

	return 1;
}