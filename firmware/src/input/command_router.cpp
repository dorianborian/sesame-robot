#include "command_router.h"
#include "../../movement-sequences.h"
#include "../globals.h"

static void clearCommandIfMatch(const String& cmd) {
  if (currentCommand == cmd) currentCommand = "";
}

void routeCurrentCommand() {
    if (currentCommand.isEmpty()) {
        return;
    }

    const String cmd = currentCommand;

    if (cmd == "forward") {
        runWalkPose();
    } 
    else if (cmd == "backward") {
        runWalkBackward();
    } 
    else if (cmd == "left") {
        runTurnLeft();
    } 
    else if (cmd == "right") {
        runTurnRight();
    } 
    else if (cmd == "wave") {
        runWavePose();
    } 
    else if (cmd == "dance") {
        runDancePose();
    } 
    else if (cmd == "swim") {
        runSwimPose();
    } 
    else if (cmd == "point") {
        runPointPose();
    } 
    else if (cmd == "pushup") {
        runPushupPose();
    } 
    else if (cmd == "bow") {
        runBowPose();
    } 
    else if (cmd == "cute") {
        runCutePose();
    } 
    else if (cmd == "freaky") {
        runFreakyPose();
    } 
    else if (cmd == "worm") {
        runWormPose();
    } 
    else if (cmd == "shake") {
        runShakePose();
    } 
    else if (cmd == "shrug") {
        runShrugPose();
    } 
    else if (cmd == "dead") {
        runDeadPose();
    } 
    else if (cmd == "crab") {
        runCrabPose();
    } 
    else if (cmd == "rest") {
        runRestPose();
        clearCommandIfMatch(cmd);
    } 
    else if (cmd == "stand") {
        runStandPose(1);
        clearCommandIfMatch(cmd);
    }
}
