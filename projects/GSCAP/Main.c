#include <GSCA/GSCA.h>
#include <SDL2/SDL.h>

static gscaAPU*             apu = nullptr;
static gscaAudioStore*      store = nullptr;
static gscaAudioEngine*     engine = nullptr;
static SDL_Window*          window = nullptr;
static SDL_Renderer*        renderer = nullptr;
static SDL_AudioDeviceID    audioDevice = 0;
static float                audioBuffer[8192] = { 0 };
static uint16_t             audioCursor = 0;
static bool                 running = true;
static uint32_t             ticks = 0;
static uint16_t             handle = 0;

static void gscapAtExit ()
{
    gscaDestroyAudioEngine(engine);
    gscaDestroyAudioStore(store);
    gscaDestroyAPU(apu);

    if (renderer != nullptr)    { SDL_DestroyRenderer(renderer); }
    if (window != nullptr)      { SDL_DestroyWindow(window); }
    if (audioDevice != 0)       { SDL_CloseAudioDevice(audioDevice); }
    SDL_Quit();
}

int main (int argc, char** argv)
{
    atexit(gscapAtExit);

    if (argc < 2)
    {
        printf("Usage: %s <file> [song]\n", argv[0]);
        return 0;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr, "Could not initialize SDL: '%s'.\n", SDL_GetError());
        return 1;
    }

    char title[512] = "GSCAP Player";
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        fprintf(stderr, "Could not create SDL window: '%s'.\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == nullptr)
    {
        fprintf(stderr, "Could not create SDL renderer: '%s'.\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec desired, obtained;
    desired.freq = GSCA_DEFAULT_SAMPLE_RATE;
    desired.channels = 2;
    desired.format = AUDIO_F32;
    desired.samples = 8192 / 2;
    desired.callback = nullptr;
    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (audioDevice == 0)
    {
        fprintf(stderr, "Could not open SDL audio device: '%s'.\n", SDL_GetError());
        return 1;
    }
    SDL_PauseAudioDevice(audioDevice, 0);

    apu = gscaCreateAPU();
    store = gscaCreateAudioStore(GSCA_AS_DEFAULT_CAPACITY);
    engine = gscaCreateAudioEngine(apu, store);

    
    if (gscaReadAudioFile(store, argv[1]) == false)
    {
        return 1;
    }

    const gscaAudioHandle* playHandle = nullptr;
    if (argc >= 3)
    {
        playHandle = gscaGetHandleByName(store, argv[2]);
        if (playHandle != nullptr)
        {
            handle = playHandle->id;
        }
    }
    else
    {
        handle = 1;
        playHandle = gscaGetHandleByIndex(store, 1);
    }

    if (gscaPlayMusic(engine, playHandle->name) == false)
    {
        return 1;
    }

    snprintf(title, 512, "GSCAP Player - \"%s\" (%s)", playHandle->name, argv[1]);
    SDL_SetWindowTitle(window, title);

    while (running == true)
    {
        ticks++;

        if (gscaTickAPU(apu) == true)
        {
            if (audioCursor + 2 <= 8192)
            {
                const gscaAudioSample* sample = gscaGetCurrentSample(apu);
                audioBuffer[audioCursor++] = sample->left;
                audioBuffer[audioCursor++] = sample->right;
            }
        }

        if (ticks % GSCA_UPDATE_INTERVAL == 0)
        {
            SDL_Event ev;
            while (SDL_PollEvent(&ev))
            {
                if (ev.type == SDL_QUIT)
                {
                    running = false;
                    break;
                }
                else if (ev.type == SDL_KEYDOWN)
                {
                    if (ev.key.keysym.sym == SDLK_LEFT)
                    {
                        if (handle > 0) { handle--; }
                        playHandle = gscaGetHandleByID(store, handle);
                        if (playHandle != nullptr)
                        {
                            // gscaFadeToMusic(engine, playHandle->name, 2);
                            gscaPlayMusic(engine, playHandle->name);
                            snprintf(title, 512, "GSCAP Player - \"%s\" (%s)", playHandle->name, argv[1]);
                            SDL_SetWindowTitle(window, title);
                        }
                    }
                    else if (ev.key.keysym.sym == SDLK_RIGHT)
                    {
                        if (handle < gscaGetAudioCount(store)) { handle++; }
                        playHandle = gscaGetHandleByID(store, handle);
                        if (playHandle != nullptr)
                        {
                            // gscaFadeToMusic(engine, playHandle->name, 2);
                            gscaPlayMusic(engine, playHandle->name);
                            snprintf(title, 512, "GSCAP Player - \"%s\" (%s)", playHandle->name, argv[1]);
                            SDL_SetWindowTitle(window, title);
                        }
                    }
                    else if (ev.key.keysym.sym == SDLK_ESCAPE)
                    {
                        running = false;
                        break;
                    }
                }
            }

            {
                uint32_t queued = SDL_GetQueuedAudioSize(audioDevice);
                if (queued < 0xFFFF)
                {
                    SDL_QueueAudio(audioDevice, audioBuffer, audioCursor * sizeof(float));
                    audioCursor = 0;
                }

                gscaUpdateAudioEngine(engine);
                SDL_Delay(15);
            }
    
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
    }

    return 0;
}
