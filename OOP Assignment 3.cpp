#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <limits>
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include<xstring>
// Add SFML includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
using namespace std;

// Forward declarations
class GameItem;
class Coins;
class HelperObject;
class Hurdles;
class Player;
class Board;
class AdventureQuest;

// Base class for all game items
class GameItem
{
protected:
    char* name;
    int pointValue;
    bool isCollected;

public:
    GameItem()
    {
        name = new char[20];
        strcpy_s(name, 20, "Unknown");
        pointValue = 0;
        isCollected = false;
    }

    GameItem(const char* itemName, int points)
    {
        name = new char[strlen(itemName) + 1];
        strcpy_s(name, strlen(itemName) + 1, itemName);
        pointValue = points;
        isCollected = false;
    }

    // Copy constructor
    GameItem(const GameItem& other)
    {
        name = new char[strlen(other.name) + 1];
        strcpy_s(name, strlen(other.name) + 1, other.name);
        pointValue = other.pointValue;
        isCollected = other.isCollected;
    }

    virtual ~GameItem()
    {
        delete[] name;
    }

    virtual void display() const
    {
        cout << name;
    }

    virtual int getPoints() const
    {
        return pointValue;
    }

    virtual const char* getName() const
    {
        return name;
    }

    virtual bool collect()
    {
        if (!isCollected)
        {
            isCollected = true;
            return true;
        }
        return false;
    }

    virtual bool isHurdle() const
    {
        return false;
    }

    virtual bool isHelper() const
    {
        return false;
    }

    virtual bool isCoin() const
    {
        return false;
    }

    virtual int getBlockTurns() const
    {
        return 0;
    }

    virtual int getBackwardCells() const
    {
        return 0;
    }

    virtual void setCollected(bool collected)
    {
        isCollected = collected;
    }

    virtual bool getCollected() const
    {
        return isCollected;
    }

    // For saving to file
    virtual void saveToFile(ofstream& outFile) const
    {
        outFile << name << endl;
        outFile << pointValue << endl;
        outFile << isCollected << endl;
    }
};

// Derived class for coins
class Coins : public GameItem
{
private:
    bool isGold; // true for gold, false for silver

public:
    Coins(bool gold) : GameItem(gold ? "Gold" : "Silver", gold ? 10 : 5)
    {
        isGold = gold;
    }

    // Copy constructor
    Coins(const Coins& other) : GameItem(other)
    {
        isGold = other.isGold;
    }

    bool isCoin() const override
    {
        return true;
    }

    bool isGoldCoin() const
    {
        return isGold;
    }

    void display() const override
    {
        cout << (isGold ? "G" : "S");
    }

    void saveToFile(ofstream& outFile) const override
    {
        outFile << "Coin" << endl;
        GameItem::saveToFile(outFile);
        outFile << isGold << endl;
    }
};

// Derived class for helper objects
class HelperObject : public GameItem
{
private:
    int usesLeft;

public:
    HelperObject(const char* name, int points, int uses) : GameItem(name, points), usesLeft(uses) {}

    // Copy constructor
    HelperObject(const HelperObject& other) : GameItem(other)
    {
        usesLeft = other.usesLeft;
    }

    bool isHelper() const override
    {
        return true;
    }

    bool use()
    {
        if (usesLeft > 0)
        {
            usesLeft--;
            return true;
        }
        return false;
    }

    int getUsesLeft() const
    {
        return usesLeft;
    }

    void display() const override
    {
        if (strcmp(name, "Sword") == 0)
            cout << "Sw";
        else if (strcmp(name, "Shield") == 0)
            cout << "Sh";
        else if (strcmp(name, "Water") == 0)
            cout << "Wt";
        else if (strcmp(name, "Key") == 0)
            cout << "Ky";
        else
            cout << "??";
    }

    void saveToFile(ofstream& outFile) const override
    {
        outFile << "Helper" << endl;
        GameItem::saveToFile(outFile);
        outFile << usesLeft << endl;
    }

    void setUsesLeft(int uses)
    {
        usesLeft = uses;
    }
};

// Derived class for hurdles
class Hurdles : public GameItem
{
private:
    int blockTurns;
    int backwardCells;
    bool requiresGold;
    bool requiresSilver;

public:
    Hurdles(const char* name, int points, int turns, int cells = 0, bool gold = false, bool silver = false)
        : GameItem(name, points), blockTurns(turns), backwardCells(cells), requiresGold(gold), requiresSilver(silver) {}

    // Copy constructor
    Hurdles(const Hurdles& other) : GameItem(other)
    {
        blockTurns = other.blockTurns;
        backwardCells = other.backwardCells;
        requiresGold = other.requiresGold;
        requiresSilver = other.requiresSilver;
    }

    bool isHurdle() const override
    {
        return true;
    }

    int getBlockTurns() const override
    {
        return blockTurns;
    }

    int getBackwardCells() const override
    {
        return backwardCells;
    }

    bool needsGold() const
    {
        return requiresGold;
    }

    bool needsSilver() const
    {
        return requiresSilver;
    }

    void display() const override
    {
        if (strcmp(name, "Fire") == 0)
            cout << "Fr";
        else if (strcmp(name, "Snake") == 0)
            cout << "Sn";
        else if (strcmp(name, "Ghost") == 0)
            cout << "Gh";
        else if (strcmp(name, "Lion") == 0)
            cout << "Ln";
        else if (strcmp(name, "Lock") == 0)
            cout << "Lk";
        else
            cout << "??";
    }

    void saveToFile(ofstream& outFile) const override
    {
        outFile << "Hurdle" << endl;
        GameItem::saveToFile(outFile);
        outFile << blockTurns << endl;
        outFile << backwardCells << endl;
        outFile << requiresGold << endl;
        outFile << requiresSilver << endl;
    }
};

// Player class
class Player
{
private:
    char* name;
    GameItem** inventory;
    int position;
    int goldCoins;
    int silverCoins;
    int points;
    int skipTurns;

    int inventorySize;
    int inventoryCapacity;
    bool zigzagMovement; // true for zigzag, false for spiral

public:
    Player(const char* playerName, int startPos, int gold, int silver, bool zigzag)
    {
        name = new char[strlen(playerName) + 1];
        strcpy_s(name, strlen(playerName) + 1, playerName);

        inventoryCapacity = 10;
        inventory = new GameItem * [inventoryCapacity];
        inventorySize = 0;

        position = startPos;
        goldCoins = gold;
        silverCoins = silver;
        points = 200; // Set initial points to 200
        skipTurns = 0;
        zigzagMovement = zigzag;

        for (int i = 0; i < inventoryCapacity; i++)
        {
            inventory[i] = nullptr;
        }
    }

    // Copy constructor
    Player(const Player& other)
    {
        name = new char[strlen(other.name) + 1];
        strcpy_s(name, strlen(other.name) + 1, other.name);

        position = other.position;
        goldCoins = other.goldCoins;
        silverCoins = other.silverCoins;
        points = other.points;
        skipTurns = other.skipTurns;
        zigzagMovement = other.zigzagMovement;

        inventoryCapacity = other.inventoryCapacity;
        inventorySize = other.inventorySize;
        inventory = new GameItem * [inventoryCapacity];

        // Deep copy inventory
        for (int i = 0; i < inventorySize; i++)
        {
            if (other.inventory[i]->isCoin())
            {
                Coins* coin = (Coins*)other.inventory[i];
                inventory[i] = new Coins(*coin);
            }
            else if (other.inventory[i]->isHelper())
            {
                HelperObject* helper = (HelperObject*)other.inventory[i];
                inventory[i] = new HelperObject(*helper);
            }
            else if (other.inventory[i]->isHurdle())
            {
                Hurdles* hurdle = (Hurdles*)other.inventory[i];
                inventory[i] = new Hurdles(*hurdle);
            }
        }

        // Initialize remaining slots to nullptr
        for (int i = inventorySize; i < inventoryCapacity; i++)
        {
            inventory[i] = nullptr;
        }
    }

    ~Player()
    {
        delete[] name;

        for (int i = 0; i < inventorySize; i++)
        {
            delete inventory[i];
        }
        delete[] inventory;
    }

    Player& operator=(const Player& other)
    {
        if (this != &other)
        {
            // Delete old data
            delete[] name;
            for (int i = 0; i < inventorySize; i++)
            {
                delete inventory[i];
            }
            delete[] inventory;

            // Copy new data
            name = new char[strlen(other.name) + 1];
            strcpy_s(name, strlen(other.name) + 1, other.name);

            position = other.position;
            goldCoins = other.goldCoins;
            silverCoins = other.silverCoins;
            points = other.points;
            skipTurns = other.skipTurns;
            zigzagMovement = other.zigzagMovement;

            inventoryCapacity = other.inventoryCapacity;
            inventorySize = other.inventorySize;
            inventory = new GameItem * [inventoryCapacity];

            // Deep copy inventory
            for (int i = 0; i < inventorySize; i++)
            {
                if (other.inventory[i]->isCoin())
                {
                    Coins* coin = (Coins*)other.inventory[i];
                    inventory[i] = new Coins(*coin);
                }
                else if (other.inventory[i]->isHelper())
                {
                    HelperObject* helper = (HelperObject*)other.inventory[i];
                    inventory[i] = new HelperObject(*helper);
                }
                else if (other.inventory[i]->isHurdle())
                {
                    Hurdles* hurdle = (Hurdles*)other.inventory[i];
                    inventory[i] = new Hurdles(*hurdle);
                }
            }

            // Initialize remaining slots to nullptr
            for (int i = inventorySize; i < inventoryCapacity; i++)
            {
                inventory[i] = nullptr;
            }
        }
        return *this;
    }

    int getPosition() const
    {
        return position;
    }

    void setPosition(int pos)
    {
        position = pos;
    }

    bool hasToSkipTurn() const
    {
        return skipTurns > 0;
    }

    void decreaseSkipTurns()
    {
        if (skipTurns > 0)
        {
            skipTurns--;
        }
    }

    void setSkipTurns(int turns)
    {
        skipTurns = turns;
    }

    int getSkipTurns() const
    {
        return skipTurns;
    }

    int getGoldCoins() const
    {
        return goldCoins;
    }

    int getSilverCoins() const
    {
        return silverCoins;
    }

    int getPoints() const
    {
        return points;
    }

    bool isZigzagMovement() const
    {
        return zigzagMovement;
    }

    void addPoints(int newPoints)
    {
        points += newPoints;
    }

    void losePoints(int lostPoints)
    {
        points -= lostPoints;
        if (points < 0)
        {
            points = 0;
        }
    }

    void addInventoryItem(GameItem* item)
    {
        if (inventorySize < inventoryCapacity)
        {
            inventory[inventorySize++] = item;
        }
        else
        {
            cout << "Inventory full! Cannot add more items.\n";
        }
    }

    bool addCoin(bool isGold)
    {
        if (isGold)
        {
            goldCoins++;
            points += 10;
        }
        else
        {
            silverCoins++;
            points += 5;
        }
        return true;
    }

    bool spendGold(int amount)
    {
        if (goldCoins >= amount)
        {
            goldCoins -= amount;
            return true;
        }
        return false;
    }

