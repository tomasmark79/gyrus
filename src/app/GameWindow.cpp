#include "GameWindow.hpp"
#include <Game/GameSession.hpp>

#include <Game/FixedStep.hpp>
#include <SDL.h>
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

namespace {
using namespace gyrus::game;

void check(bool condition, const char* operation) {
  if (!condition) throw std::runtime_error(std::string(operation) + ": " + SDL_GetError());
}

struct Window {
  GameSession game;
  SDL_Window* window{};
  SDL_Renderer* renderer{};
  SDL_Texture* texture{};
  Framebuffer frame{};
  Framebuffer playerLayer{};
  std::array<std::uint32_t, sceneWidth * sceneHeight> pixels{};
  SDL_AudioDeviceID audioDevice{};
  FixedStep clock;
  bool playing{}, paused{}, quit{};
  unsigned rendered{}, frameLimit{};
  std::uint64_t previous{};
  std::string capture;

  ~Window() {
    if (audioDevice) SDL_CloseAudioDevice(audioDevice);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void begin(unsigned players = 1) {
    playing = true;
    paused = false;
    game.begin(players);
    clock = FixedStep{};
    // Open on the start gesture, allowing browser audio activation.
    if (!audioDevice) {
      SDL_AudioSpec wanted{};
      wanted.freq = 48000;
      wanted.format = AUDIO_F32SYS;
      wanted.channels = 1;
      wanted.samples = 1024;
      audioDevice = SDL_OpenAudioDevice(nullptr, 0, &wanted, nullptr, 0);
      if (!audioDevice) SDL_Log("Audio unavailable: %s", SDL_GetError());
    }
    if (audioDevice) {
      SDL_ClearQueuedAudio(audioDevice);
      SDL_PauseAudioDevice(audioDevice, 0);
    }
  }

  void initializeRenderer() {
    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    check(renderer != nullptr, "create renderer");
    SDL_RenderSetLogicalSize(renderer, 320, sceneHeight);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, sceneWidth, sceneHeight);
    check(texture != nullptr, "create texture");
  }

  void update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) quit = true;
      if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          if (playing) {
            playing = false; paused = false;
            if (audioDevice) SDL_ClearQueuedAudio(audioDevice);
          }
#if !defined(__EMSCRIPTEN__)
          else quit = true;
#endif
          break;
        case SDLK_RETURN:
        case SDLK_SPACE:
          if (!playing) begin();
          break;
        case SDLK_2:
          if (!playing) begin(2);
          break;
        case SDLK_p:
          if (playing) {
            paused = !paused;
            if (audioDevice) SDL_ClearQueuedAudio(audioDevice);
          }
          break;
        default: break;
        }
      }
    }
    const auto now = SDL_GetPerformanceCounter();
    const auto elapsed = static_cast<double>(now - previous) / SDL_GetPerformanceFrequency();
    previous = now;
    const auto* keys = SDL_GetKeyboardState(nullptr);
    std::uint8_t joystick = 15;
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) joystick &= ~1u;
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) joystick &= ~2u;
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) joystick &= ~4u;
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) joystick &= ~8u;
    if (playing && !paused) {
      clock.advance(elapsed, [&] {
        game.step(joystick, keys[SDL_SCANCODE_SPACE] != 0);
        if (!game.playing) playing = false;
        std::array<float, 960> samples{};
        game.audio.render(samples);
        if (audioDevice) {
          // Bound latency after a stalled browser tab or audio device.
          if (SDL_GetQueuedAudioSize(audioDevice) > sizeof(samples) * 5)
            SDL_ClearQueuedAudio(audioDevice);
          check(SDL_QueueAudio(audioDevice, samples.data(), sizeof(samples)) == 0,
                "queue audio");
        }
      });
    }
    frame.fill(0);
    playerLayer.fill(0);
    SceneOptions scene = game.scene;
    scene.title = !playing;
    if (!playing) scene.message = SceneMessage::None;
    PlayerColors playerColors{};
    if (!playing) {
      drawTitle(frame);
      drawPlanet(frame, 0, true);
    }
    else {
      playerColors = game.draw(frame, playerLayer);
    }
    const auto composed = composeScene(frame, game.hud, scene, playerColors, &playerLayer);
    for (std::size_t i = 0; i < composed.size(); ++i) {
      const auto color = classicAtariRgb(composed[i]);
      pixels[i] = 0xff000000u | (std::uint32_t(color.r) << 16)
                  | (std::uint32_t(color.g) << 8) | color.b;
    }
    check(SDL_UpdateTexture(texture, nullptr, pixels.data(), 160 * sizeof(std::uint32_t)) == 0,
          "update texture");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    ++rendered;
    if (frameLimit && rendered >= frameLimit) quit = true;
    if (quit && !capture.empty()) {
      auto* surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels.data(), sceneWidth, sceneHeight, 32,
                         160 * sizeof(std::uint32_t), SDL_PIXELFORMAT_ARGB8888);
      check(surface != nullptr, "create capture");
      const int status = SDL_SaveBMP(surface, capture.c_str());
      SDL_FreeSurface(surface);
      check(status == 0, "save capture");
    }
  }
};

#if defined(__EMSCRIPTEN__)
void browserFrame(void* data) {
  auto* window = static_cast<Window*>(data);
  // SDL's EGL swap-interval setup changes Emscripten loop timing even for an
  // interval of zero. Create the renderer only after the loop exists.
  if (!window->renderer) {
    window->initializeRenderer();
    emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
  }
  window->update();
  if (window->quit) {
    emscripten_cancel_main_loop();
    delete window;
  }
}
#endif
} // namespace

int runGameWindow(unsigned frameLimit, const std::string& capture, bool preview) {
  auto state = std::make_unique<Window>();
  check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) == 0, "initialize SDL");
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  state->window = SDL_CreateWindow("Gyrus — reconstruction preview",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 600,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  check(state->window != nullptr, "create window");
  state->frameLimit = frameLimit;
  state->capture = capture;
  state->previous = SDL_GetPerformanceCounter();
  if (preview) state->begin();
#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(browserFrame, state.release(), 0, false);
#else
  state->initializeRenderer();
  while (!state->quit) {
    state->update();
    SDL_Delay(1);
  }
#endif
  return 0;
}
