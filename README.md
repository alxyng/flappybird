# Flappy Bird

A clone of [.GEARS' Flappy Bird](https://dotgears.com/games/flappy-birds-family) in just over 1000 lines of C.

![Preview](preview.gif "Preview")

## Checkout

### A fresh one

```
git clone --recurse-submodules https://github.com/alxyng/flappybird.git
```

### Existing one

```
git clone https://github.com/alxyng/flappybird.git
git submodule update --init --recursive
```

## Build and run

### macOS

The following has been tested on **macOS** only but may work on other platforms.

```
cmake --preset Release
cmake --build --preset Release
./build/Release/flappybird
```

### iOS

The following has been tested on XCode 16.0.

From the root of the repo, apply the following patch to SDL_mixer:

```
(cd deps/SDL_mixer && git apply ../../SDL_mixer.patch)
```

Then open XCode, select a device and hit Run. You may have to set up signing if you haven't done so already.