#ifndef EVENTSIMULATION_H
#define EVENTSIMULATION_H

// Prototypes and definitions for simulator functions

void sendMessage(int dest, int val);
void sendAntimessage(int dest, int val);

void statePush(char* stateSave, int x);
int statePop(char* stateSave);

inline void sendMessage__forward(int dest, int val, char* stateSave) {
  sendMessage(dest, val);
}
inline void sendMessage__backward(int dest, int val, char* stateSave) {
  sendAntimessage(dest, val);
}

#endif /* EVENTSIMULATION_H */
