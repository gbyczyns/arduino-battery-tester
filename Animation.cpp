#include "Arduino.h"
#include "Animation.h"

Animation::Animation(byte size, byte * chars) : size(size), chars(chars) {}

byte Animation::getNextFrame() {
    charPointer = (charPointer + 1) % size;
    return chars[charPointer];
}

Animation* Animation::createDischargingAnimation() {
    byte * characters = new byte[4]{0, 1, 2, 3};
    Animation * animation = new Animation(4, characters);
    return animation;
}

Animation* Animation::createDoneAnimation() {
    byte * characters = new byte[2]{3, ' '};
    Animation * animation = new Animation(2, characters);
    return animation;
}
