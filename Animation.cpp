#include "Arduino.h"
#include "Animation.h"

Animation::Animation(byte size, byte * chars) : size(size), chars(chars) {}

byte Animation::getFrame() {
    return chars[charPointer];
}

void Animation::advanceToNextFrame() {
    charPointer = (charPointer + 1) % size;
}

Animation* Animation::createDischargingAnimation() {
    byte * characters = new byte[4]{0, 1, 2, 2};
    Animation * animation = new Animation(4, characters);
    return animation;
}

Animation* Animation::createDoneAnimation() {
    byte * characters = new byte[2]{2, ' '};
    Animation * animation = new Animation(2, characters);
    return animation;
}
