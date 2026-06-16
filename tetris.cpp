#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>

const int M = 20;
const int N = 10;
int field[M][N] = {0};

struct Point {
    int x, y;
} a[4], b[4];

int figures[7][4] = {
    {1, 3, 5, 7}, // I
    {2, 4, 5, 7}, // Z
    {3, 5, 4, 6}, // S
    {3, 5, 4, 7}, // T
    {2, 3, 5, 7}, // L
    {3, 5, 7, 6}, // J
    {2, 3, 4, 5}  // O
};

bool check() {
    for (int i = 0; i < 4; i++) {
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return false;
        else if (field[a[i].y][a[i].x]) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // SFML 3 Window Initialization using Vector2u
    sf::RenderWindow window(sf::VideoMode({320, 480}), "TETRIS");

    sf::Texture tex1, tex2, tex3;
    // Warnings are handled naturally by letting files load sequentially
    tex1.loadFromFile("images/tiles.png");
    tex2.loadFromFile("images/background.png");
    tex3.loadFromFile("images/frame.png");

    sf::Sprite s(tex1), background(tex2), frame(tex3);

    int dx = 0;
    bool rotate = false;
    int colorNum = 1;
    float timer = 0;
    float delay = 0.3f;
    sf::Clock clock;

    // Initialize first tetromino piece on startup so 'a' array isn't empty
    int firstPiece = std::rand() % 7;
    for (int i = 0; i < 4; i++) {
        a[i].x = figures[firstPiece][i] % 2 + N / 2 - 1;
        a[i].y = figures[firstPiece][i] / 2;
    }

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        // --- MODERN SFML 3 EVENT LOOP ---
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Up) {
                    rotate = true;
                } else if (keyPressed->code == sf::Keyboard::Key::Left) {
                    dx = -1;
                } else if (keyPressed->code == sf::Keyboard::Key::Right) {
                    dx = 1;
                }
            }
        }

        // Real-time button holding check
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            delay = 0.05f;
        }

        // Horizontal Movement
        for (int i = 0; i < 4; i++) {
            b[i] = a[i];
            a[i].x += dx;
        }
        if (!check()) {
            for (int i = 0; i < 4; i++) a[i] = b[i];
        }

        // Rotation
        if (rotate) {
            Point p = a[1]; // Center of rotation (Using index 1 instead of invalid 'i')
            for (int i = 0; i < 4; i++) {
                int x = a[i].y - p.y;
                int y = a[i].x - p.x;
                a[i].x = p.x - x;
                a[i].y = p.y + y;
            }
            if (!check()) {
                for (int i = 0; i < 4; i++) a[i] = b[i];
            }
        }

        // Tick / Falling Logic
        if (timer > delay) {
            for (int i = 0; i < 4; i++) {
                b[i] = a[i];
                a[i].y += 1;
            }
            if (!check()) {
                for (int i = 0; i < 4; i++) {
                    field[b[i].y][b[i].x] = colorNum;
                }

                colorNum = 1 + std::rand() % 7;
                int n = std::rand() % 7;
                for (int i = 0; i < 4; i++) {
                    a[i].x = figures[n][i] % 2 + N / 2 - 1; // Spawns near horizontal center
                    a[i].y = figures[n][i] / 2;
                }
            }
            timer = 0;
        }

        // Line Clearing Logic
        int k = M - 1;
        for (int i = M - 1; i > 0; i--) {
            int count = 0;
            for (int j = 0; j < N; j++) {
                if (field[i][j]) count++;
                field[k][j] = field[i][j];
            }
            if (count < N) k--;
        }

        // Reset tracking vars
        dx = 0;
        rotate = false;
        delay = 0.3f;

        // --- DRAWING STAGE (Modern SFML 3 Coordinate Vectors) ---
        window.clear(sf::Color::White);
        window.draw(background);

        // Draw Field
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (field[i][j] == 0) continue;
                s.setTextureRect(sf::IntRect({field[i][j] * 18, 0}, {18, 18}));
                s.setPosition({static_cast<float>(j * 18), static_cast<float>(i * 18)});
                s.move({28.f, 31.f});
                window.draw(s);
            }
        }

        // Draw Active falling piece
        for (int i = 0; i < 4; i++) {
            s.setTextureRect(sf::IntRect({colorNum * 18, 0}, {18, 18}));
            s.setPosition({static_cast<float>(a[i].x * 18), static_cast<float>(a[i].y * 18)});
            s.move({28.f, 31.f});
            window.draw(s);
        }

        window.draw(frame);
        window.display();
    }

    return 0;
}