#include <cstdlib>
#include <math.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

#include <dirent.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"


const int CARTS_PER_PAGE = 15;
const string EXTENSION(".p8.png");

struct State
{
    bool Running=true;
    string CartDirectory;
    vector<string> Carts;
    int Page=1;
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
void LaunchCart();

State* state;

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;

SDL_Joystick* joystick;

int main(int argc, char** argv)
{
    // Open the libraries we need
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    // Prepare for joystick events
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);
    if (!joystick)
    {
        cout << "Couldn't open joystick" << endl;
    }

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
                // Initialize the launcher state
                state = new State();
                Init();

                // Start the functioning loop
                while(state->Running)
                {
                    Update();
                    Draw();
                    SDL_Delay(16);
                }

                // Clean up the launcher state
                delete state;
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

void Init()
{
    // Retrieve the cart directory
    const char* value = getenv("CART_COLLECTOR_DIR");

    // Throw an error if it hasn't been defined
    if (!value)
    {
        cout << "Error! Failed to find cart directory. Did you remember"
            << " to load the runtime configuration?" << endl;
        return;
    }

    state->CartDirectory = value;

    // Extract a list of every cart installed
    DIR* dir;
    struct dirent *ent;
    dir = opendir(value);

    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            string filename(ent->d_name);
            if (filename.find(EXTENSION) != string::npos)
            {
                state->Carts.push_back(ent->d_name);
                cout << ent->d_name << endl;
            }
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

    LoadCartImage();
}

void Update()
{
    // Process events
    SDL_Event event;

    SDL_Keycode key;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                state->Running = false;
                return;

            case SDL_KEYDOWN:
                key = event.key.keysym.sym;
                switch (key)
                {
                    case SDLK_LEFT:
                        SetPage(state->Page-1);
                        break;
                    case SDLK_RIGHT:
                        SetPage(state->Page + 1);
                        break;
                    case SDLK_UP:
                        SetSelectedIndex(state->SelectedIndex-1);
                        break;
                    case SDLK_DOWN:
                        SetSelectedIndex(state->SelectedIndex+1);
                        break;
                    case SDLK_RETURN:
                        LaunchCart();
                        break;
                }
                break;

            case SDL_JOYAXISMOTION:
                if (event.jaxis.value < -20000)
                {
                    if (event.jaxis.axis == 0)
                    {
                        SetPage(state->Page-1);
                    }
                    if (event.jaxis.axis == 1)
                    {
                        SetSelectedIndex(state->SelectedIndex-1);
                    }
                }
                if (event.jaxis.value > 20000)
                {
                    if (event.jaxis.axis == 0)
                    {
                        SetPage(state->Page+1);
                    }
                    if (event.jaxis.axis == 1)
                    {
                        SetSelectedIndex(state->SelectedIndex+1);
                    }
                }
                break;
        }
    }
}

void Draw()
{
    // Clear the screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Draw the cover of the currently selected cartridge
    SDL_Rect cartDisplayRect;
    cartDisplayRect.w = 160*2;
    cartDisplayRect.h = 205*2;
    cartDisplayRect.x = 50;
    cartDisplayRect.y = 480/2-cartDisplayRect.h/2;
    SDL_RenderCopy(renderer, state->CartImage, NULL, &cartDisplayRect);

    // Define colors for UI text
    SDL_Color selectedColor;
    selectedColor.r = 255;
    selectedColor.g = 255;
    selectedColor.b = 0;
    selectedColor.a = 255;

    SDL_Color normalColor;
    normalColor.r = 255;
    normalColor.g = 255;
    normalColor.b = 255;
    normalColor.a = 255;

    // Draw the current page number
    int x = 460;
    int y = cartDisplayRect.y;

    stringstream pageInfo;
    pageInfo << state->Page << "/" << state->MaxPage;
    const char* pageInfoStr = pageInfo.str().c_str();

    SDL_Texture* pageInfoTexture = SDL_CreateTextureFromSurface(
            renderer,
            TTF_RenderText_Solid(font, pageInfoStr, normalColor));

    SDL_Rect pageInfoRect;
    pageInfoRect.x = x;
    pageInfoRect.y = y;
    TTF_SizeText(font, pageInfoStr, &pageInfoRect.w, &pageInfoRect.h);
    SDL_RenderCopy(renderer, pageInfoTexture, NULL, &pageInfoRect);

    SDL_DestroyTexture(pageInfoTexture);

    // Draw the list of cartridge titles
    int firstCartOnPage = (state->Page-1) * CARTS_PER_PAGE;

    for (int i = 0; i < CARTS_PER_PAGE; ++i)
    {
        // Strip the .p8.png extension from the cart filname
        string cartTitle = state->Carts[i+firstCartOnPage];
        cartTitle = cartTitle.substr(0, cartTitle.size() - EXTENSION.size());

        // Display in yellow if the cart is selected
        SDL_Color color = normalColor;
        if (state->SelectedIndex == i)
        {
            color = selectedColor;
        }

        // Draw the cart title
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(
                renderer,
                TTF_RenderText_Solid(
                    font,
                    cartTitle.c_str(),
                    color));

        y = cartDisplayRect.y + 24 * (i+2);

        SDL_Rect destRect;
        destRect.x = x;
        destRect.y = y;
        TTF_SizeText(font, cartTitle.c_str(), &destRect.w, &destRect.h);

        SDL_RenderCopy(renderer, titleTexture, NULL, &destRect);
        SDL_DestroyTexture(titleTexture);
    }

    // Show the scene
    SDL_RenderPresent(renderer);
}

int SelectedCart()
{
    return (state->Page-1) * CARTS_PER_PAGE + state->SelectedIndex;
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
    // Wrap page between 1 and max
    if (page <= 0) page = state->MaxPage;
    else if (page > state->MaxPage) page = 1;
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

void LaunchCart()
{
    // Get the full path of the cart
    string cartName = state->Carts[SelectedCart()];
    string cartPath = state->CartDirectory + "/" + cartName;

    // Launch the cart through PICO-8
    string launchCommand = "pico8 -run \"" + cartPath + "\"";
    cout << launchCommand << endl;
    FILE* in = popen(launchCommand.c_str(), "r");
    pclose(in);
}