    bool spendSilver(int amount)
    {
        if (silverCoins >= amount)
        {
            silverCoins -= amount;
            return true;
        }
        return false;
    }

    void displayInventory() const
    {
        cout << "\n" << name << "'s Inventory:\n";
        cout << "Gold Coins: " << goldCoins << " | Silver Coins: " << silverCoins << " | Points: " << points << endl;

        if (inventorySize == 0)
        {
            cout << "No items in inventory.\n";
            return;
        }

        cout << "Items:\n";
        for (int i = 0; i < inventorySize; i++)
        {
            cout << i + 1 << ". ";
            if (inventory[i]->isHelper())
            {
                HelperObject* helper = (HelperObject*)inventory[i];
                cout << helper->getName() << " (Uses left: " << helper->getUsesLeft() << ")\n";
            }
            else
            {
                cout << inventory[i]->getName() << endl;
            }
        }
    }

    HelperObject* useHelperObject(int index)
    {
        if (index < 0 || index >= inventorySize)
        {
            cout << "Invalid inventory index.\n";
            return nullptr;
        }

        if (!inventory[index]->isHelper())
        {
            cout << "Selected item is not a helper object.\n";
            return nullptr;
        }

        HelperObject* helper = (HelperObject*)inventory[index];

        if (helper->use())
        {
            // If uses left is 0 after using, remove from inventory
            if (helper->getUsesLeft() == 0)
            {
                // Create a temporary copy to return
                HelperObject* usedHelper = new HelperObject(*helper);

                // Remove from inventory and shift remaining items
                delete inventory[index];
                for (int i = index; i < inventorySize - 1; i++)
                {
                    inventory[i] = inventory[i + 1];
                }
                inventorySize--;
                inventory[inventorySize] = nullptr;

                return usedHelper;
            }
            return helper;
        }

        cout << "This helper object has no uses left.\n";
        return nullptr;
    }

    bool hasHelper(const char* helperName) const
    {
        for (int i = 0; i < inventorySize; i++)
        {
            if (inventory[i]->isHelper() && strcmp(inventory[i]->getName(), helperName) == 0)
            {
                return true;
            }
        }
        return false;
    }

    int getHelperIndex(const char* helperName) const
    {
        for (int i = 0; i < inventorySize; i++)
        {
            if (inventory[i]->isHelper() && strcmp(inventory[i]->getName(), helperName) == 0)
            {
                return i;
            }
        }
        return -1; // Not found
    }

    // Added methods for save/load functionality
    int getInventorySize() const
    {
        return inventorySize;
    }

    GameItem* getInventoryItem(int index) const
    {
        if (index >= 0 && index < inventorySize)
        {
            return inventory[index];
        }
        return nullptr;
    }

    const char* getName() const
    {
        return name;
    }

    void saveToFile(ofstream& outFile) const
    {
        outFile << name << endl;
        outFile << position << endl;
        outFile << goldCoins << endl;
        outFile << silverCoins << endl;
        outFile << points << endl;
        outFile << skipTurns << endl;
        outFile << inventorySize << endl;
        outFile << zigzagMovement << endl;

        for (int i = 0; i < inventorySize; i++)
        {
            inventory[i]->saveToFile(outFile);
        }
    }
};

// Board class
class Board
{
private:
    GameItem*** cells;  // 2D array of game items
    int size;          // Board size (e.g., 5 for 5x5)
    int pathLength;    // Number of cells in each path

    // Textures and font for SFML display
    sf::Font font;
    sf::Texture playerTexture;
    sf::Texture player2Texture;
    sf::Texture goldCoinTexture;
    sf::Texture silverCoinTexture;
    sf::Texture swordTexture;
    sf::Texture shieldTexture;
    sf::Texture waterTexture;
    sf::Texture keyTexture;
    sf::Texture fireTexture;
    sf::Texture snakeTexture;
    sf::Texture ghostTexture;
    sf::Texture lionTexture;
    sf::Texture lockTexture;
    sf::Texture goalTexture;

    int selectedPosition; // For cursor highlighting
    int* player1Path;    // Zigzag path for player 1
    int* player2Path;    // Zigzag path for player 2
    int* player1SpiralPath; // Spiral path for player 1
    int* player2SpiralPath; // Spiral path for player 2

    // Helper method to load all textures
    void loadTextures() {
        // Load font from the specified path
        if (!font.loadFromFile("x64/Debug/arial.ttf")) {
            cout << "Error loading font! Trying alternate path..." << endl;
            // Try alternate path
            if (!font.loadFromFile("arial.ttf")) {
                cout << "Could not load font!" << endl;
            }
        }

        // Load player textures from files
        if (!playerTexture.loadFromFile("x64/Debug/assets/p1.png")) {
            cout << "Error loading player 1 texture! Creating default." << endl;
            // Create a default texture for player 1
            playerTexture.create(64, 64);
            sf::Image playerImage;
            playerImage.create(64, 64, sf::Color::Blue);
            playerTexture.update(playerImage);
        }
        
        // Load Player 2 texture
        if (!player2Texture.loadFromFile("x64/Debug/assets/p2.png")) {
            cout << "Error loading player 2 texture! Creating default." << endl;
            // Create a default texture for player 2
            player2Texture.create(64, 64);
            sf::Image player2Image;
            player2Image.create(64, 64, sf::Color::Red);
            player2Texture.update(player2Image);
        }

        // Load all other textures from files with fallbacks
        // Gold coin
        if (!goldCoinTexture.loadFromFile("x64/Debug/assets/gold.png")) {
            cout << "Error loading gold coin texture! Creating default." << endl;
            goldCoinTexture.create(64, 64);
            sf::Image goldImage;
            goldImage.create(64, 64, sf::Color(255, 215, 0)); // Gold color
            goldCoinTexture.update(goldImage);
        }

        // Silver coin
        if (!silverCoinTexture.loadFromFile("x64/Debug/assets/silver.png")) {
            cout << "Error loading silver coin texture! Creating default." << endl;
            silverCoinTexture.create(64, 64);
            sf::Image silverImage;
            silverImage.create(64, 64, sf::Color(192, 192, 192)); // Silver color
            silverCoinTexture.update(silverImage);
        }

        // Sword
        if (!swordTexture.loadFromFile("x64/Debug/assets/sword.png")) {
            cout << "Error loading sword texture! Creating default." << endl;
            swordTexture.create(64, 64);
            sf::Image swordImage;
            swordImage.create(64, 64, sf::Color(220, 20, 60)); // Crimson color
            swordTexture.update(swordImage);
        }

        // Shield
        if (!shieldTexture.loadFromFile("x64/Debug/assets/shield.png")) {
            cout << "Error loading shield texture! Creating default." << endl;
            shieldTexture.create(64, 64);
            sf::Image shieldImage;
            shieldImage.create(64, 64, sf::Color(0, 0, 139)); // Dark blue color
            shieldTexture.update(shieldImage);
        }

        // Water
        if (!waterTexture.loadFromFile("x64/Debug/assets/water.png")) {
            cout << "Error loading water texture! Creating default." << endl;
            waterTexture.create(64, 64);
            sf::Image waterImage;
            waterImage.create(64, 64, sf::Color(173, 216, 230)); // Light blue color
            waterTexture.update(waterImage);
        }

        // Key
        if (!keyTexture.loadFromFile("x64/Debug/assets/key.png")) {
            cout << "Error loading key texture! Creating default." << endl;
            keyTexture.create(64, 64);
            sf::Image keyImage;
            keyImage.create(64, 64, sf::Color(255, 165, 0)); // Orange color
            keyTexture.update(keyImage);
        }

        // Fire
        if (!fireTexture.loadFromFile("x64/Debug/assets/fire.png")) {
            cout << "Error loading fire texture! Creating default." << endl;
            fireTexture.create(64, 64);
            sf::Image fireImage;
            fireImage.create(64, 64, sf::Color(255, 0, 0)); // Red color
            fireTexture.update(fireImage);
        }

        // Snake
        if (!snakeTexture.loadFromFile("x64/Debug/assets/snake.png")) {
            cout << "Error loading snake texture! Creating default." << endl;
            snakeTexture.create(64, 64);
            sf::Image snakeImage;
            snakeImage.create(64, 64, sf::Color(0, 128, 0)); // Green color
            snakeTexture.update(snakeImage);
        }

        // Ghost
        if (!ghostTexture.loadFromFile("x64/Debug/assets/ghost.png")) {
            cout << "Error loading ghost texture! Creating default." << endl;
            ghostTexture.create(64, 64);
            sf::Image ghostImage;
            ghostImage.create(64, 64, sf::Color(211, 211, 211)); // Light gray color
            ghostTexture.update(ghostImage);
        }

        // Lion
        if (!lionTexture.loadFromFile("x64/Debug/assets/lion.png")) {
            cout << "Error loading lion texture! Creating default." << endl;
            lionTexture.create(64, 64);
            sf::Image lionImage;
            lionImage.create(64, 64, sf::Color(139, 69, 19)); // Brown color
            lionTexture.update(lionImage);
        }

        // Lock
        if (!lockTexture.loadFromFile("x64/Debug/assets/lock.png")) {
            cout << "Error loading lock texture! Creating default." << endl;
            lockTexture.create(64, 64);
            sf::Image lockImage;
            lockImage.create(64, 64, sf::Color(105, 105, 105)); // Dark gray color
            lockTexture.update(lockImage);
        }

        // Goal
        if (!goalTexture.loadFromFile("x64/Debug/assets/goal.png")) {
            cout << "Error loading goal texture! Creating default." << endl;
            goalTexture.create(64, 64);
            sf::Image goalImage;
            goalImage.create(64, 64, sf::Color(128, 0, 128)); // Purple color
            goalTexture.update(goalImage);
        }
    }

public:

    // Modify the Board constructor to properly initialize player paths for both zigzag and spiral patterns
    Board(int boardSize)
    {
        size = boardSize;
        pathLength = size * size;
        cells = new GameItem * *[size];
        for (int i = 0; i < size; i++)
        {
            cells[i] = new GameItem * [size];
            for (int j = 0; j < size; j++)
            {
                cells[i][j] = nullptr;
            }
        }

        // Initialize player paths
        player1Path = new int[pathLength];
        player2Path = new int[pathLength];
        player1SpiralPath = new int[pathLength];
        player2SpiralPath = new int[pathLength];

        // Initialize both zigzag and spiral paths
        initializeZigzagPaths();
        initializeSpiralPaths();

        loadTextures();
        selectedPosition = -1; // No position selected initially
    }
    
