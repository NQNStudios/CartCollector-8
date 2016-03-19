#include <iostream>
using namespace std;

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"


struct State
{
    bool Running = true;
};

void Init(State* state);
void Update(State* state);
void Draw(SDL_Renderer* renderer, TTF_Font* font);

int main(int argc, char** argv)
{
    // Open the libraries we need
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    // Open a TODO fullscreen window
    SDL_Window* window = SDL_CreateWindow(
            "CartLauncher-8",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800, 480,
            SDL_WINDOW_SHOWN);

    // Make sure the window was created properly
    if (window)
    {
        // Create a renderer
        SDL_Renderer* renderer = SDL_CreateRenderer(
                window, -1, SDL_RENDERER_ACCELERATED);

        // Make sure the renderer was created properly
        if (renderer)
        {
            // Get a font for text
            TTF_Font* font = TTF_OpenFont("font.ttf", 18);

            // Make sure font was opened properly
            if (font)
            {
                // Initialize the launcher state
                State state;
                Init(&state);

                // Start the functioning loop
                while(state.Running)
                {
                    Update(&state);
                    Draw(renderer, font);
                    SDL_Delay(16);
                }
            }
            else
            {
                cout << "Error! Failed to open font: "
                    << TTF_GetError() << endl;
            }

            // Destroy the renderer
            SDL_DestroyRenderer(renderer);
        }
        else
        {
            cout << "Error! Failed to inititalize renderer: "
                << SDL_GetError() << endl;
        }

        // Close the window
        SDL_DestroyWindow(window);
    }
    else
    {
        cout << "Error! Failed to create window: " << SDL_GetError() << endl;
    }

    // Close the libraries
    TTF_Quit();
    SDL_Quit();
}

SDL_Texture* testCart;
void Init(State* state)
{
}

void Update(State* state)
{
}

void Draw(SDL_Renderer* renderer, TTF_Font* font)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, NULL);
    if (!testCart)
    {
    testCart = SDL_CreateTextureFromSurface(
            renderer,
            IMG_Load(
                "/Users/natman/Library/Application Support/PICO-8/carts/The Tower of Archeos.p8.png"));
    }
    SDL_Rect cartDisplayRect;
    cartDisplayRect.x = 0;
    cartDisplayRect.y = 0;
    cartDisplayRect.w = 160;
    cartDisplayRect.h = 205;
    SDL_RenderCopy(renderer, testCart, NULL, &cartDisplayRect);
    SDL_RenderPresent(renderer);
}
