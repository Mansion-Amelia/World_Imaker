/**
 * \file SDLWindowManager.cpp
 * \brief Gestion de la fenêtre
 * \author MANSION Amélia & SGRO' Manon
 * \version 0.1
 * \date 20 décembre 2019
 *
 * Gestion de la fenêtre SDL2
 *
 */

#include "glimac/SDLWindowManager.hpp"
#include <iostream>
#include <GL/gl.h>

namespace glimac {

SDLWindowManager::SDLWindowManager(uint32_t width, uint32_t height, const char* title) {
    if(0 != SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << SDL_GetError() << std::endl;
        return;
    }

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

    SDL_GLContext openglContext = SDL_GL_CreateContext (window);

    std::cout <<  glGetString (GL_VERSION) << std::endl;

    if(!window) {
        std::cerr << SDL_GetError() << std::endl;
        return;
    }
}

SDLWindowManager::~SDLWindowManager() {
    SDL_Quit();
}

bool SDLWindowManager::pollEvent(SDL_Event& e) {
    return SDL_PollEvent(&e);
}

bool SDLWindowManager::isKeyPressed(SDL_Keycode key) const {
    return SDL_GetKeyboardState(nullptr)[key];
}

// button can SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT and SDL_BUTTON_MIDDLE
bool SDLWindowManager::isMouseButtonPressed(uint32_t button) const {
    return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(button);
}

glm::ivec2 SDLWindowManager::getMousePosition() const {
    glm::ivec2 mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    return mousePos;
}

void SDLWindowManager::swapBuffers() {
    SDL_GL_SwapWindow(window);
}

float SDLWindowManager::getTime() const {
    return 0.001f * SDL_GetTicks();
}

void SDLWindowManager::printSignature(){
    std::cout << " " << std::endl;
    std::cout << " " << std::endl;
    std::cout << "        \033[1;31m     ___        \033[0m" << std::endl;
    std::cout << "        \033[1;31m   '\033[0m      \033[1;32m \\ \033[0m  " << std::endl;
    std::cout << "        \033[1;31m  '\033[0m        \033[1;32m \\ \033[0m  " << std::endl;
    std::cout << "                    \033[1;32m \\ \033[0m" << std::endl;
    std::cout << "       \033[1;32m \\ \033[0m   \033[1;34m  \\ \033[0m   \033[1;32m / \033[0m" << std::endl;
    std::cout << "        \033[1;32m \\ \033[0m   \033[1;34m  \\ \033[0m    " << std::endl;
    std::cout << "        \033[1;32m  \\_\033[0m \033[1;34m  __\\/ \033[0m   "<<std::endl;
    std::cout << " " << std::endl;
    std::cout << "          \033[1;34mWORLD IMAKER\033[0m\n";
    std::cout << " " << std::endl;
    std::cout << " " << std::endl;
    std::cout << "    || Hic sunt commentes ||       " << std::endl;
    std::cout << "    \\/                    \\/       " << std::endl;
    std::cout << " " << std::endl;
};

}