    // Destructor to properly clean up dynamically allocated memory
    ~Board()
    {
        // Delete all game items in cells
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (cells[i][j] != nullptr)
                {
                    delete cells[i][j];
                    cells[i][j] = nullptr;
                }
            }
            delete[] cells[i];
        }
        delete[] cells;
        
        // Delete path arrays
        delete[] player1Path;
        delete[] player2Path;
        delete[] player1SpiralPath;
        delete[] player2SpiralPath;
    }

    // Add these methods to the Board class:
    void initializeZigzagPaths()
    {
        // Player 1 zigzag path (bottom left to center)
        int index = 0;
        for (int row = size - 1; row >= 0; row--) {
            if ((size - 1 - row) % 2 == 0) {
                // Even rows from bottom: Move left to right
                for (int col = 0; col < size; col++) {
                    player1Path[index++] = row * size + col;
                }
            }
            else {
                // Odd rows from bottom: Move right to left
                for (int col = size - 1; col >= 0; col--) {
                    player1Path[index++] = row * size + col;
                }
            }
        }

        // Player 2 zigzag path (top right to center)
        index = 0;
        for (int row = 0; row < size; row++) {
            if (row % 2 == 0) {
                // Even rows from top: Move right to left
                for (int col = size - 1; col >= 0; col--) {
                    player2Path[index++] = row * size + col;
                }
            }
            else {
                // Odd rows from top: Move left to right
                for (int col = 0; col < size; col++) {
                    player2Path[index++] = row * size + col;
                }
            }
        }
    }

    // Method to initialize spiral paths
    void initializeSpiralPaths() {
        // Generate Player 1's spiral path (starting from top-right corner)
        int index = 0;
        int top = 0, bottom = size - 1, left = 0, right = size - 1;
        int row = 0, col = size - 1; // Starting position for Player 1

        while (index < pathLength) {
            // Move left
            for (int i = col; i >= left && index < pathLength; i--) {
                player1SpiralPath[index++] = row * size + i;
            }
            top++;
            // Move down
            for (int i = row + 1; i <= bottom && index < pathLength; i++) {
                player1SpiralPath[index++] = i * size + left;
            }
            left++;
            // Move right
            for (int i = left; i <= col && index < pathLength; i++) {
                player1SpiralPath[index++] = bottom * size + i;
            }
            bottom--;
            // Move up
            for (int i = bottom; i >= top && index < pathLength; i--) {
                player1SpiralPath[index++] = i * size + col;
            }
            right--;
            col--;
            row++;
        }

        // Reset indices and generate Player 2's spiral path (starting from bottom-left corner)
        index = 0;
        top = 0;
        bottom = size - 1;
        left = 0;
        right = size - 1;
        row = size - 1;
        col = 0; // Starting position for Player 2

        while (index < pathLength) {
            // Move right
            for (int i = col; i <= right && index < pathLength; i++) {
                player2SpiralPath[index++] = row * size + i;
            }
            bottom--;
            // Move up
            for (int i = row - 1; i >= top && index < pathLength; i--) {
                player2SpiralPath[index++] = i * size + right;
            }
            right--;
            // Move left
            for (int i = right; i >= col && index < pathLength; i--) {
                player2SpiralPath[index++] = top * size + i;
            }
            top++;
            // Move down
            for (int i = top; i <= row - 1 && index < pathLength; i++) {
                player2SpiralPath[index++] = i * size + col;
            }

            left++;
            row--;
            col++;
        }
    }

    int getSize() const
    {
        return size;
    }

    void getCoords(int position, int& row, int& col) const
    {
        row = position / size;
        col = position % size;
    }

    GameItem* getCell(int row, int col) const
    {
        if (row < 0 || row >= size || col < 0 || col >= size)
            return nullptr;
        return cells[row][col];
    }

    void setCell(int row, int col, GameItem* item)
    {
        if (row < 0 || row >= size || col < 0 || col >= size)
            return;
        cells[row][col] = item;
    }

    bool isGoalPosition(int position) const
    {
        int center = (size / 2) * size + (size / 2);
        return position == center;
    }

    int getNextPosition(Player* player, bool zigzag) const
    {
        int currentPos = player->getPosition();
        int* path;

        // Choose the right path based on player and movement type
        if (strcmp(player->getName(), "Player 1") == 0) {
            path = zigzag ? player1Path : player1SpiralPath;
        }
        else {
            path = zigzag ? player2Path : player2SpiralPath;
        }

        int currentIndex = -1;
        for (int i = 0; i < pathLength; i++)
        {
            if (path[i] == currentPos)
            {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex == -1 || currentIndex == pathLength - 1)
        {
            return -1; // No next position
        }

        return path[currentIndex + 1];
    }

    // Method to initialize spiral paths
    // Note: These methods are already defined elsewhere in the class
    // Keeping only the declarations here to avoid duplicate definitions

    // Place random objects on the board
    void placeRandomObjects(int minObjects, int maxObjects)
    {
        srand(time(nullptr));
        int numObjects = minObjects + rand() % (maxObjects - minObjects + 1);

        // Calculate player positions to avoid
        int player1Row = size - 1;
        int player1Col = 0;
        int player1Pos = player1Row * size + player1Col;

        int player2Row = 0;
        int player2Col = size - 1;
        int player2Pos = player2Row * size + player2Col;

        // Goal position (center)
        int goalRow = size / 2;
        int goalCol = size / 2;
        int goalPos = goalRow * size + goalCol;

        for (int i = 0; i < numObjects; i++)
        {
            int row, col, pos;
            do
            {
                row = rand() % size;
                col = rand() % size;
                pos = row * size + col;

                // Skip center cell (goal), player 1 and player 2 positions
                if (pos == goalPos || pos == player1Pos || pos == player2Pos)
                {
                    continue;
                }
            } while (cells[row][col] != nullptr);

            // 70% chance of coin, 30% chance of helper object
            if (rand() % 100 < 70)
            {
                // Create coin (70% silver, 30% gold)
                cells[row][col] = new Coins(rand() % 100 < 30);
            }
            else
            {
                // Create helper object
                int helperType = rand() % 4;
                switch (helperType)
                {
                case 0:
                    cells[row][col] = new HelperObject("Sword", 40, 2);
                    break;
                case 1:
                    cells[row][col] = new HelperObject("Shield", 30, 1);
                    break;
                case 2:
                    cells[row][col] = new HelperObject("Water", 50, 1);
                    break;
                case 3:
                    cells[row][col] = new HelperObject("Key", 70, 1);
                    break;
                }
            }
        }
    }

    void displayBoard(Player* player1, Player* player2) const
    {
        cout << "\n===== Adventure Quest Board =====\n\n";

        // Display movement types
        cout << "Player 1 (" << (player1->isZigzagMovement() ? "Zigzag" : "Spiral") << " movement) - Points: " << player1->getPoints() << endl;
        cout << "Player 2 (" << (player2->isZigzagMovement() ? "Zigzag" : "Spiral") << " movement) - Points: " << player2->getPoints() << endl;
        cout << endl;

        // Display column numbers
        cout << "    ";
        for (int j = 0; j < size; j++)
        {
            cout << j << "     ";
        }
        cout << endl;

        // Display top border
        cout << "  +";
        for (int j = 0; j < size; j++)
        {
            cout << "-----+";
        }
        cout << endl;

        for (int i = 0; i < size; i++)
        {
            // Display row number
            cout << i << " |";

            for (int j = 0; j < size; j++)
            {
                int position = i * size + j;

                // Check if players are here
                bool p1Here = (player1->getPosition() == position);
                bool p2Here = (player2->getPosition() == position);

                cout << " ";
                if (p1Here && p2Here)
                {
                    cout << "P*";
                }
                else if (p1Here)
                {
                    cout << " P1 ";
                }
                else if (p2Here)
                {
                    cout << " P2 ";
                }
                else if (i == size / 2 && j == size / 2)
                {
                    // Goal cell
                    cout << " GL ";
                }
                else if (cells[i][j] != nullptr)
                {
                    cells[i][j]->display();
                }
                else
                {
                    cout << "  ";
                }
                cout << " |";
            }
            cout << endl;

            // Display row separator
            cout << "  +";
            for (int j = 0; j < size; j++)
            {
                cout << "-----+";
            }
            cout << endl;
        }

        cout << "\nLegend:\n";
        cout << "P1/P2 - Players | GL - Goal\n";
        cout << "G - Gold Coin | S - Silver Coin\n";
        cout << "Sw - Sword | Sh - Shield | Wt - Water | Ky - Key\n";
        cout << "Fr - Fire | Sn - Snake | Gh - Ghost | Ln - Lion | Lk - Lock\n";
    }

    // SFML rendering method for the board
    void displayBoardSFML(Player* player1, Player* player2, sf::RenderWindow& window) const
    {
        // Clear the window with a dark background
        window.clear(sf::Color(30, 30, 30));

        // Calculate cell size based on window size and board dimensions
        float windowWidth = static_cast<float>(window.getSize().x);
        float windowHeight = static_cast<float>(window.getSize().y);
        float boardSize = min(windowWidth, windowHeight) * 0.8f; // Use 80% of the smallest dimension
        float cellSize = boardSize / static_cast<float>(size);

        // Calculate board position (centered)
        float boardX = (windowWidth - boardSize) / 2.0f;
        float boardY = (windowHeight - boardSize) / 2.0f;

        // Draw board background
        sf::RectangleShape boardBackground(sf::Vector2f(boardSize, boardSize));
        boardBackground.setPosition(boardX, boardY);
        boardBackground.setFillColor(sf::Color(50, 50, 50));
        boardBackground.setOutlineThickness(2.0f);
        boardBackground.setOutlineColor(sf::Color::White);
        window.draw(boardBackground);

        // Draw cells and their contents
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int position = i * size + j;
                float x = boardX + j * cellSize;
                float y = boardY + i * cellSize;

                // Draw cell
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(x, y);
                cell.setFillColor(sf::Color(70, 70, 70));
                cell.setOutlineThickness(1.0f);
                cell.setOutlineColor(sf::Color(100, 100, 100));

                // Highlight cell if it's selected
                if (position == selectedPosition) {
                    cell.setFillColor(sf::Color(100, 100, 150));
                }

                // Special color for goal (center cell)
                if (i == size / 2 && j == size / 2) {
                    cell.setFillColor(sf::Color(128, 0, 128)); // Purple for goal
                }

                window.draw(cell);

                // Check if players are here
                bool p1Here = (player1->getPosition() == position);
                bool p2Here = (player2->getPosition() == position);

                // Draw goal if center cell
                if (i == size / 2 && j == size / 2) {
                    sf::Sprite goalSprite(goalTexture);
                    goalSprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                    goalSprite.setScale(
                        (cellSize * 0.8f) / goalTexture.getSize().x,
                        (cellSize * 0.8f) / goalTexture.getSize().y
                    );
                    window.draw(goalSprite);

                    // Goal is indicated by the purple color and texture
                }

                // Draw the game item in the cell using textures, but only if no player is on this cell
                if (cells[i][j] != nullptr && !cells[i][j]->getCollected() && !p1Here && !p2Here) {
                    sf::Sprite itemSprite;
                    const char* itemName = cells[i][j]->getName();

                    if (cells[i][j]->isCoin()) {
                        Coins* coin = (Coins*)cells[i][j];
                        if (coin->isGoldCoin()) {
                            itemSprite.setTexture(goldCoinTexture);
                        }
                        else {
                            itemSprite.setTexture(silverCoinTexture);
                        }
                    }
                    else if (cells[i][j]->isHelper()) {
                        if (strcmp(itemName, "Sword") == 0) {
                            itemSprite.setTexture(swordTexture);
                        }
                        else if (strcmp(itemName, "Shield") == 0) {
                            itemSprite.setTexture(shieldTexture);
                        }
                        else if (strcmp(itemName, "Water") == 0) {
                            itemSprite.setTexture(waterTexture);
                        }
                        else if (strcmp(itemName, "Key") == 0) {
                            itemSprite.setTexture(keyTexture);
                        }
                    }
                    else if (cells[i][j]->isHurdle()) {
                        if (strcmp(itemName, "Fire") == 0) {
                            itemSprite.setTexture(fireTexture);
                        }
                        else if (strcmp(itemName, "Snake") == 0) {
                            itemSprite.setTexture(snakeTexture);
                        }
                        else if (strcmp(itemName, "Ghost") == 0) {
                            itemSprite.setTexture(ghostTexture);
                        }
                        else if (strcmp(itemName, "Lion") == 0) {
                            itemSprite.setTexture(lionTexture);
                        }
                        else if (strcmp(itemName, "Lock") == 0) {
                            itemSprite.setTexture(lockTexture);
                        }
                    }

                    // Position and scale the sprite
                    itemSprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                    itemSprite.setScale(
                        (cellSize * 0.8f) / itemSprite.getTexture()->getSize().x,
                        (cellSize * 0.8f) / itemSprite.getTexture()->getSize().y
                    );
                    window.draw(itemSprite);

                    // Add small label for helper objects to show uses left
                    if (cells[i][j]->isHelper()) {
                        HelperObject* helper = (HelperObject*)cells[i][j];
                        int uses = helper->getUsesLeft();
                        sf::Text usesText(std::to_string(uses), font, static_cast<unsigned int>(cellSize * 0.2f));
                        usesText.setFillColor(sf::Color::White);
                        usesText.setPosition(x + cellSize * 0.8f, y + cellSize * 0.8f);
                        window.draw(usesText);
                    }
                }

                // Draw players using actual textures
                if (p1Here || p2Here) {
                    if (p1Here && p2Here) {
                        // Both players at same position - draw smaller scaled sprites side by side
                        sf::Sprite p1Sprite(playerTexture);
                        p1Sprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                        p1Sprite.setScale(
                            (cellSize * 0.4f) / playerTexture.getSize().x,
                            (cellSize * 0.4f) / playerTexture.getSize().y
                        );
                        p1Sprite.setColor(sf::Color::Blue); // Tint player 1 blue
                        window.draw(p1Sprite);

                        sf::Sprite p2Sprite(player2Texture);
                        p2Sprite.setPosition(x + cellSize * 0.5f, y + cellSize * 0.1f);
                        p2Sprite.setScale(
                            (cellSize * 0.4f) / player2Texture.getSize().x,
                            (cellSize * 0.4f) / player2Texture.getSize().y
                        );
                        window.draw(p2Sprite);

                        // Players are indicated by the colored sprites (blue for P1, red for P2)
                    }
                    else if (p1Here) {
                        // Only player 1
                        sf::Sprite p1Sprite(playerTexture);
                        p1Sprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                        p1Sprite.setScale(
                            (cellSize * 0.8f) / playerTexture.getSize().x,
                            (cellSize * 0.8f) / playerTexture.getSize().y
                        );
                        p1Sprite.setColor(sf::Color::Blue); // Tint player 1 blue
                        window.draw(p1Sprite);

                        // Player 1 is indicated by the blue sprite
                    }
                    else if (p2Here) {
                        // Only player 2
                        sf::Sprite p2Sprite(player2Texture);
                        p2Sprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                        p2Sprite.setScale(
                            (cellSize * 0.8f) / player2Texture.getSize().x,
                            (cellSize * 0.8f) / player2Texture.getSize().y
                        );
                        window.draw(p2Sprite);

                        // Player 2 is indicated by the red sprite
                    }
                }
            }
        }

        // Draw player info panel on the right
        float panelWidth = windowWidth * 0.2f;
        float panelHeight = windowHeight * 0.8f;
        float panelX = windowWidth - panelWidth - 10.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        sf::RectangleShape infoPanel(sf::Vector2f(panelWidth, panelHeight));
        infoPanel.setPosition(panelX, panelY);
        infoPanel.setFillColor(sf::Color(40, 40, 40));
        infoPanel.setOutlineThickness(2.0f);
        infoPanel.setOutlineColor(sf::Color::White);
        window.draw(infoPanel);

        // Player 1 info
        float textY = panelY + 10.0f;

        sf::Text p1Title("Player 1", font, 20);
        p1Title.setFillColor(sf::Color::Blue);
        p1Title.setPosition(panelX + 10.0f, textY);
        window.draw(p1Title);
        textY += 30.0f;

        sf::Text p1Info("Position: " + std::to_string(player1->getPosition()) +
            "\nGold: " + std::to_string(player1->getGoldCoins()) +
            "\nSilver: " + std::to_string(player1->getSilverCoins()) +
            "\nPoints: " + std::to_string(player1->getPoints()),
            font, 16);
        p1Info.setFillColor(sf::Color::White);
        p1Info.setPosition(panelX + 10.0f, textY);
        window.draw(p1Info);
        textY += 80.0f;

        // Player 1 Inventory
        sf::Text p1InvTitle("Inventory:", font, 18);
        p1InvTitle.setFillColor(sf::Color::Yellow);
        p1InvTitle.setPosition(panelX + 10.0f, textY);
        window.draw(p1InvTitle);
        textY += 25.0f;

        // Display Player 1's inventory items
        for (int i = 0; i < player1->getInventorySize(); i++) {
            GameItem* item = player1->getInventoryItem(i);
            if (item && item->isHelper()) {
                HelperObject* helper = (HelperObject*)item;
                std::string itemText = std::string(helper->getName()) +
                    " (Uses: " + std::to_string(helper->getUsesLeft()) + ")";

                sf::Text itemInfo(itemText, font, 14);
                itemInfo.setFillColor(sf::Color(200, 200, 200));
                itemInfo.setPosition(panelX + 20.0f, textY);
                window.draw(itemInfo);
                textY += 20.0f;
            }
        }

        textY += 30.0f;

        // Player 2 info (below Player 1)
        sf::Text p2Title("Player 2", font, 20);
        p2Title.setFillColor(sf::Color::Red);
        p2Title.setPosition(panelX + 10.0f, textY);
        window.draw(p2Title);
        textY += 30.0f;

        sf::Text p2Info("Position: " + std::to_string(player2->getPosition()) +
            "\nGold: " + std::to_string(player2->getGoldCoins()) +
            "\nSilver: " + std::to_string(player2->getSilverCoins()) +
            "\nPoints: " + std::to_string(player2->getPoints()),
            font, 16);
        p2Info.setFillColor(sf::Color::White);
        p2Info.setPosition(panelX + 10.0f, textY);
        window.draw(p2Info);
        textY += 80.0f;

        // Player 2 Inventory
        sf::Text p2InvTitle("Inventory:", font, 18);
        p2InvTitle.setFillColor(sf::Color::Yellow);
        p2InvTitle.setPosition(panelX + 10.0f, textY);
        window.draw(p2InvTitle);
        textY += 25.0f;

        // Display Player 2's inventory items
        for (int i = 0; i < player2->getInventorySize(); i++) {
            GameItem* item = player2->getInventoryItem(i);
            if (item && item->isHelper()) {
                HelperObject* helper = (HelperObject*)item;
                std::string itemText = std::string(helper->getName()) +
                    " (Uses: " + std::to_string(helper->getUsesLeft()) + ")";

                sf::Text itemInfo(itemText, font, 14);
                itemInfo.setFillColor(sf::Color(200, 200, 200));
                itemInfo.setPosition(panelX + 20.0f, textY);
                window.draw(itemInfo);
                textY += 20.0f;
            }
        }

        // Show whose turn it is
       /* sf::Text turnText("Current Turn: Player " +
            std::string((currentPlayer == 1) ? "1" : "2"),
            font, 20);
        turnText.setPosition(panelX + 10.0f, panelY + panelHeight - 40.0f);
        turnText.setFillColor(sf::Color::Green);
        window.draw(turnText);*/

        // Display the rendered window
        window.display();
    }

    // Update method signature to include current player information
    void displayBoardSFML(Player* player1, Player* player2, sf::RenderWindow& window, int whichPlayerTurn) const
    {
        // Clear the window with a dark background
        window.clear(sf::Color(30, 30, 30));

        // Calculate cell size based on window size and board dimensions
        float windowWidth = static_cast<float>(window.getSize().x);
        float windowHeight = static_cast<float>(window.getSize().y);
        float boardSize = min(windowWidth, windowHeight) * 0.8f; // Use 80% of the smallest dimension
        float cellSize = boardSize / static_cast<float>(size);

        // Calculate board position (centered)
        float boardX = (windowWidth - boardSize) / 2.0f;
        float boardY = (windowHeight - boardSize) / 2.0f;

        // Draw board background
        sf::RectangleShape boardBackground(sf::Vector2f(boardSize, boardSize));
        boardBackground.setPosition(boardX, boardY);
        boardBackground.setFillColor(sf::Color(50, 50, 50));
        boardBackground.setOutlineThickness(2.0f);
        boardBackground.setOutlineColor(sf::Color::White);
        window.draw(boardBackground);

        // Draw cells and their contents
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int position = i * size + j;
                float x = boardX + j * cellSize;
                float y = boardY + i * cellSize;

                // Draw cell
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(x, y);
                cell.setFillColor(sf::Color(70, 70, 70));
                cell.setOutlineThickness(1.0f);
                cell.setOutlineColor(sf::Color(100, 100, 100));

                // Highlight cell if it's selected
                if (position == selectedPosition) {
                    cell.setFillColor(sf::Color(100, 100, 150));
                }

                // Special color for goal (center cell)
                if (i == size / 2 && j == size / 2) {
                    cell.setFillColor(sf::Color(128, 0, 128)); // Purple for goal
                }

                window.draw(cell);

                // Check if players are here
                bool p1Here = (player1->getPosition() == position);
                bool p2Here = (player2->getPosition() == position);

                // Draw goal if center cell
                if (i == size / 2 && j == size / 2) {
                    sf::Sprite goalSprite(goalTexture);
                    goalSprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                    goalSprite.setScale(
                        (cellSize * 0.8f) / goalTexture.getSize().x,
                        (cellSize * 0.8f) / goalTexture.getSize().y
                    );
                    window.draw(goalSprite);

                    // Goal is indicated by the purple color and texture
                }

                // Draw the game item in the cell using textures, but only if no player is on this cell
                if (cells[i][j] != nullptr && !cells[i][j]->getCollected() && !p1Here && !p2Here) {
                    sf::Sprite itemSprite;
                    const char* itemName = cells[i][j]->getName();

                    if (cells[i][j]->isCoin()) {
                        Coins* coin = (Coins*)cells[i][j];
                        if (coin->isGoldCoin()) {
                            itemSprite.setTexture(goldCoinTexture);
                        }
                        else {
                            itemSprite.setTexture(silverCoinTexture);
                        }
                    }
                    else if (cells[i][j]->isHelper()) {
                        if (strcmp(itemName, "Sword") == 0) {
                            itemSprite.setTexture(swordTexture);
                        }
                        else if (strcmp(itemName, "Shield") == 0) {
                            itemSprite.setTexture(shieldTexture);
                        }
                        else if (strcmp(itemName, "Water") == 0) {
                            itemSprite.setTexture(waterTexture);
                        }
                        else if (strcmp(itemName, "Key") == 0) {
                            itemSprite.setTexture(keyTexture);
                        }
                    }
                    else if (cells[i][j]->isHurdle()) {
                        if (strcmp(itemName, "Fire") == 0) {
                            itemSprite.setTexture(fireTexture);
                        }
                        else if (strcmp(itemName, "Snake") == 0) {
                            itemSprite.setTexture(snakeTexture);
                        }
                        else if (strcmp(itemName, "Ghost") == 0) {
                            itemSprite.setTexture(ghostTexture);
                        }
                        else if (strcmp(itemName, "Lion") == 0) {
                            itemSprite.setTexture(lionTexture);
                        }
                        else if (strcmp(itemName, "Lock") == 0) {
                            itemSprite.setTexture(lockTexture);
                        }
                    }

                    // Position and scale the sprite
                    itemSprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                    itemSprite.setScale(
                        (cellSize * 0.8f) / itemSprite.getTexture()->getSize().x,
                        (cellSize * 0.8f) / itemSprite.getTexture()->getSize().y
                    );
                    window.draw(itemSprite);

                    // Add small label for helper objects to show uses left
                    if (cells[i][j]->isHelper()) {
                        HelperObject* helper = (HelperObject*)cells[i][j];
                        int uses = helper->getUsesLeft();
                        sf::Text usesText(std::to_string(uses), font, static_cast<unsigned int>(cellSize * 0.2f));
                        usesText.setFillColor(sf::Color::White);
                        usesText.setPosition(x + cellSize * 0.8f, y + cellSize * 0.8f);
                        window.draw(usesText);
                    }
                }

                // Draw players using actual textures
                if (p1Here || p2Here) {
                    if (p1Here && p2Here) {
                        // Both players at same position - draw smaller scaled sprites side by side
                        sf::Sprite p1Sprite(playerTexture);
                        p1Sprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                        p1Sprite.setScale(
                            (cellSize * 0.4f) / playerTexture.getSize().x,
                            (cellSize * 0.4f) / playerTexture.getSize().y
                        );
                        p1Sprite.setColor(sf::Color::Blue); // Tint player 1 blue
                        window.draw(p1Sprite);

                        sf::Sprite p2Sprite(player2Texture);
                        p2Sprite.setPosition(x + cellSize * 0.5f, y + cellSize * 0.1f);
                        p2Sprite.setScale(
                            (cellSize * 0.4f) / player2Texture.getSize().x,
                            (cellSize * 0.4f) / player2Texture.getSize().y
                        );
                        window.draw(p2Sprite);

                        // Players are indicated by the colored sprites (blue for P1, red for P2)
                    }
                    else if (p1Here) {
                        // Only player 1
                        sf::Sprite p1Sprite(playerTexture);
                        p1Sprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                        p1Sprite.setScale(
                            (cellSize * 0.8f) / playerTexture.getSize().x,
                            (cellSize * 0.8f) / playerTexture.getSize().y
                        );
                        p1Sprite.setColor(sf::Color::Blue); // Tint player 1 blue
                        window.draw(p1Sprite);

                        // Player 1 is indicated by the blue sprite
                    }
                    else if (p2Here) {
                        // Only player 2
                        sf::Sprite p2Sprite(player2Texture);
                        p2Sprite.setPosition(x + cellSize * 0.1f, y + cellSize * 0.1f);
                        p2Sprite.setScale(
                            (cellSize * 0.8f) / player2Texture.getSize().x,
                            (cellSize * 0.8f) / player2Texture.getSize().y
                        );
                        window.draw(p2Sprite);

                        // Player 2 is indicated by the red sprite
                    }
                }
            }
        }

        // Draw player info panel on the right
        float panelWidth = windowWidth * 0.2f;
        float panelHeight = windowHeight * 0.8f;
        float panelX = windowWidth - panelWidth - 10.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        sf::RectangleShape infoPanel(sf::Vector2f(panelWidth, panelHeight));
        infoPanel.setPosition(panelX, panelY);
        infoPanel.setFillColor(sf::Color(40, 40, 40));
        infoPanel.setOutlineThickness(2.0f);
        infoPanel.setOutlineColor(sf::Color::White);
        window.draw(infoPanel);

        // Player 1 info
        float textY = panelY + 10.0f;

        sf::Text p1Title("Player 1", font, 20);
        p1Title.setFillColor(sf::Color::Blue);
        p1Title.setPosition(panelX + 10.0f, textY);
        window.draw(p1Title);
        textY += 30.0f;

        sf::Text p1Info("Position: " + std::to_string(player1->getPosition()) +
            "\nGold: " + std::to_string(player1->getGoldCoins()) +
            "\nSilver: " + std::to_string(player1->getSilverCoins()) +
            "\nPoints: " + std::to_string(player1->getPoints()),
            font, 16);
        p1Info.setFillColor(sf::Color::White);
        p1Info.setPosition(panelX + 10.0f, textY);
        window.draw(p1Info);
        textY += 80.0f;

        // Player 1 Inventory
        sf::Text p1InvTitle("Inventory:", font, 18);
        p1InvTitle.setFillColor(sf::Color::Yellow);
        p1InvTitle.setPosition(panelX + 10.0f, textY);
        window.draw(p1InvTitle);
        textY += 25.0f;

        // Display Player 1's inventory items
        for (int i = 0; i < player1->getInventorySize(); i++) {
            GameItem* item = player1->getInventoryItem(i);
            if (item && item->isHelper()) {
                HelperObject* helper = (HelperObject*)item;
                std::string itemText = std::string(helper->getName()) +
                    " (Uses: " + std::to_string(helper->getUsesLeft()) + ")";

                sf::Text itemInfo(itemText, font, 14);
                itemInfo.setFillColor(sf::Color(200, 200, 200));
                itemInfo.setPosition(panelX + 20.0f, textY);
                window.draw(itemInfo);
                textY += 20.0f;
            }
        }

        textY += 30.0f;

        // Player 2 info (below Player 1)
        sf::Text p2Title("Player 2", font, 20);
        p2Title.setFillColor(sf::Color::Red);
        p2Title.setPosition(panelX + 10.0f, textY);
        window.draw(p2Title);
        textY += 30.0f;

        sf::Text p2Info("Position: " + std::to_string(player2->getPosition()) +
            "\nGold: " + std::to_string(player2->getGoldCoins()) +
            "\nSilver: " + std::to_string(player2->getSilverCoins()) +
            "\nPoints: " + std::to_string(player2->getPoints()),
            font, 16);
        p2Info.setFillColor(sf::Color::White);
        p2Info.setPosition(panelX + 10.0f, textY);
        window.draw(p2Info);
        textY += 80.0f;

        // Player 2 Inventory
        sf::Text p2InvTitle("Inventory:", font, 18);
        p2InvTitle.setFillColor(sf::Color::Yellow);
        p2InvTitle.setPosition(panelX + 10.0f, textY);
        window.draw(p2InvTitle);
        textY += 25.0f;

        // Display Player 2's inventory items
        for (int i = 0; i < player2->getInventorySize(); i++) {
            GameItem* item = player2->getInventoryItem(i);
            if (item && item->isHelper()) {
                HelperObject* helper = (HelperObject*)item;
                std::string itemText = std::string(helper->getName()) +
                    " (Uses: " + std::to_string(helper->getUsesLeft()) + ")";

                sf::Text itemInfo(itemText, font, 14);
                itemInfo.setFillColor(sf::Color(200, 200, 200));
                itemInfo.setPosition(panelX + 20.0f, textY);
                window.draw(itemInfo);
                textY += 20.0f;
            }
        }

        // No legend section - removed as requested

        // Show whose turn it is - make it visible but not too large
        textY = panelY + panelHeight - 40.0f; // Position at the bottom of the panel with some margin
        sf::Text turnText("Current Turn: Player " +
            std::string((whichPlayerTurn == 1) ? "1" : "2"),
            font, 18); // Smaller font size
        
        // Center the text horizontally in the panel
        float textWidth = turnText.getLocalBounds().width;
        turnText.setPosition(panelX + (panelWidth - textWidth) / 2.0f, textY);
        
        // Make the text visible with a subtle outline
        turnText.setFillColor(whichPlayerTurn == 1 ? sf::Color::Blue : sf::Color::Yellow);
        turnText.setOutlineThickness(0.8f);
        turnText.setOutlineColor(sf::Color::White);
        turnText.setStyle(sf::Text::Bold);
        window.draw(turnText);

        // Display the rendered window
        window.display();
    }

    void saveToFile(ofstream& outFile) const
    {
        outFile << size << endl;
        outFile << pathLength << endl;

        // Save cells
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (cells[i][j] == nullptr)
                {
                    outFile << "Empty" << endl;
                }
                else
                {
                    cells[i][j]->saveToFile(outFile);
                }
            }
        }

        // Save zigzag paths
        for (int i = 0; i < pathLength; i++)
        {
            outFile << player1Path[i] << endl;
        }

        for (int i = 0; i < pathLength; i++)
        {
            outFile << player2Path[i] << endl;
        }

        // Save spiral paths
        for (int i = 0; i < pathLength; i++)
        {
            outFile << player1SpiralPath[i] << endl;
        }

        for (int i = 0; i < pathLength; i++)
        {
            outFile << player2SpiralPath[i] << endl;
        }
    }

    int* getPlayer1Path() const
    {
        return player1Path;
    }

    int* getPlayer2Path() const
    {
        return player2Path;
    }

    void displayPaths(const Player* player1, const Player* player2) const
    {
        cout << "\n===== Player Movement Paths =====\n";

        char** gridPlayer1 = new char* [size];
        char** gridPlayer2 = new char* [size];

        // Initialize grids
        for (int i = 0; i < size; i++) {
            gridPlayer1[i] = new char[size];
            gridPlayer2[i] = new char[size];
            for (int j = 0; j < size; j++) {
                gridPlayer1[i][j] = ' ';
                gridPlayer2[i][j] = ' ';
            }
        }

        // Mark player 1's path
        int* path1 = player1->isZigzagMovement() ? player1Path : player1SpiralPath;
        for (int i = 0; i < pathLength; i++) {
            int row = path1[i] / size;
            int col = path1[i] % size;
            gridPlayer1[row][col] = (i < 10) ? ('0' + i) : '*';
        }

        // Mark player 2's path
        int* path2 = player2->isZigzagMovement() ? player2Path : player2SpiralPath;
        for (int i = 0; i < pathLength; i++) {
            int row = path2[i] / size;
            int col = path2[i] % size;
            gridPlayer2[row][col] = (i < 10) ? ('0' + i) : '*';
        }

        // Display player 1's path
        cout << "\nPlayer 1's " << (player1->isZigzagMovement() ? "Zigzag" : "Spiral") << " Path:\n";
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << "[" << gridPlayer1[i][j] << "]";
            }
            cout << endl;
        }

        // Display player 2's path
        cout << "\nPlayer 2's " << (player2->isZigzagMovement() ? "Zigzag" : "Spiral") << " Path:\n";
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << "[" << gridPlayer2[i][j] << "]";
            }
            cout << endl;
        }

        // Free memory
        for (int i = 0; i < size; i++) {
            delete[] gridPlayer1[i];
            delete[] gridPlayer2[i];
        }
        delete[] gridPlayer1;
        delete[] gridPlayer2;

        cout << "Note: Numbers represent steps (0-9), '*' represents later steps\n";
    }

    void setSelectedPosition(int position) {
        selectedPosition = position;
    }

    int getSelectedPosition() const {
        return selectedPosition;
    }

    // Destructor is already defined above
};

