#include "WiFiS3.h"
#include "secrets.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_TSC2007.h>
#include <Adafruit_GFX.h>
#include <Fonts/TomThumb.h>
#include <list>

#include "Shared.hpp"
#include "PixelBroadcastMessage.hpp"
// #include "message.h"

typedef enum
{
    MAIN_MENU,
    CALIBRATION_MENU,
    DRAW_MENU
} Menu;

class statusCircle
{
public:
    statusCircle(unsigned x, unsigned y, unsigned radius) : x(x),
                                                            y(y),
                                                            radius(radius)
    {
    }

    void draw(Adafruit_GFX &screen, const unsigned int colour) const
    {
        screen.fillCircle(x, y, radius, colour);
    }

private:
    unsigned x;
    unsigned y;
    unsigned int radius;
};

class TouchQuery
{
public:
    TouchQuery(Adafruit_TSC2007 &screen, unsigned minPressure)
    {
        screen.read_touch(&x, &y, &z1, &z2);
        pressed = z1 > minPressure;
    }

    uint16_t getX() const
    {
        return x;
    }

    uint16_t getY() const
    {
        return y;
    }

    uint16_t getZ1() const
    {
        return z1;
    }

    uint16_t getZ2() const
    {
        return z2;
    }

    bool getPressed() const
    {
        return pressed;
    }

private:
    uint16_t x;
    uint16_t y;
    uint16_t z1;
    uint16_t z2;
    bool pressed;
};

class Mapper
{
public:
    Mapper(
        unsigned fromMin,
        unsigned fromMax,
        unsigned toMin,
        unsigned toMax) : fromMin(fromMin),
                          fromMax(fromMax),
                          toMin(toMin),
                          toMax(toMax)
    {
    }
    unsigned map(unsigned x) const
    {
        return ::map(x, fromMin, fromMax, toMin, toMax);
    }

private:
    unsigned fromMin;
    unsigned fromMax;
    unsigned toMin;
    unsigned toMax;
};

static Adafruit_TSC2007 ts;
static Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);
static WiFiClient client;
static const Mapper screenXMapper(screenMinX, screenMaxX, 0, tft.width());
static const Mapper screenYMapper(screenMinY, screenMaxY, 0, tft.height());
static const unsigned ts_min_pressure = 200;

Adafruit_GFX_Button calibrationButtons[4];

void setupSerialModule()
{
    Serial.begin(9600);
    while (!Serial)
        ;
}

void setupWifiModule()
{
    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed."
                       "Stopping execution.");
        while (1)
            ;
    }
}

void setupTFT()
{
    tft.begin();
    if (!ts.begin())
    {
        Serial.println("Communication with touchscreen controller failed."
                       "Stopping execution");
        while (1)
            ;
    }
}

void drawMainMenu()
{
    tft.fillScreen(ILI9341_BLACK);
}

void setup()
{
    setupSerialModule();
    setupTFT();
    setupWifiModule();
}

void handlePixelBroadcast(SerialData pixelBroadcastData)
{
    const PixelBroadcastMessage recvPixel = [&]()
    {
        PixelBroadcastMessage _;
        _.init(pixelBroadcastData);
        return _;
    }();

    tft.fillRect(recvPixel.x, recvPixel.y, 3, 3, recvPixel.colour);
}

Menu mainMenuLoop()
{
    const unsigned margin = 10;

    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_PURPLE);
    tft.setCursor(margin, margin);
    tft.setTextSize(3);
    tft.print("LoveBoxR4");
    tft.print(" <3");
    tft.println();
    tft.println();
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(margin, tft.getCursorY());
    tft.print("WiFi: ");

    const statusCircle wifiStatusCircle(
        tft.getCursorX() + 5,
        tft.getCursorY() + 7,
        10);
    wifiStatusCircle.draw(tft, ILI9341_RED);

    tft.println();
    tft.println();
    tft.setCursor(margin, tft.getCursorY());
    tft.print(wifiName);

    tft.println();
    tft.println();
    tft.println();
    tft.setCursor(margin, tft.getCursorY());
    tft.print("Server: ");

    const statusCircle serverStatusCircle(
        tft.getCursorX() + 5,
        tft.getCursorY() + 7,
        10);
    serverStatusCircle.draw(tft, ILI9341_RED);

    tft.println();
    tft.println();
    tft.setCursor(margin, tft.getCursorY());
    tft.print(serverAddress);

    const IPAddress serverIP = [&]()
    {
        IPAddress serverIP;
        serverIP.fromString(serverAddress);
        return serverIP;
    }();

    Menu currentMenu = MAIN_MENU;
    int wifiStatus = -1;
    bool wifiConnected = false;
    bool serverConnected = client.connected();
    while (currentMenu == MAIN_MENU)
    {
        const TouchQuery touchQuery(ts, ts_min_pressure);
        const unsigned touchX = screenXMapper.map((unsigned)touchQuery.getX());
        const unsigned touchY = screenYMapper.map((unsigned)touchQuery.getY());
        const int newWifiStatus = WiFi.status();

        if (wifiStatus != newWifiStatus)
        {
            wifiStatus = newWifiStatus;
            wifiConnected = wifiStatus == WL_CONNECTED;
            wifiStatusCircle.draw(tft, wifiConnected ? ILI9341_GREEN
                                                     : ILI9341_RED);
        }

        if (serverConnected != client.connected())
        {
            serverConnected = client.connected();
            serverStatusCircle.draw(tft, serverConnected ? ILI9341_GREEN
                                                         : ILI9341_RED);
        }

        if (newWifiStatus != WL_CONNECTED)
        {
            WiFi.begin(wifiName, wifiPassword);
        }

        if (client.connected() == false)
        {
            client.connect(serverIP, 5341);
        }
        const bool canStartDrawing = wifiConnected && serverConnected;

        if (canStartDrawing)
        {
            currentMenu = DRAW_MENU;
        }
    }

    return currentMenu;
}

