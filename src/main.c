#include <math.h>
#include <pthread.h>
#include <unistd.h>

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "style_cyber.h"

#include "graph.h"



static pthread_t threadId = { 0 };

static const int screenWidth = 1000;
static const int screenHeight = 600;
static Camera2D camera = { 0 };

static Vector2 mouseScreenPos = {0.0f, 0.0f};
static Vector2 mouseWorldPos = {0.0f, 0.0f};

static const int padding = 20;
static const int panelWidth = 240;
static const int controlWidth = panelWidth - 2 * padding;
static const int controlHeight = 33;

static const float nodeSize = 20.0f;
static int mode = 1;
static int selectedNode = -1;

static int dropDownActive = 0;
static bool dropDownEditMode = false;

static Graph *g = NULL;




static int NearestNode(Vector2 pos)
{
    for (int i = 0; i < g->nodes_i; ++i)
    {
        GraphNode *n = &g->nodes[i];
        Vector2 center = {n->x, n->y};
        if (CheckCollisionPointCircle(mouseWorldPos, center, nodeSize * 2))
        {
            return i;
        }
    }
    return -1;
}




void HandleModeFree(void)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouseScreenPos.x > panelWidth + 2*padding)
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        camera.offset = mouseScreenPos;
        camera.target = mouseWorldPos;
        float scaleFactor = 1.0f + (0.25f*fabsf(wheel));
        if (wheel < 0) scaleFactor = 1.0f/scaleFactor;
        camera.zoom = Clamp(camera.zoom*scaleFactor, 0.125f, 64.0f);
    }
}

void HandleModeAdd(void)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseScreenPos.x > panelWidth + 2*padding)
    {
        int nearest = NearestNode(mouseWorldPos);
        if (nearest == -1)
        {
            int added = GraphAddNode(g, mouseWorldPos.x, mouseWorldPos.y);
            TraceLog(LOG_INFO, "Added node %d at %f %f", added, mouseWorldPos.x, mouseWorldPos.y);
            if (selectedNode != -1)
            {
                TraceLog(LOG_INFO, "Added edge %d -> %d", selectedNode, added);
                GraphAddEdge(g, selectedNode, added);
                selectedNode = -1;
            }
            return;
        }
        if (selectedNode == -1)
        {
            selectedNode = nearest;
            return;
        }
        if (selectedNode == nearest)
        {
            selectedNode = -1;
            return;
        }
        if (!GraphHasEdge(g, selectedNode, nearest))
        {
            TraceLog(LOG_INFO, "Added edge %d -> %d", selectedNode, nearest);
            GraphAddEdge(g, selectedNode, nearest);
            selectedNode = -1;
            return;
        }
        TraceLog(LOG_INFO, "Edge %d -> %d already exists", selectedNode, nearest);
        selectedNode = -1;
    }
}

static int selectedMoveNode = -1;
void HandleModeMove(void)
{
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        if (selectedMoveNode == -1)
        {
            selectedMoveNode = NearestNode(mouseWorldPos);
        }
        if (selectedMoveNode != -1)
        {
            GraphNode *n = &g->nodes[selectedMoveNode];
            n->x = mouseWorldPos.x;
            n->y = mouseWorldPos.y;
        }
    }
    else
    {
        selectedMoveNode = -1;
    }
}






void DrawArrow(Vector2 start, Vector2 end, Color color)
{
    Vector2 offset = Vector2Scale(Vector2Normalize(Vector2Subtract(end, start)), nodeSize);
    Vector2 s = Vector2Add(start, offset);
    Vector2 e = Vector2Subtract(end, offset);
    Vector2 e2 = Vector2Subtract(e, offset);
    Vector2 perp = {-offset.y / 2, offset.x / 2};
    Vector2 p1 = Vector2Add(e2, perp);
    Vector2 p2 = Vector2Subtract(e2, perp);
    DrawLineEx(s, e2, 2, color);
    DrawTriangle(e, p2, p1, color);
}


void DrawGraph(Graph *g)
{
    for (size_t i = 0; i < g->nodes_i; ++i)
    {
        GraphNode *node = &g->nodes[i];
        Vector2 from = {node->x, node->y};
        DrawCircleV(from, nodeSize, GetColor(GuiGetStyle(DEFAULT, selectedNode == i ? BASE_COLOR_PRESSED : BASE_COLOR_FOCUSED)));
        GraphEdge *edge = node->first;
        while (edge)
        {
            node = &g->nodes[edge->node];
            Vector2 to = {node->x, node->y};
            DrawArrow(from, to, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED)));
            edge = edge->next;
        }
        DrawText(TextFormat("%d", i), from.x - nodeSize/4, from.y - nodeSize/2, nodeSize, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED)));
    }
}