// Main game class
class AdventureQuest
{
private:
    Board* gameBoard;
    Player* player1;
    Player* player2;
    bool gameOver;
    int currentPlayer; // 1 or 2

public:
    AdventureQuest(int boardSize, bool newGame = true)
    {
        gameBoard = new Board(boardSize);

        int initialGold = 10;
        int initialSilver = 20;

        if (newGame)
        {
            // Initialize players - ask for movement preference
            char movement1, movement2;
            cout << "Player 1, choose movement type (Z for Zigzag, S for Spiral): ";
            cin >> movement1;
            bool zigzag1 = (movement1 == 'Z' || movement1 == 'z');

            cout << "Player 2, choose movement type (Z for Zigzag, S for Spiral): ";
            cin >> movement2;
            bool zigzag2 = (movement2 == 'Z' || movement2 == 'z');

            // Player 1 starts at bottom left (size-1, 0) = (size-1)*size + 0
            int player1Start = (boardSize - 1) * boardSize;
            // Player 2 starts at top right (0, size-1) = 0*size + (size-1)
            int player2Start = boardSize - 1;

            player1 = new Player("Player 1", player1Start, initialGold, initialSilver, zigzag1);
            player2 = new Player("Player 2", player2Start, initialGold, initialSilver, zigzag2);

            // Place random objects on board
            int minObjects = boardSize == 5 ? 4 : 10;
            int maxObjects = boardSize == 5 ? 10 : 20;
            gameBoard->placeRandomObjects(minObjects, maxObjects);
        }
        else
        {
            // These will be overridden by load game
            int player1Start = (boardSize - 1) * boardSize;
            int player2Start = boardSize - 1;
            player1 = new Player("Player 1", player1Start, initialGold, initialSilver, true);
            player2 = new Player("Player 2", player2Start, initialGold, initialSilver, true);
        }

        gameOver = false;
        currentPlayer = 1;
    }