Menu drawMenuLoop()
{
    tft.fillScreen(ILI9341_BLACK);

    size_t nColourButtons = 4;
    Adafruit_GFX_Button colourButtons[nColourButtons];
    unsigned buttonColours[nColourButtons] = {
        ILI9341_RED,
        ILI9341_GREEN,
        ILI9341_BLUE,
        ILI9341_PINK};
    for (int i = 0; i < nColourButtons; i++)
    {
        colourButtons[i].initButton(
            &tft,
            tft.width() / (nColourButtons + 1) * (i + 1),
            20,
            20,
            20,
            buttonColours[i],
            buttonColours[i],
            ILI9341_BLACK,
            "",
            1);
        colourButtons[i].drawButton();
    }

    Menu currentMenu = DRAW_MENU;
    SerialData recvMessageData;
    unsigned currentColour = buttonColours[0];
    while (currentMenu == DRAW_MENU)
    {
        const TouchQuery touchQuery(ts, ts_min_pressure);

        for (int i = 0; i < nColourButtons; i++)
        {
            if (colourButtons[i].justReleased())
            {
                colourButtons[i].drawButton();
                currentColour = buttonColours[i];
            }
        }

        if (touchQuery.getPressed())
        {
            const unsigned touchX = screenXMapper.map((unsigned)touchQuery.getX());
            const unsigned touchY = screenYMapper.map((unsigned)touchQuery.getY());

            bool noButtonsPressed = true;

            for (int i = 0; i < nColourButtons; i++)
            {
                colourButtons[i].press(
                    colourButtons[i].contains(touchX, touchY));

                if (colourButtons[i].isPressed())
                {
                    noButtonsPressed = false;
                }

                if (colourButtons[i].justPressed())
                {
                    colourButtons[i].drawButton(true);
                }
            }

            const bool touchedDrawingArea = noButtonsPressed;

            if (touchedDrawingArea)
            {
                tft.fillRect(touchX, touchY, 3, 3, currentColour);
                const PixelBroadcastMessage sendPixel(touchX, touchY, currentColour);
                const SerialData sendMessageData = sendPixel.serialize();
                for (uint8_t byte : sendMessageData)
                {
                    client.write(byte);
                }
            }
        }

        if (WiFi.status() != WL_CONNECTED || client.connected() == false)
        {
            currentMenu = MAIN_MENU;
        }

        if (client.available() >= 1)
        {
            recvMessageData.push_back((uint8_t)client.read());
        }

        if ( !recvMessageData.empty())
        {
            const uint8_t expectedMessageSize = recvMessageData.front();

            if (recvMessageData.size() == expectedMessageSize)
            {
                const Message::Info recvMessageInfo = Message::deserialize(recvMessageData);
                switch (recvMessageInfo.type)
                {
                case Message::PIXEL_BROADCAST_MSG:
                    handlePixelBroadcast(recvMessageInfo.data);
                    break;
                default:
                    break;
                }
                recvMessageData.clear();
            }
        }
    }
    return currentMenu;
}

void loop()
{
    static Menu currentMenu = MAIN_MENU;

    switch (currentMenu)
    {
    case MAIN_MENU:
        currentMenu = mainMenuLoop();
        break;
    case DRAW_MENU:
        currentMenu = drawMenuLoop();
        break;
    default:
        break;
    }
}