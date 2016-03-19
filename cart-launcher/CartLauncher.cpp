#include <cstdlib>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <dirent.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"


const int CARTS_PER_PAGE = 10;

struct State
{
    bool Running=true;
    string CartDirectory;
    vector<string> Carts;
    int Page=0;
    int MaxPage;
    int SelectedIndex=0;
    SDL_Texture* CartImage=NULL;
};

void Init();
void Update();
void Draw();

int SelectedCart();
void SetSelectedIndex(int index);
void SetPage(int page);
void LoadCartImage();

State* state;

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;

int main(int argc, char** argv)
{
    // Open the libraries we need
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    // Open a TODO fullscreen window
    window = SDL_CreateWindow(
            "CartLauncher-8",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800, 480,
            SDL_WINDOW_SHOWN);

    // Make sure the window was created properly
    if (window)
    {
        // Create a renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Make sure the renderer was created properly
        if (renderer)
        {
            // Get a font for text
            font = TTF_OpenFont("font.ttf", 18);

            // Make sure font was opened properly
            if (font)
            {
                Init();

                // Start the functioning loop
                while(state->Running)
                {
                    Update();
                    Draw();
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
void Init()
{
    // Retrieve the cart directory
    state->CartDirectory = getenv("CART_COLLECTOR_DIR");

    // Extract a list of every cart installed
    DIR* dir;
    struct dirent *ent;
    dir = opendir(state->CartDirectory.c_str());

    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            state->Carts.push_back(ent->d_name);
            cout << ent->d_name << endl;
        }

        closedir(dir);
    }
    else
    {
        cout << "Error! Failed to open cart directory: "
            << state->CartDirectory << endl;
    }

    // Calculate how many pages we will have
    int carts = state->Carts.size();
    state->MaxPage = ceil((float) carts / (float) CARTS_PER_PAGE);
}

void Update()
{
    // Process events
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                state->Running = false;
                return;
        }
    }
}

void Draw()
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

int SelectedCart()
{
    return state->Page * CARTS_PER_PAGE + state->SelectedIndex;
}

void SetSelectedIndex(int index)
{
    // Calculate how many carts are displayed on this page
    int cartsOnPage = CARTS_PER_PAGE;

    if (state->Page == state->MaxPage)
    {
        cartsOnPage = state->Carts.size() % CARTS_PER_PAGE;
    }

    // Wrap selection between top and bottom
    if (index < 0) index = cartsOnPage - 1;
    if (index >= cartsOnPage) index = 0;

    // Change the selected index
    state->SelectedIndex = index;

    // Load the proper cart image
    LoadCartImage();
}

void SetPage(int page)
{
    state->Page = page;
    SetSelectedIndex(0);
}

void LoadCartImage()
{
    if (state->CartImage)
    {
        // Free the previously loaded cart image
        SDL_DestroyTexture(state->CartImage);
    }

    // Load the new cart image
    string cartName = state->Carts[SelectedCart()];
    string imagePath = state->CartDirectory + "/" + cartName;
    state->CartImage = SDL_CreateTextureFromSurface(
            renderer, IMG_Load(imagePath.c_str()));
}