    // Copy constructor
    AdventureQuest(const AdventureQuest& other)
    {
        gameBoard = new Board(*other.gameBoard);
        player1 = new Player(*other.player1);
        player2 = new Player(*other.player2);
        gameOver = other.gameOver;
        currentPlayer = other.currentPlayer;
    }

    ~AdventureQuest()
    {
        delete gameBoard;
        delete player1;
        delete player2;
    }
    // Implement game loading
    bool gameLoad()
    {
        cout << "Loading game...\n";
        ifstream inFile("savegame.dat", ios::in);
        if (!inFile.is_open())
        {
            cout << "No saved game found.\n";
            return false;
        }

        try {
            // Clear existing data
            delete player1;
            delete player2;
            delete gameBoard;

            // Read board size
            int boardSize;
            inFile >> boardSize;
            inFile.ignore(); // Skip newline

            // Recreate board
            gameBoard = new Board(boardSize);

            // Read player data
            bool player1Zigzag, player2Zigzag;
            int player1Pos, player2Pos;
            int player1Gold, player1Silver, player1Points, player1SkipTurns;
            int player2Gold, player2Silver, player2Points, player2SkipTurns;

            inFile >> player1Zigzag >> player1Pos >> player1Gold >> player1Silver >> player1Points >> player1SkipTurns;
            inFile.ignore(); // Skip newline

            inFile >> player2Zigzag >> player2Pos >> player2Gold >> player2Silver >> player2Points >> player2SkipTurns;
            inFile.ignore(); // Skip newline

            // Create players with loaded data
            player1 = new Player("Player 1", player1Pos, player1Gold, player1Silver, player1Zigzag);
            player1->setSkipTurns(player1SkipTurns);
            player1->addPoints(player1Points);

            player2 = new Player("Player 2", player2Pos, player2Gold, player2Silver, player2Zigzag);
            player2->setSkipTurns(player2SkipTurns);
            player2->addPoints(player2Points);

            // Read current player
            inFile >> currentPlayer;
            inFile.ignore(); // Skip newline

            // Read game board items
            int numItems;
            inFile >> numItems;
            inFile.ignore(); // Skip newline

            for (int i = 0; i < numItems; i++)
            {
                string itemType;
                getline(inFile, itemType);

                int row, col;
                inFile >> row >> col;
                inFile.ignore(); // Skip newline

                string itemName;
                getline(inFile, itemName);

                int pointValue;
                inFile >> pointValue;
                inFile.ignore(); // Skip newline

                bool isCollected;
                inFile >> isCollected;
                inFile.ignore(); // Skip newline

                GameItem* item = nullptr;

                if (itemType == "Coin")
                {
                    bool isGold;
                    inFile >> isGold;
                    inFile.ignore(); // Skip newline

                    item = new Coins(isGold);
                }
                else if (itemType == "Helper")
                {
                    int usesLeft;
                    inFile >> usesLeft;
                    inFile.ignore(); // Skip newline

                    item = new HelperObject(itemName.c_str(), pointValue, usesLeft);
                    ((HelperObject*)item)->setUsesLeft(usesLeft);
                }
                else if (itemType == "Hurdle")
                {
                    int blockTurns, backwardCells;
                    bool requiresGold, requiresSilver;

                    inFile >> blockTurns >> backwardCells >> requiresGold >> requiresSilver;
                    inFile.ignore(); // Skip newline

                    item = new Hurdles(itemName.c_str(), pointValue, blockTurns, backwardCells, requiresGold, requiresSilver);
                }

                if (item != nullptr)
                {
                    item->setCollected(isCollected);
                    gameBoard->setCell(row, col, item);

                    // If item is collected, add it to the appropriate player's inventory
                    if (isCollected)
                    {
                        // Read which player has collected this item
                        int collectedByPlayer;
                        inFile >> collectedByPlayer;
                        inFile.ignore(); // Skip newline

                        Player* targetPlayer = (collectedByPlayer == 1) ? player1 : player2;

                        if (item->isCoin())
                        {
                            Coins* coin = (Coins*)item;
                            targetPlayer->addCoin(coin->isGoldCoin());
                        }
                        else if (item->isHelper())
                        {
                            targetPlayer->addInventoryItem(item);
                        }
                    }
                }
            }

            // Read player inventory items separately
            int player1InventorySize, player2InventorySize;
            inFile >> player1InventorySize;
            inFile.ignore(); // Skip newline

            for (int i = 0; i < player1InventorySize; i++)
            {
                string itemType;
                getline(inFile, itemType);

                if (itemType == "Helper")
                {
                    string itemName;
                    getline(inFile, itemName);

                    int pointValue, usesLeft;
                    inFile >> pointValue >> usesLeft;
                    inFile.ignore(); // Skip newline

                    HelperObject* helper = new HelperObject(itemName.c_str(), pointValue, usesLeft);
                    helper->setUsesLeft(usesLeft);
                    player1->addInventoryItem(helper);
                }
            }

            inFile >> player2InventorySize;
            inFile.ignore(); // Skip newline

            for (int i = 0; i < player2InventorySize; i++)
            {
                string itemType;
                getline(inFile, itemType);

                if (itemType == "Helper")
                {
                    string itemName;
                    getline(inFile, itemName);

                    int pointValue, usesLeft;
                    inFile >> pointValue >> usesLeft;
                    inFile.ignore(); // Skip newline

                    HelperObject* helper = new HelperObject(itemName.c_str(), pointValue, usesLeft);
                    helper->setUsesLeft(usesLeft);
                    player2->addInventoryItem(helper);
                }
            }

            inFile.close();
            cout << "Game loaded successfully.\n";
            return true;
        }
        catch (const exception& e)
        {
            cout << "Error loading game: " << e.what() << endl;
            inFile.close();
            return false;
        }
    }

