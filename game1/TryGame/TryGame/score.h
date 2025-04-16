#ifndef SCORE_H
#define SCORE_H

#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class ScoreManager {
public:
    ScoreManager(const std::string& filename = "..\\assets\\score\\highscore.txt");

    void loadScores();
    void saveScores() const;
    void addScore(int score);
    const std::vector<int>& getHighScores() const;
    void showHighScores(SDL_Renderer* renderer, bool& isRunning);

private:
    std::string filename;
    std::vector<int> scores;
    static const int MAX_SCORES = 10;

    void sortScores();
};

#endif
