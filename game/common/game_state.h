#ifndef GAME_STATE_H
#define GAME_STATE_H

struct GameState {
    float ballPosition[2];
    float paddlePositions[4];
    int scores[4];
};

#endif
