#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <cstring>

struct GameState {
    float ballPosition[2];
    float paddlePositions[4];
    int scores[4];
};

inline void initializeGameState(GameState& state) {
    memset(&state, 0, sizeof(GameState));
    state.ballPosition[0] = 0.0f;
    state.ballPosition[1] = 0.0f;
    for (int i = 0; i < 4; ++i) {
        state.paddlePositions[i] = 0.0f;
        state.scores[i] = 0;
    }
}

#endif
