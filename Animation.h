#ifndef ANIMATION_H
#define ANIMATION_H

class Animation {
    const byte size;
    byte * chars;
    byte charPointer;

    public:
    Animation(byte, byte *);
    byte getNextFrame();
    static Animation* createDischargingAnimation();
    static Animation* createDoneAnimation();
};

#endif
