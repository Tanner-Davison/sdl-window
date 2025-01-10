#include "Player.hpp"
#include "SDL_render.h"
#include "createwindow.hpp"
#include <SDL.h>

Player::Player() : Player(100.0f, 100.0f, 50, 50) {}
Player::Player(float x, float y) : Player(x, y, 50, 50) {};
Player::Player(float x, float y, int width, int height)
    : rectXf(x), rectYf(y), rectX(static_cast<int>(x)),
      rectY(static_cast<int>(y)), rectWidth(width), rectHeight(height) {
  initCommonValues();
};
// initializer list to ease this file lol
void Player::initCommonValues() {
  isMovingUp = isMovingDown = isMovingLeft = isMovingRight = false;
  shooting = boost = false;
  velocityX = velocityY = 0.0f;
  mTexture = nullptr;
  textureWidth = textureHeight = 0;
  ACCELERATION = BASE_ACCELERATION;
  playerRect = {rectX, rectY, rectWidth, rectHeight};
}
void Player::renderPlayer(SDL_Renderer *renderer) {
  SDL_Rect rect = {rectX, rectY, rectWidth, rectHeight};
  if (mTexture != nullptr) {
    SDL_RenderCopy(renderer, mTexture, nullptr, &rect);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
  }
  // Render weapon // updates position
  weapon.update((rectX - 2) + rectWidth / 2, rectY - 6);
  weapon.render(renderer);
}

void Player::setPlayerPos(int x, int y) {
  this->rectX = x;
  this->rectY = y;
  this->rectXf = static_cast<float>(x);
  this->rectYf = static_cast<float>(y);
}

void Player::updatePlayerPos() {

  rectX = static_cast<int>(rectXf);
  rectY = static_cast<int>(rectYf);
}

void Player::handleBounds(float nextX, float nextY) {
  if (nextX <= 0) {
    rectXf = 0.0f;
    velocityX = -velocityX * 1.8f;
  } else if (nextX >= SCREEN_WIDTH - rectWidth) {
    rectXf = SCREEN_WIDTH - rectWidth;
    velocityX = -velocityX * 1.8f;
  } else {
    rectXf = nextX;
  }

  if (nextY <= 0) {
    rectYf = 0.0f;
    velocityY = -velocityY * 1.8f;
  } else if (nextY >= SCREEN_HEIGHT - rectHeight) {
    rectYf = SCREEN_HEIGHT - rectHeight;
    velocityY = -velocityY * 1.8f;
  } else {
    rectYf = nextY;
  }

  updatePlayerPos();
}
bool Player::loadTexture(const char *path, SDL_Renderer *renderer) {
  // Free Existing Texture * ERROR HANDLING *
  if (mTexture != nullptr) {
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
  }
  // Load Image
  SDL_Surface *loadedSurface = IMG_Load(path);
  if (loadedSurface == nullptr) {
    printf("Unable to laod image %s! SDL_image Error: %s\n", path,
           IMG_GetError());
    return false;
  }
  mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
  if (mTexture == nullptr) {
    printf("Unable to create Texture %s! SDL Error: %s\n", path,
           SDL_GetError());
    return false;
  }
  // Main implementation
  textureWidth = loadedSurface->w;
  textureHeight = loadedSurface->h;
  // Free the survface!!
  SDL_FreeSurface(loadedSurface);
  return true;
}

void Player::handleInput(bool up, bool down, bool left, bool right,
                         bool isShooting, bool boost) {
  ACCELERATION = boost ? BOOST_ACCELERATION : BASE_ACCELERATION;
  float CurrentMaxVelocity = boost ? MAX_VELOCITY * 1.5f : MAX_VELOCITY;
  // Movement handling
  if (right) {
    velocityX = std::min(velocityX + ACCELERATION, CurrentMaxVelocity);
  } else if (left) {
    velocityX = std::max(velocityX - ACCELERATION, -CurrentMaxVelocity);
  } else {
    velocityX *= DECELERATION;
    if (abs(velocityX) < 0.3f) {
      velocityX = 0;
    }
  }
  if (down) {
    velocityY = std::min(velocityY + ACCELERATION, CurrentMaxVelocity);
  } else if (up) {
    velocityY = std::max(velocityY - ACCELERATION, -CurrentMaxVelocity);
  } else {
    velocityY *= DECELERATION;
    if (abs(velocityY) < 0.3f) {
      velocityY = 0;
    }
  }
  // hand weapons shooting
  if (isShooting) {
    weapon.shoot();
  }

  // next position
  float nextX = rectXf + velocityX;
  float nextY = rectYf + velocityY;
  handleBounds(nextX, nextY);
}

// PlayerInput
void Player::handlePlayerInput(const Uint8 *keyState) {
  handleInput(keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP],
              keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN],
              keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT],
              keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT],
              keyState[SDL_SCANCODE_SPACE], keyState[SDL_SCANCODE_LSHIFT]);
}

std::pair<int, int> Player::getPosition() const {
  return std::make_pair(rectX, rectY);
}
bool Player::checkCollision(const SDL_Rect &a, const SDL_Rect &b) {
  // edges recA
  int leftA = a.x;
  int rightA = a.x + a.w;
  int topA = a.y;
  int bottomA = a.y + a.h;

  // edges recB
  int leftB = b.x;
  int rightB = b.x + b.w;
  int topB = b.y;
  int bottomB = b.y + b.h;

  // Check if any edges don't overlap
  if (bottomA <= topB)
    return false;
  if (topA >= bottomB)
    return false;
  if (rightA <= leftB)
    return false;
  if (leftA >= rightB)
    return false;

  // If none of the above, they overlap
  return true;
}
const Weapon &Player::getWeapon() const { return weapon; }
Weapon &Player::getWeapon() { return weapon; }
void Player::cleanup() {
  if (mTexture != nullptr) {
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
  }
}

Player::~Player() { cleanup(); };