    // Implement game saving
    void gameSave()
    {
        cout << "Saving game...\n";
        ofstream outFile("savegame.dat", ios::out);
        if (!outFile.is_open())
        {
            cout << "Error opening file for saving.\n";
            return;
        }

        try
        {
            // Save board size
            outFile << gameBoard->getSize() << endl;

            // Save player data (including zigzag movement type)
            outFile << player1->isZigzagMovement() << " "
                << player1->getPosition() << " "
                << player1->getGoldCoins() << " "
                << player1->getSilverCoins() << " "
                << player1->getPoints() << " "
                << player1->getSkipTurns() << endl;

            outFile << player2->isZigzagMovement() << " "
                << player2->getPosition() << " "
                << player2->getGoldCoins() << " "
                << player2->getSilverCoins() << " "
                << player2->getPoints() << " "
                << player2->getSkipTurns() << endl;

            // Save current player
            outFile << currentPlayer << endl;

            // Save game board items
            // First count the number of items
            int numItems = 0;
            for (int i = 0; i < gameBoard->getSize(); i++)
            {
                for (int j = 0; j < gameBoard->getSize(); j++)
                {
                    if (gameBoard->getCell(i, j) != nullptr)
                    {
                        numItems++;
                    }
                }
            }
            outFile << numItems << endl;

            // Now save each item
            for (int i = 0; i < gameBoard->getSize(); i++)
            {
                for (int j = 0; j < gameBoard->getSize(); j++)
                {
                    GameItem* item = gameBoard->getCell(i, j);
                    if (item != nullptr)
                    {
                        if (item->isCoin())
                        {
                            outFile << "Coin" << endl;
                        }
                        else if (item->isHelper())
                        {
                            outFile << "Helper" << endl;
                        }
                        else if (item->isHurdle())
                        {
                            outFile << "Hurdle" << endl;
                        }

                        outFile << i << " " << j << endl;
                        outFile << item->getName() << endl;
                        outFile << item->getPoints() << endl;
                        outFile << item->getCollected() << endl;

                        if (item->isCoin())
                        {
                            Coins* coin = (Coins*)item;
                            outFile << coin->isGoldCoin() << endl;
                        }
                        else if (item->isHelper())
                        {
                            HelperObject* helper = (HelperObject*)item;
                            outFile << helper->getUsesLeft() << endl;
                        }
                        else if (item->isHurdle())
                        {
                            Hurdles* hurdle = (Hurdles*)item;
                            outFile << hurdle->getBlockTurns() << " "
                                << hurdle->getBackwardCells() << " "
                                << hurdle->needsGold() << " "
                                << hurdle->needsSilver() << endl;
                        }

                        // If collected, save which player collected it
                        if (item->getCollected())
                        {
                            // Check if player 1 has this coin
                            if (item->isCoin())
                            {
                                Coins* coin = (Coins*)item;
                                // Check gold/silver count to determine who collected it
                                // This is an approximation; in a real implementation, you'd track who collected what
                                if (coin->isGoldCoin())
                                {
                                    outFile << (player1->getGoldCoins() > 0 ? 1 : 2) << endl;
                                }
                                else
                                {
                                    outFile << (player1->getSilverCoins() > 0 ? 1 : 2) << endl;
                                }
                            }
                            else
                            {
                                // For helpers and hurdles, we'll need a better way to track
                                // For now, default to player 1
                                outFile << 1 << endl;
                            }
                        }
                    }
                }
            }

            // Save player 1's inventory
            outFile << player1->getInventorySize() << endl;
            for (int i = 0; i < player1->getInventorySize(); i++)
            {
                // We only need to save HelperObjects since coins are tracked separately
                if (player1->getInventoryItem(i)->isHelper())
                {
                    outFile << "Helper" << endl;
                    HelperObject* helper = (HelperObject*)player1->getInventoryItem(i);
                    outFile << helper->getName() << endl;
                    outFile << helper->getPoints() << " " << helper->getUsesLeft() << endl;
                }
            }

            // Save player 2's inventory
            outFile << player2->getInventorySize() << endl;
            for (int i = 0; i < player2->getInventorySize(); i++)
            {
                if (player2->getInventoryItem(i)->isHelper())
                {
                    outFile << "Helper" << endl;
                    HelperObject* helper = (HelperObject*)player2->getInventoryItem(i);
                    outFile << helper->getName() << endl;
                    outFile << helper->getPoints() << " " << helper->getUsesLeft() << endl;
                }
            }

            outFile.close();
            cout << "Game saved successfully.\n";
        }
        catch (const exception& e)
        {
            cout << "Error saving game: " << e.what() << endl;
            outFile.close();
        }
    }

