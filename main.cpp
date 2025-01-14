#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace sf;

float randF(float min = 30.0f, float max = 60.0f)
{
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

int strToInt(const string& str)
{
    int num = 0;
    stringstream ss(str);
    ss >> num;
    return num;
}

string intToStr(int num) 
{
    stringstream ss;
    ss << num;
    return ss.str();
}

int main()
{
    RenderWindow window(VideoMode::getDesktopMode(), "PROJECT NINJA RUSH", Style::Fullscreen);
    window.setFramerateLimit(60);

    enum screen { intro, playing, end };
    screen currentScreen = intro;

    fstream file;
    Font font;
    font.loadFromFile("bold.ttf");
    
    file.open("score.txt", ios::in);
    string lastHighScore = "0";
    string currentScoreStr = "0";
    string currentUser;
    int highScore = INT_MIN;

    while (file >> currentUser >> currentScoreStr)
    {
        if (strToInt(currentScoreStr) > highScore)
            highScore = strToInt(currentScoreStr);
    }
    file.close();

    Text logo("NINJA RUSH", font, 250.0f);
    logo.setPosition(window.getSize().x / 7, window.getSize().y / 7);

    Text scoreText("", font, 30.0f);
    scoreText.setPosition(window.getSize().x / 6.5, window.getSize().y / 2.5);

    Text introText("Please Enter Your Name:", font, 30.0f);
    introText.setPosition(window.getSize().x / 3, window.getSize().y / 2);

    Text inputText("", font, 40.0f);
    inputText.setPosition(window.getSize().x / 3, window.getSize().y / 1.6);

    string userName = "";
    bool isKeyPressed = false;

    Text endText("", font, 150.0f);
    endText.setPosition(window.getSize().x / 10, window.getSize().y / 7);

    RectangleShape surface(Vector2f(window.getSize().x, 30.0f));
    surface.setFillColor(Color::Green);
    surface.setPosition(0, window.getSize().y - surface.getSize().y);

    Texture ninjaTexture;
    if (!ninjaTexture.loadFromFile("guy.png"))
    {
        cerr << "Error loading ninja texture!" << endl;
        return -1;
    }

    Sprite ninja(ninjaTexture);
    ninja.setScale(0.25f, 0.25f);
    ninja.setPosition(250, (window.getSize().y - surface.getSize().y) - ninja.getGlobalBounds().height + 13.0f);

    RectangleShape obstacle(Vector2f(50.0f, 50.0f));
    obstacle.setFillColor(Color::Blue);
    obstacle.setPosition(window.getSize().x - obstacle.getSize().x, window.getSize().y - surface.getSize().y - obstacle.getSize().y);

    bool isJumping = false, isFalling = false;
    float jumpY = window.getSize().y - (window.getSize().y / 3.0), currentY;
    float jumpSpeed = 12.0f, obstacleSpeed = 12.0f;
    float groundY = (window.getSize().y - surface.getSize().y) - ninja.getGlobalBounds().height + 13.0f;
    int currentScore = 0;
    string scoreOutput;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space)
            {
                if (currentScreen == end)
			            {
                    file.open("score.txt", ios::out | ios::app);
                    file << "\t" << currentScore << endl;
                    file.close();
                    window.close();
                }
                else if (currentScreen == intro)
                {
                    window.close();
                }
                else
                    currentScreen = end;
            }
        }

        if (currentScreen == intro)
        {
            if (event.type == sf::Event::TextEntered) 
            {
                if (event.text.unicode < 128) 
                {
                    if (event.text.unicode == 8 && userName.length() > 0 && !isKeyPressed) 
                    {
                        userName.erase(userName.length() - 1, 1);
                        isKeyPressed = true;
                    } 
                    else if (!isKeyPressed && event.text.unicode != 13 && event.text.unicode != 32) 
                    {
                        userName += static_cast<char>(event.text.unicode);
                        isKeyPressed = true;
                    } 
                    else if (!isKeyPressed && event.text.unicode == 32) 
                    {
                        userName += '_';
                        isKeyPressed = true;
                    }
                    inputText.setString(userName);
                }
            }
            if (event.type == sf::Event::KeyReleased) 
            {
                isKeyPressed = false;
            }
            if (event.text.unicode == 13)
            {
                file.open("score.txt", ios::in);
                while (file >> currentUser >> currentScoreStr)
                {
                    if (userName == currentUser && strToInt(currentScoreStr) > strToInt(lastHighScore))
                    {
                        lastHighScore = currentScoreStr;
                    }
                }
                file.close();

                file.open("score.txt", ios::out | ios::app);
                file << userName;
                file.close();
                currentScreen = playing;
            }

            window.clear(Color::Black);
            window.draw(logo);
            window.draw(inputText);
            window.draw(introText);
        }

        if (currentScreen == playing)
        {
            if (Keyboard::isKeyPressed(Keyboard::Up) && !isJumping && !isFalling)
            {
                isJumping = true;
            }
            if (isJumping)
            {
                if (ninja.getPosition().y > jumpY)
                {
                    ninja.move(0.0f, -jumpSpeed);
                }
                else
                {
                    isJumping = false;
                    isFalling = true;
                }
            }
            else if (isFalling)
            {
                if (ninja.getPosition().y < groundY)
                {
                    ninja.move(0.0f, jumpSpeed);
                }
                else
                {
                    isFalling = false;
                }
            }

            if (obstacle.getPosition().x + obstacle.getSize().x > 0)
            {
                obstacle.move(-obstacleSpeed, 0.0f);
            }
            else
            {
                obstacle.setSize(Vector2f(randF(), randF()));
                obstacle.setPosition(window.getSize().x - obstacle.getSize().x, window.getSize().y - surface.getSize().y - obstacle.getSize().y);
                obstacle.setFillColor(Color(rand() % 256, rand() % 256, rand() % 256));
                currentScore++;
                if (currentScore % 7 == 0)
                {
                    obstacleSpeed += 3.0f;
                    jumpSpeed += 1.0f;
                }
            }

            FloatRect ninjaBounds
            (
                ninja.getGlobalBounds().left + 20.0f,
                ninja.getGlobalBounds().top + 20.0f,
                ninja.getGlobalBounds().width - 50.0f,
                ninja.getGlobalBounds().height - 30.0f
            );

            if (ninjaBounds.intersects(obstacle.getGlobalBounds()))
            {
                currentScreen = end;
            }

            scoreOutput = "Score: " + intToStr(currentScore) + "\t Your Last High Score: " + lastHighScore + "\t Overall High Score: " + intToStr(highScore);
            scoreText.setString(scoreOutput);

            window.clear(Color::Black);
            window.draw(logo);
            window.draw(obstacle);
            window.draw(scoreText);
            window.draw(ninja);
        }
        else if (currentScreen == end)
        {
            endText.setString("Game  End!!!\nYour Score: " + intToStr(currentScore));
            window.clear(Color::Black);
            window.draw(endText);
        }

        window.draw(surface);
        window.display();
    }

    return 0;
}