static void *GraphBFS(void *arg)
{
    // TODO: BFS
    TraceLog(LOG_INFO, "BFS started");
    sleep(1);
    TraceLog(LOG_INFO, "BFS finished");
    return NULL;
}

static void *GraphDFS(void *arg)
{
    // TODO: DFS
    TraceLog(LOG_INFO, "DFS started");
    sleep(1);
    TraceLog(LOG_INFO, "DFS finished");
    return NULL;
}

static void DispatchThread(void)
{
    void *(*threadFunction)(void *) = NULL;
    switch (dropDownActive)
    {
        case 0:
            threadFunction = GraphBFS;
            break;
        case 1:
            threadFunction = GraphDFS;
            break;
        default:
            break;
    }
    if (pthread_create(&threadId, NULL, threadFunction, NULL) != 0)
        TraceLog(LOG_ERROR, "Error creating loading thread");
    else TraceLog(LOG_INFO, "Loading thread initialized successfully");
}





int main(void)
{

    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screenWidth, screenHeight, "Graphs");
    GuiLoadStyleCyber();
    SetTargetFPS(60);
    camera.zoom = 1.0f;


    g = GraphCreate();

    while (!WindowShouldClose())
    {
        mouseScreenPos = GetMousePosition();
        mouseWorldPos = GetScreenToWorld2D(mouseScreenPos, camera);

        switch (GetKeyPressed())
        {
            case KEY_F:
                mode = 0;
                break;
            case KEY_A:
                mode = 1;
                break;
            case KEY_M:
                mode = 2;
                break;
            case KEY_R:
                camera.zoom = 1.0f;
                camera.offset = camera.target = (Vector2){0.0f, 0.0f};
                break;

            default:
                break;
        }


        switch (mode)
        {
            case 0:
                HandleModeFree();
                break;
            case 1:
                HandleModeAdd();
                break;
            case 2:
                HandleModeMove();
                break;
            default:
                break;
        }



        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            BeginMode2D(camera);
                DrawGraph(g);
            EndMode2D();





            // raygui ------------------------------------------------------
            if (dropDownEditMode) GuiLock();
            Rectangle panelRect = { padding, padding, panelWidth, screenHeight - 2 * padding};
            DrawRectangleRec(panelRect, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_DISABLED)));
            GuiGroupBox(panelRect, "Control Panel");
            int gui_i = 0;


            GuiToggleGroup((Rectangle){ 2*padding, 2*padding + gui_i*(controlHeight+padding), 
                            controlWidth/3.0f, controlHeight}, "[F]REE;[A]DD;[M]OVE", &mode);
            ++gui_i;


            if (GuiButton((Rectangle){ 2*padding, 2*padding + gui_i*(controlHeight+padding), controlWidth, controlHeight}, "[R]ESET CAMERA"))
            {
                camera.zoom = 1.0f;
                camera.offset = camera.target = (Vector2){0.0f, 0.0f};
            }
            ++gui_i;


            GuiLine((Rectangle){ 2*padding, 2*padding + gui_i*(controlHeight+padding), controlWidth, controlHeight}, NULL);
            ++gui_i;


            if (GuiDropdownBox((Rectangle){ 2*padding, 2*padding + gui_i*(controlHeight+padding), controlWidth/2.-padding/2., controlHeight}, 
                               "BFS;DFS", &dropDownActive, dropDownEditMode)) dropDownEditMode = !dropDownEditMode;
            if (GuiButton((Rectangle){ 2.5*padding + controlWidth/2., 2.*padding + gui_i*(controlHeight+padding), controlWidth/2.-padding/2., controlHeight}, "START"))
            {
                if (selectedNode != -1) DispatchThread();
                else TraceLog(LOG_WARNING, "No node selected");
            }
            ++gui_i;


            GuiUnlock();
            // raygui ------------------------------------------------------


            DrawFPS(screenWidth - 80, 0);

        EndDrawing();
    }



    GraphDestroy(g);

    CloseWindow();

    return 0;
}