    // Implement player turn
    void playerTurn(Player* currentPlayer, Player* otherPlayer)
    {
        cout << "\n=== " << currentPlayer->getName() << "'s Turn ===\n";

        // Display board and player info
        gameBoard->displayBoard(player1, player2);
        currentPlayer->displayInventory();

        // Check if player needs to skip turn
        if (currentPlayer->hasToSkipTurn())
        {
            cout << currentPlayer->getName() << " must skip this turn. "
                << "Turns remaining: " << currentPlayer->getSkipTurns() << endl;
            currentPlayer->decreaseSkipTurns();
            return;
        }

        // Get current position details
        int position = currentPlayer->getPosition();
        int row, col;
        gameBoard->getCoords(position, row, col);

        // Check if there's a hurdle at current position
        GameItem* currentItem = gameBoard->getCell(row, col);
        if (currentItem != nullptr && currentItem->isHurdle() && !currentItem->getCollected())
        {
            Hurdles* hurdle = (Hurdles*)currentItem;

            cout << "You encountered a " << hurdle->getName() << " hurdle!\n";

            // Check if player has helper to deal with the hurdle
            bool canUseHelper = false;

            if (strcmp(hurdle->getName(), "Fire") == 0 && currentPlayer->hasHelper("Water"))
            {
                canUseHelper = true;
                cout << "You can use Water to extinguish the Fire.\n";
            }
            else if (strcmp(hurdle->getName(), "Snake") == 0 && currentPlayer->hasHelper("Sword"))
            {
                canUseHelper = true;
                cout << "You can use Sword to kill the Snake.\n";
            }
            else if (strcmp(hurdle->getName(), "Ghost") == 0 && currentPlayer->hasHelper("Shield"))
            {
                canUseHelper = true;
                cout << "You can use Shield to protect against the Ghost.\n";
            }
            else if (strcmp(hurdle->getName(), "Lion") == 0 && currentPlayer->hasHelper("Sword"))
            {
                canUseHelper = true;
                cout << "You can use Sword to kill the Lion.\n";
            }
            else if (strcmp(hurdle->getName(), "Lock") == 0 && currentPlayer->hasHelper("Key"))
            {
                canUseHelper = true;
                cout << "You can use Key to open the Lock.\n";
            }

            if (canUseHelper)
            {
                char choice;
                cout << "Do you want to use your helper object? (Y/N): ";
                cin >> choice;

                if (choice == 'Y' || choice == 'y')
                {
                    // Use helper object
                    const char* helperName = "";

                    if (strcmp(hurdle->getName(), "Fire") == 0)
                        helperName = "Water";
                    else if (strcmp(hurdle->getName(), "Snake") == 0)
                        helperName = "Sword";
                    else if (strcmp(hurdle->getName(), "Ghost") == 0)
                        helperName = "Shield";
                    else if (strcmp(hurdle->getName(), "Lion") == 0)
                        helperName = "Sword";
                    else if (strcmp(hurdle->getName(), "Lock") == 0)
                        helperName = "Key";

                    int helperIndex = currentPlayer->getHelperIndex(helperName);
                    currentPlayer->useHelperObject(helperIndex);

                    cout << "You used " << helperName << " to overcome the " << hurdle->getName() << "!\n";

                    // Mark hurdle as collected
                    hurdle->collect();
                }
                else
                {
                    // Skip turns
                    cout << "You must skip " << hurdle->getBlockTurns() << " turns!\n";
                    currentPlayer->setSkipTurns(hurdle->getBlockTurns());

                    // Move backward if needed
                    if (hurdle->getBackwardCells() > 0)
                    {
                        // Find position in path
                        int currentPos = currentPlayer->getPosition();
                        int* path = strcmp(currentPlayer->getName(), "Player 1") == 0 ? gameBoard->getPlayer1Path() : gameBoard->getPlayer2Path();
                        int pathLength = gameBoard->getSize() * gameBoard->getSize();

                        int currentIndex = -1;
                        for (int i = 0; i < pathLength; i++)
                        {
                            if (path[i] == currentPos)
                            {
                                currentIndex = i;
                                break;
                            }
                        }

                        if (currentIndex != -1 && currentIndex >= hurdle->getBackwardCells())
                        {
                            int newPos = path[currentIndex - hurdle->getBackwardCells()];
                            currentPlayer->setPosition(newPos);
                            cout << "You moved back " << hurdle->getBackwardCells() << " cells!\n";
                        }
                    }

                    return; // Skip the rest of the turn
                }
            }
            else
            {
                // Skip turns
                cout << "You must skip " << hurdle->getBlockTurns() << " turns!\n";
                currentPlayer->setSkipTurns(hurdle->getBlockTurns());

                // Move backward if needed
                if (hurdle->getBackwardCells() > 0)
                {
                    // Find position in path
                    int currentPos = currentPlayer->getPosition();
                    int* path = strcmp(currentPlayer->getName(), "Player 1") == 0 ? gameBoard->getPlayer1Path() : gameBoard->getPlayer2Path();
                    int pathLength = gameBoard->getSize() * gameBoard->getSize();

                    int currentIndex = -1;
                    for (int i = 0; i < pathLength; i++)
                    {
                        if (path[i] == currentPos)
                        {
                            currentIndex = i;
                            break;
                        }
                    }

                    if (currentIndex != -1 && currentIndex >= hurdle->getBackwardCells())
                    {
                        int newPos = path[currentIndex - hurdle->getBackwardCells()];
                        currentPlayer->setPosition(newPos);
                        cout << "You moved back " << hurdle->getBackwardCells() << " cells!\n";
                    }
                }

                return; // Skip the rest of the turn
            }
        }

        // Show options
        cout << "\nOptions:\n";
        cout << "1. Move forward\n";
        cout << "2. Buy helper object\n";
        cout << "3. Place hurdle on opponent's path\n";
        cout << "4. Skip turn\n";
        cout << "5. Save game\n";

        int choice;
        bool validChoice = false;

        do {
            cout << "Enter your choice (1-5): ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear(); // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
                cout << "Invalid input. Please enter a number." << endl;
            }
            else if (choice < 1 || choice > 5) {
                cout << "Invalid choice. Please enter a number between 1 and 5." << endl;
            }
            else {
                validChoice = true;
            }
        } while (!validChoice);

