class Wheel {
    public:
        Wheel(int forwardPin, int backwardPin, int speedPin);
        void forward(int speed);
        void backward(int speed);
        void stop();
        void check();

    private:
        int forwardPin;
        int backwardPin;
        int speedPin;
};