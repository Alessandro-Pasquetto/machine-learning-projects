#include <iostream>
#include <cmath>
#include <bitset>
#include <cassert>

#define NUM_STATES 19683 // 3^9
#define PLAYER_MOVE '1'
#define AI_MOVE '2'

using namespace std;

struct state{
    int nextKeyState = -1;
    int reward = -10;
};

int winCoords[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},  // rows
    {0,3,6}, {1,4,7}, {2,5,8},  // columns
    {0,4,8}, {2,4,6}            // diagonals
};

string getStringState(string result, size_t n){
    result = char('0' + (n % 3)) + result;

    size_t nextN = n/3;

    if(nextN == 0)
        return string(9 - result.length(), '0') + result;

    return getStringState(result, nextN);
}

size_t getKeyState(string state){
    size_t result = 0;

    for (size_t i = 0; i < state.length(); i++)
        result += (state[i] - '0') * pow(3, state.length() - 1 - i);

    return result;
}

bool isWinningState(string state){
    for (const auto& w : winCoords) {
        int a = w[0], b = w[1], c = w[2];

        if (state[a] != '0' && state[a] == state[b] && state[b] == state[c])
            return true;
    }
    return false;
}

bool isFinalState(string state){
    if(isWinningState(state))
        return true;

    for(char c : state){
        if(c == '0')
            return false;
    }

    return true;
}

void printBoard(string state){
    for (int i = 0; i < 9; i++) {
        char c = state[i];

        if (c == '0') cout << ". ";
        else if (c == PLAYER_MOVE) cout << "X ";
        else if (c == AI_MOVE) cout << "O ";

        if ((i + 1) % 3 == 0) cout << endl;
    }
}

// forward declaration
void maxMove(state (&stateSpace)[NUM_STATES], string maxState);

void minMove(state (&stateSpace)[NUM_STATES], string minState){
    size_t keyMinState = getKeyState(minState);
    
    // avoid revisiting already visited states
    if(stateSpace[keyMinState].reward != -10)
        return;

    int minReward = 10;
    string minNextState = "";

    for (size_t i = 0; i < 9; i++){
        if(minState[i] == '0'){
            string nextState = minState;
            nextState[i] = PLAYER_MOVE;

            size_t keyNextState = getKeyState(nextState);

            if(isFinalState(nextState))
                stateSpace[keyNextState].reward = isWinningState(nextState) ? -1 : 0;
            else
                maxMove(stateSpace, nextState);

            int moveReward = stateSpace[keyNextState].reward;

            if(moveReward < minReward){
                minReward = moveReward;
                minNextState = nextState;
            }
        }
    }

    assert(minReward != 10);

    size_t keyMinNextState = getKeyState(minNextState);

    stateSpace[keyMinState].reward = minReward;
    stateSpace[keyMinState].nextKeyState = keyMinNextState;
}

void maxMove(state (&stateSpace)[NUM_STATES], string maxState){
    size_t keyMaxState = getKeyState(maxState);

    // avoid revisiting already visited states
    if(stateSpace[keyMaxState].reward != -10)
        return;

    int maxReward = -10;
    string maxNextState = "";

    for (size_t i = 0; i < 9; i++){
        if(maxState[i] == '0'){
            string nextState = maxState;
            nextState[i] = AI_MOVE;

            size_t keyNextState = getKeyState(nextState);

            if(isFinalState(nextState))
                stateSpace[keyNextState].reward = isWinningState(nextState) ? 1 : 0;
            else
                minMove(stateSpace, nextState);

            int moveReward = stateSpace[keyNextState].reward;

            if(moveReward > maxReward){
                maxReward = moveReward;
                maxNextState = nextState;
            }
        }
    }

    assert(maxReward != -10);
    
    size_t keyMaxNextState = getKeyState(maxNextState);

    stateSpace[keyMaxState].reward = maxReward;
    stateSpace[keyMaxState].nextKeyState = keyMaxNextState;
}

void inputPlayerMove(string &state){
    int move;
    cout << endl << "Inserisci mossa X (0-8): ";
    cin >> move;

    while (move < 0 || move > 8 || state[move] != '0') {
        cout << "Mossa non valida!" << endl;

        cout << endl << "Inserisci mossa X (0-8): ";
        cin >> move;
    }

    state[move] = PLAYER_MOVE;
}

int main(){
    state stateSpace[NUM_STATES];

    string state = "000000000";

    bool aiTurn = true;
    bool aiTurnCopy = aiTurn;

    // training
    if(aiTurn)
        maxMove(stateSpace, state);
    else
        minMove(stateSpace, state);

    // game
    cout << endl << "--New game--";
    
    while (true) {
        if(aiTurnCopy)
            state = getStringState("", stateSpace[getKeyState(state)].nextKeyState);
        else{
            cout << endl << "currentReward: "<<stateSpace[getKeyState(state)].reward<<endl;
            printBoard(state);
            inputPlayerMove(state);
        }

        if(isFinalState(state)){
            printBoard(state);

            if(isWinningState(state))
                if(aiTurnCopy)
                    cout << "Hai perso!";
                else
                    cout << "Hai vinto!";
            else
                cout << "Pareggio!";

            state = getStringState("", 0);
            aiTurnCopy = aiTurn;
            cout << endl << endl << "--New game--";
            continue;
        }

        aiTurnCopy = !aiTurnCopy;
    }

    return 0;
}