        switch (choice)
        {
        case 1:
        {
            // Move player
            int nextPos = gameBoard->getNextPosition(currentPlayer, currentPlayer->isZigzagMovement());

            if (nextPos == -1)
            {
                cout << "Cannot move further on this path!\n";
                break;
            }

            // Check if next position has opponent
            if (nextPos == otherPlayer->getPosition())
            {
                cout << "Cannot move to a cell occupied by the other player!\n";
                break;
            }

            // Move player
            currentPlayer->setPosition(nextPos);

            // Get new cell coordinates
            int newRow, newCol;
            gameBoard->getCoords(nextPos, newRow, newCol);

            // Check if we reached goal
            if (gameBoard->isGoalPosition(nextPos))
            {
                cout << currentPlayer->getName() << " reached the goal!\n";
                gameOver = true;
                break;
            }

            // Check if there's an item to collect
            GameItem* cellItem = gameBoard->getCell(newRow, newCol);
            if (cellItem != nullptr && !cellItem->getCollected())
            {
                if (cellItem->isCoin())
                {
                    // Collect coin
                    Coins* coin = (Coins*)cellItem;
                    currentPlayer->addCoin(coin->isGoldCoin());
                    coin->collect();
                    // Remove the item from the cell after collection
                    gameBoard->setCell(newRow, newCol, nullptr);
                    cout << "You collected a " << (coin->isGoldCoin() ? "Gold" : "Silver") << " coin!\n";
                }
                else if (cellItem->isHelper())
                {
                    // Collect helper object
                    HelperObject* helper = (HelperObject*)cellItem;
                    HelperObject* newHelper = new HelperObject(*helper);
                    currentPlayer->addInventoryItem(newHelper);
                    helper->collect();
                    // Remove the item from the cell after collection
                    gameBoard->setCell(newRow, newCol, nullptr);
                    cout << "You collected a " << helper->getName() << "!\n";
                }
            }

            break;
        }
        case 2:
        {
            // Buy helper object
            cout << "\nHelper Objects for Sale:\n";
            cout << "1. Sword (40 points, 2 uses) - Kill Snake or Lion\n";
            cout << "2. Shield (30 points, 1 use) - Protect against Ghost\n";
            cout << "3. Water (50 points, 1 use) - Extinguish Fire\n";
            cout << "4. Key (70 points, 1 use) - Open Lock\n";
            cout << "0. Cancel\n";

            int buyChoice;
            bool validBuyChoice = false;

            do {
                cout << "Enter choice (0-4): ";
                cin >> buyChoice;

                if (cin.fail()) {
                    cin.clear(); // Clear error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
                    cout << "Invalid input. Please enter a number." << endl;
                }
                else if (buyChoice < 0 || buyChoice > 4) {
                    cout << "Invalid choice. Please enter a number between 0 and 4." << endl;
                }
                else {
                    validBuyChoice = true;
                }
            } while (!validBuyChoice);

            if (buyChoice >= 1 && buyChoice <= 4)
            {
                HelperObject* helper = nullptr;
                int cost = 0;

                switch (buyChoice)
                {
                case 1:
                    helper = new HelperObject("Sword", 40, 2);
                    cost = 40;
                    break;
                case 2:
                    helper = new HelperObject("Shield", 30, 1);
                    cost = 30;
                    break;
                case 3:
                    helper = new HelperObject("Water", 50, 1);
                    cost = 50;
                    break;
                case 4:
                    helper = new HelperObject("Key", 70, 1);
                    cost = 70;
                    break;
                }

                // Check if player has enough points
                if (currentPlayer->getPoints() >= cost)
                {
                    currentPlayer->losePoints(cost);
                    currentPlayer->addInventoryItem(helper);
                    cout << "You bought a " << helper->getName() << "!\n";
                }
                else
                {
                    cout << "Not enough points to buy this item!\n";
                    delete helper;
                }
            }
            break;
        }
        case 3:
        {
            // Place hurdle on opponent's path
            cout << "\nHurdles to Place:\n";
            cout << "1. Fire (50 points) - Block for 2 turns\n";
            cout << "2. Snake (30 points) - Block for 3 turns, move back 3 cells\n";
            cout << "3. Ghost (20 points) - Block for 1 turn\n";
            cout << "4. Lion (50 points, gold coins only) - Block for 4 turns\n";
            cout << "5. Lock (60 points, silver coins only) - Block until key used\n";
            cout << "0. Cancel\n";

            int hurdleChoice;
            bool validHurdleChoice = false;

            do {
                cout << "Enter choice (0-5): ";
                cin >> hurdleChoice;

                if (cin.fail()) {
                    cin.clear(); // Clear error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
                    cout << "Invalid input. Please enter a number." << endl;
                }
                else if (hurdleChoice < 0 || hurdleChoice > 5) {
                    cout << "Invalid choice. Please enter a number between 0 and 5." << endl;
                }
                else {
                    validHurdleChoice = true;
                }
            } while (!validHurdleChoice);

            if (hurdleChoice == 0) {
                break;
            }

            if (hurdleChoice >= 1 && hurdleChoice <= 5)
            {
                // Get opponent's next position
                int opponentNextPos = gameBoard->getNextPosition(otherPlayer, otherPlayer->isZigzagMovement());

                if (opponentNextPos == -1)
                {
                    cout << "Cannot place hurdle - opponent has no valid next position!\n";
                    break;
                }

                int nextRow, nextCol;
                gameBoard->getCoords(opponentNextPos, nextRow, nextCol);

                // Check if cell already has a hurdle
                if (gameBoard->getCell(nextRow, nextCol) != nullptr)
                {
                    cout << "Cell already has an item - cannot place hurdle here!\n";
                    break;
                }

                Hurdles* hurdle = nullptr;
                int cost = 0;
                bool requiresGold = false;
                bool requiresSilver = false;

                switch (hurdleChoice)
                {
                case 1:
                    hurdle = new Hurdles("Fire", 50, 2);
                    cost = 50;
                    break;
                case 2:
                    hurdle = new Hurdles("Snake", 30, 3, 3);
                    cost = 30;
                    break;
                case 3:
                    hurdle = new Hurdles("Ghost", 20, 1);
                    cost = 20;
                    break;
                case 4:
                    hurdle = new Hurdles("Lion", 50, 4, 0, true, false);
                    cost = 50;
                    requiresGold = true;
                    break;
                case 5:
                    hurdle = new Hurdles("Lock", 60, 999, 0, false, true); // "infinite" turns until key used
                    cost = 60;
                    requiresSilver = true;
                    break;
                }

                // Check if player has enough points and coins
                bool canBuy = currentPlayer->getPoints() >= cost;

                if (requiresGold && currentPlayer->getGoldCoins() < cost / 10)
                {
                    cout << "Not enough gold coins to buy this hurdle!\n";
                    delete hurdle;
                    break;
                }

                if (requiresSilver && currentPlayer->getSilverCoins() < cost / 5)
                {
                    cout << "Not enough silver coins to buy this hurdle!\n";
                    delete hurdle;
                    break;
                }

                if (canBuy)
                {
                    currentPlayer->losePoints(cost);

                    // Deduct coins if needed
                    if (requiresGold)
                    {
                        currentPlayer->spendGold(cost / 10);
                    }
                    else if (requiresSilver)
                    {
                        currentPlayer->spendSilver(cost / 5);
                    }

                    // Place hurdle
                    gameBoard->setCell(nextRow, nextCol, hurdle);
                    cout << "You placed a " << hurdle->getName() << " on the opponent's path!\n";
                }
                else
                {
                    cout << "Not enough points to buy this hurdle!\n";
                    delete hurdle;
                }
            }
            break;
        }
        case 4:
            // Skip turn
            cout << "Turn skipped.\n";
            break;
        case 5:
            // Save game
            gameSave();
            break;
        default:
            cout << "Invalid choice!\n";
        }
    }

    // Run the game
    void run()
    {
        cout << "\n=== Adventure Quest Game ===\n";

        // Create SFML window
        sf::RenderWindow window(sf::VideoMode(1024, 768), "Adventure Quest Game");
        window.setFramerateLimit(60);

        // Display player paths at the start of the game
        gameBoard->displayPaths(player1, player2);

        while (!gameOver && window.isOpen())
        {
            // Process SFML window events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    return;
                }
            }

            // Clear console for next output
            system("cls");

            Player* currentPlayerPtr = (currentPlayer == 1) ? player1 : player2;
            Player* otherPlayerPtr = (currentPlayer == 1) ? player2 : player1;

            // Display the board with SFML
            gameBoard->displayBoardSFML(player1, player2, window, currentPlayer);

            // Handle player turn through console
            playerTurn(currentPlayerPtr, otherPlayerPtr);

            // Update the visual display after the turn
            gameBoard->displayBoardSFML(player1, player2, window, currentPlayer);

            // Check for game over condition
            if (gameBoard->isGoalPosition(player1->getPosition()) ||
                gameBoard->isGoalPosition(player2->getPosition()))
            {
                // Both players reached goal, compare points
                cout << "\n=== Game Over! ===\n";
                cout << "Player 1 Points: " << player1->getPoints() << endl;
                cout << "Player 2 Points: " << player2->getPoints() << endl;

                if (player1->getPoints() > player2->getPoints())
                {
                    cout << "Player 1 wins!\n";
                }
                else if (player2->getPoints() > player1->getPoints())
                {
                    cout << "Player 2 wins!\n";
                }
                else
                {
                    cout << "It's a draw!\n";
                }

                gameOver = true;

                // Display final state in SFML
                gameBoard->displayBoardSFML(player1, player2, window, currentPlayer);

                break;
            }
            else if (gameBoard->isGoalPosition(player1->getPosition()))
            {
                cout << "\nPlayer 1 reached the goal! Waiting for Player 2...\n";
            }
            else if (gameBoard->isGoalPosition(player2->getPosition()))
            {
                cout << "\nPlayer 2 reached the goal! Waiting for Player 1...\n";
            }

            // Switch to next player
            currentPlayer = (currentPlayer == 1) ? 2 : 1;

            // Pause for players to switch
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
        }

        // Keep window open until user closes it if game is over
        if (gameOver)
        {
            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                    {
                        window.close();
                    }
                }

                // Keep updating the display
                gameBoard->displayBoardSFML(player1, player2, window, currentPlayer);

                // Sleep to prevent high CPU usage
                sf::sleep(sf::milliseconds(100));
            }
        }

        // Ask if players want to continue to next level
        if (gameOver)
        {
            char nextLevel;
            cout << "Do you want to continue to the next level? (Y/N): ";
            cin >> nextLevel;

            if (nextLevel == 'Y' || nextLevel == 'y')
            {
                // Calculate new board size (next odd number)
                int currentSize = gameBoard->getSize();
                int newSize = currentSize + 2;
                if (newSize > 11)
                {
                    cout << "Maximum board size reached. Starting with 5x5 board.\n";
                    newSize = 5;
                }

                // Keep player gold and silver
                int player1Gold = player1->getGoldCoins();
                int player1Silver = player1->getSilverCoins();
                int player2Gold = player2->getGoldCoins();
                int player2Silver = player2->getSilverCoins();

                // Add bonus (same for both players)
                int goldBonus = 5;
                int silverBonus = 10;

                // Create new game with larger board
                delete gameBoard;
                delete player1;
                delete player2;

                // Create new board
                gameBoard = new Board(newSize);

                // Ask for movement preferences
                char movement1, movement2;
                cout << "Player 1, choose movement type (Z for Zigzag, S for Spiral): ";
                cin >> movement1;
                bool zigzag1 = (movement1 == 'Z' || movement1 == 'z');

                cout << "Player 2, choose movement type (Z for Zigzag, S for Spiral): ";
                cin >> movement2;
                bool zigzag2 = (movement2 == 'Z' || movement2 == 'z');

                // Create players with carried over coins plus bonus
                player1 = new Player("Player 1", 0, player1Gold + goldBonus, player1Silver + silverBonus, zigzag1);
                player2 = new Player("Player 2", newSize * newSize - 1, player2Gold + goldBonus, player2Silver + silverBonus, zigzag2);

                // Place random objects on board
                int minObjects = newSize == 5 ? 4 : 10;
                int maxObjects = newSize == 5 ? 10 : 20;
                gameBoard->placeRandomObjects(minObjects, maxObjects);

                // Reset game state
                gameOver = false;
                currentPlayer = 1; // Player 1 starts

                // Continue with new game
                run();
            }
        }
    }
};

// Main function
int main()
{
    system("color 2");
    srand(time(nullptr));

    cout << "===== Welcome to Adventure Quest =====\n\n";
    cout << "This game uses SFML for visual display and console for input.\n\n";

    // Create a small test window to verify SFML works correctly
    try {
        sf::RenderWindow testWindow(sf::VideoMode(200, 100), "SFML Test", sf::Style::None);
        testWindow.close();
        cout << "SFML initialized successfully.\n\n";
    }
    catch (const std::exception& e) {
        cout << "Error initializing SFML: " << e.what() << endl;
        cout << "Please make sure SFML 2.6 libraries are correctly installed.\n";
        cout << "Press Enter to try to continue anyway...\n";
        cin.get();
    }

    char loadChoice;
    cout << "Do you want to load a saved game? (Y/N): ";
    cin >> loadChoice;

    AdventureQuest* game;

    if (loadChoice == 'Y' || loadChoice == 'y')
    {
        game = new AdventureQuest(5, false); // Create temporary game with default size
        if (!game->gameLoad())
        {
            delete game;
            game = new AdventureQuest(5); // Create new game with 5x5 board
        }
    }
    else
    {
        int boardSize;
        cout << "Enter board size (odd number between 5 and 11): ";
        cin >> boardSize;

        // Ensure board size is odd and within range
        if (boardSize < 5)
            boardSize = 5;
        if (boardSize > 11)
            boardSize = 11;
        if (boardSize % 2 == 0)
            boardSize++; // Make it odd

        game = new AdventureQuest(boardSize);
    }

    game->run();

    delete game;
    return 0;
}
