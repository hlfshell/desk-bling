#define RED_PIN 1
#define GREEN_PIN 0
#define BLUE_PIN 2

#define NORMAL 0
#define FADING 1
#define PULSING 2
#define PARTY 3

int currentRed = 0;
int currentGreen = 0;
int currentBlue = 0;

int targetRed = 0;
int targetGreen = 0;
int targetBlue = 0;

int secondaryTargetRed = 0;
int secondaryTargetGreen = 0;
int secondaryTargetBlue = 0;

int delayTarget = 0;
int delayCounter = 0;

int mode = NORMAL;

int blinked = 0;

//PARTY MODE COLORS
int partyColors[10][3] = {
    {204, 0, 204},
    {128, 255, 0},
    {102, 102, 255},
    {51, 255, 51},
    {255, 153, 51},
    {153, 51, 255},
    {255, 0, 0},
    {255, 153, 255},
    {0, 128, 255},
    {255, 255, 0}
};

void setup() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    
    Particle.function("setColor", setColor);
    Particle.function("fadeColor", fadeColor);
    Particle.function("pulseColors", pulseColors);
    Particle.function("blinkLights", blinkLights);
    Particle.function("partyMode", partyMode);
    
    //For debugging, let's make the variables watchable
    Particle.variable("red", currentRed);
    Particle.variable("green", currentGreen);
    Particle.variable("blue", currentBlue);
    
    Particle.variable("targetRed", targetRed);
    Particle.variable("targetGreen", targetGreen);
    Particle.variable("targetBlue", targetBlue);
    
    Particle.variable("delayTarget", delayTarget);
    Particle.variable("delayCounter", delayCounter);
    Particle.variable("mode", mode);
    Particle.variable("blinked", blinked);
    
    randomSeed(analogRead(0));
}

void setCurrent(int red, int green, int blue){
    currentRed = red;
    currentGreen = green;
    currentBlue = blue;
}

int setColor(String command){
    int r, g, b;
    if(sscanf(command, "%d,%d,%d", &r, &g, &b) == 3){
        setCurrent(r, g, b);
        outputColor();
        mode = NORMAL;
        return 1;
    } else {
        return -1;
    }
}

int fadeColor(String command){
    int r, g, b, fadeDelay;
    if(sscanf(command, "%d,%d,%d,%d", &r, &g, &b, &fadeDelay) == 4){
        targetRed = r;
        targetGreen = g;
        targetBlue = b;
        delayTarget = fadeDelay;
        delayCounter = delayTarget;
        mode = FADING;
        return 1;
    } else {
        return -1;
    }
}

int pulseColors(String command){
    int r, g, b, r2, g2, b2, fadeDelay;
    if(sscanf(command, "%d,%d,%d,%d,%d,%d,%d", &r, &g, &b, &r2, &g2, &b2, &fadeDelay) == 7){
        targetRed = r;
        targetGreen = g;
        targetBlue = b;
        
        secondaryTargetRed = r2;
        secondaryTargetGreen = g2;
        secondaryTargetBlue = b2;
        
        delayTarget = fadeDelay;
        delayCounter = delayTarget;
        
        mode = PULSING;
    } else {
        return -1;
    }
}

int partyMode(String input){
    mode = PARTY;
    return 1;
}

int blinkLights(String input){
    blinked++;

    int oldR = currentRed;
    int oldG = currentGreen;
    int oldB = currentBlue;
    
    setCurrent(0, 0, 0);
    outputColor();
    delay(100);
    setCurrent(255, 0, 0);
    outputColor();
    delay(100);
    setCurrent(0, 255, 0);
    outputColor();
    delay(100);
    setCurrent(0, 0, 255);
    outputColor();
    delay(100);
    setCurrent(0, 0, 0);
    outputColor();
    delay(50);
    
    setCurrent(oldR, oldG, oldB);
    outputColor();
    
    return 1;
}

void outputColor(){
    analogWrite(RED_PIN, currentRed);
    analogWrite(GREEN_PIN, currentGreen);
    analogWrite(BLUE_PIN, currentBlue);
}

void loop() {
    if(mode != NORMAL) {
        if(delayCounter <= 0){
            if(targetRed != currentRed){
                currentRed += currentRed < targetRed ? 1 : -1;
            }
            if(targetGreen != currentGreen){
                currentGreen += currentGreen < targetGreen ? 1 : -1;
            }
            if(targetBlue != currentBlue){
                currentBlue += currentBlue < targetBlue ? 1 : -1;
            }

            delayCounter = delayTarget;
            
            //If mode is not normal, we need to check if we need to switch our target color
            if(targetRed == currentRed && targetGreen == currentGreen && targetBlue == currentBlue && mode != NORMAL){
                if(mode == FADING) mode = NORMAL;
                else if(mode == PULSING){
                    targetRed = secondaryTargetRed;
                    targetGreen = secondaryTargetGreen;
                    targetBlue = secondaryTargetBlue;
                    
                    secondaryTargetRed = currentRed;
                    secondaryTargetGreen = currentGreen;
                    secondaryTargetBlue = currentBlue;

                } else if(mode == PARTY){
                    int partyPosition = random(0, sizeof(partyColors));
                    setCurrent(partyColors[partyPosition][0], partyColors[partyPosition][1], partyColors[partyPosition][2]);
                }
            }
        }
        
        //Count down on the delay
        delayCounter--;
    }
    outputColor();
    delay(1);
    
}