/*******************************************************************************************
*
*   Layout v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_LAYOUT_IMPLEMENTATION
*       #include "gui_layout.h"
*
*       INIT: GuiLayoutState state = InitGuiLayout();
*       DRAW: GuiLayout(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_LAYOUT_H
#define GUI_LAYOUT_H

typedef struct {
    int modeActive;
    bool resetCameraPressed;
    bool algoEditMode;
    int algoActive;
    bool startPressed;
    bool autoPressed;
    bool stepPressed;

    Rectangle layoutRecs[8];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiLayoutState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiLayoutState InitGuiLayout(void);
void GuiLayout(GuiLayoutState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_LAYOUT_H

/***********************************************************************************
*
*   GUI_LAYOUT IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_LAYOUT_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiLayoutState InitGuiLayout(void)
{
    GuiLayoutState state = { 0 };

    state.modeActive = 0;
    state.resetCameraPressed = false;
    state.algoEditMode = false;
    state.algoActive = 0;
    state.startPressed = false;
    state.autoPressed = false;
    state.stepPressed = false;

    state.layoutRecs[0] = (Rectangle){ 24, 24, 247, 496 };
    state.layoutRecs[1] = (Rectangle){ 36, 36, 70, 30 };
    state.layoutRecs[2] = (Rectangle){ 34, 75, 214, 29 };
    state.layoutRecs[3] = (Rectangle){ 34, 118, 214, 17 };
    state.layoutRecs[4] = (Rectangle){ 40, 152, 111, 28 };
    state.layoutRecs[5] = (Rectangle){ 160, 153, 92, 29 };
    state.layoutRecs[6] = (Rectangle){ 168, 191, 79, 27 };
    state.layoutRecs[7] = (Rectangle){ 47, 195, 100, 23 };

    // Custom variables initialization

    return state;
}

void GuiLayout(GuiLayoutState *state)
{
    if (state->algoEditMode) GuiLock();

    GuiGroupBox(state->layoutRecs[0], "CONTROL PANEL");
    GuiToggleGroup(state->layoutRecs[1], "[F]REE;[A]DD;[M]OVE", &state->modeActive);
    state->resetCameraPressed = GuiButton(state->layoutRecs[2], "[R]ESET CAMERA"); 
    GuiLine(state->layoutRecs[3], NULL);
    state->startPressed = GuiButton(state->layoutRecs[5], "START"); 
    state->autoPressed = GuiButton(state->layoutRecs[6], "AUTO"); 
    state->stepPressed = GuiButton(state->layoutRecs[7], "STEP"); 
    if (GuiDropdownBox(state->layoutRecs[4], "BFS;DFS", &state->algoActive, state->algoEditMode)) state->algoEditMode = !state->algoEditMode;
    
    GuiUnlock();
}

#endif // GUI_LAYOUT_IMPLEMENTATION
