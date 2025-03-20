#include <math.h>
#include <pthread.h>
#include <unistd.h>


#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "raylib.h"
#include "raymath.h"

#include "style_cyber.h"

#include "graph.h"
#include "list.h"

// TODO: shaders
// TODO: force-directed

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
static bool threadActive = false;
static bool autoStep = false;
static pthread_mutex_t stepMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t stepCond = PTHREAD_COND_INITIALIZER;

static Graph *graph = NULL;




static int NearestNode(Vector2 pos)
{
    for (int i = 0; i < graph->nodes_i; ++i)
    {
        GraphNode *n = &graph->nodes[i];
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
            int added = GraphAddNode(graph, mouseWorldPos.x, mouseWorldPos.y);
            TraceLog(LOG_INFO, "Added node %d at %f %f", added, mouseWorldPos.x, mouseWorldPos.y);
            if (selectedNode != -1)
            {
                TraceLog(LOG_INFO, "Added edge %d -> %d", selectedNode, added);
                GraphAddEdge(graph, selectedNode, added);
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
        if (!GraphHasEdge(graph, selectedNode, nearest))
        {
            TraceLog(LOG_INFO, "Added edge %d -> %d", selectedNode, nearest);
            GraphAddEdge(graph, selectedNode, nearest);
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
            GraphNode *n = &graph->nodes[selectedMoveNode];
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
        DrawCircleV(from, nodeSize, GetColor(GuiGetStyle(DEFAULT, selectedNode == i ? BASE_COLOR_PRESSED
                                                                  : node->visited ? BASE_COLOR_FOCUSED
                                                                  : BASE_COLOR_NORMAL)));
        GraphEdge *edge = node->first;
        while (edge)
        {
            GraphNode *adjNode = &g->nodes[edge->node];
            Vector2 to = {adjNode->x, adjNode->y};
            DrawArrow(from, to, GetColor(GuiGetStyle(DEFAULT, edge->visited ? BASE_COLOR_FOCUSED : BASE_COLOR_NORMAL)));
            edge = edge->next;
        }
        DrawText(TextFormat("%d", i), from.x - nodeSize/4, from.y - nodeSize/2, nodeSize,
                 GetColor(GuiGetStyle(DEFAULT, selectedNode == i ? TEXT_COLOR_PRESSED
                                               : node->visited ? TEXT_COLOR_FOCUSED
                                               : TEXT_COLOR_NORMAL)));
    }
}


static void ThreadStep(void)
{
    if (autoStep)
    {
        // TODO: make this configurable (slider)
        usleep(500000);
        return;
    }
    pthread_mutex_lock(&stepMutex);
    pthread_cond_wait(&stepCond, &stepMutex);
    pthread_mutex_unlock(&stepMutex);
}

static void *GraphBFSThread(void *arg)
{
    GraphClearVisited(graph);
    TraceLog(LOG_INFO, "BFS started");
    int root = selectedNode;
    selectedNode = -1;
    List *queue = ListCreate();
    graph->nodes[root].visited = true;
    TraceLog(LOG_INFO, "BFS visited %d (root)", root);
    ThreadStep();
    ListAppend(queue, INT_ELEM(root));
    while (ListSize(queue) > 0)
    {
        int adj = ListRemoveFirst(queue).i;
        GraphEdge *edge = graph->nodes[adj].first;
        while (edge)
        {
            GraphNode *node = &graph->nodes[edge->node];
            if (!node->visited)
            {
                node->visited = true;
                edge->visited = true;
                TraceLog(LOG_INFO, "BFS visited %d", edge->node);
                ListAppend(queue, INT_ELEM(edge->node));
                ThreadStep();
            }
            edge = edge->next;
        }
    }
    ListDestroy(queue);
    TraceLog(LOG_INFO, "BFS finished");
    threadActive = false;
    return NULL;
}

static void *GraphDFSThread(void *arg)
{
    // TODO: DFS
    TraceLog(LOG_INFO, "DFS started");
    sleep(1);
    TraceLog(LOG_INFO, "DFS finished");
    threadActive = false;
    return NULL;
}

static void DispatchThread(void)
{
    if (threadActive)
    {
        TraceLog(LOG_WARNING, "Thread already active");
        return;
    }
    void *(*threadFunction)(void *) = NULL;
    switch (dropDownActive)
    {
        case 0:
            threadFunction = GraphBFSThread;
            break;
        case 1:
            threadFunction = GraphDFSThread;
            break;
        default:
            break;
    }
    if (pthread_create(&threadId, NULL, threadFunction, NULL) != 0)
    {
        TraceLog(LOG_ERROR, "Error creating thread");
    }
    else
    {
        TraceLog(LOG_INFO, "Thread created successfully");
        threadActive = true;
    }
}





int main(void)
{

    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screenWidth, screenHeight, "Graphs");
    GuiLoadStyleCyber();
    SetTargetFPS(60);
    camera.zoom = 1.0f;


    graph = GraphCreate();

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
                DrawGraph(graph);
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


            ++gui_i;
            if (GuiButton((Rectangle){ 2*padding, 2*padding + gui_i*(controlHeight+padding), controlWidth/2.f-padding/2., controlHeight}, "STEP"))
            {
                pthread_mutex_lock(&stepMutex);
                pthread_cond_signal(&stepCond);
                pthread_mutex_unlock(&stepMutex);
            }
            bool autoStepPrev = autoStep;
            GuiToggle((Rectangle){ 2.5*padding + controlWidth/2., 2*padding + gui_i*(controlHeight+padding), controlWidth/2.-padding/2., controlHeight}, "AUTO", &autoStep);
            if (autoStep != autoStepPrev)
            {
                pthread_mutex_lock(&stepMutex);
                pthread_cond_signal(&stepCond);
                pthread_mutex_unlock(&stepMutex);

            }
            --gui_i;


            if (GuiDropdownBox((Rectangle){ 2*padding, 2*padding + gui_i*(controlHeight+padding), controlWidth/2.-padding/2., controlHeight}, 
                               "BFS;DFS", &dropDownActive, dropDownEditMode)) dropDownEditMode = !dropDownEditMode;
            if (GuiButton((Rectangle){ 2.5*padding + controlWidth/2., 2.*padding + gui_i*(controlHeight+padding), controlWidth/2.-padding/2., controlHeight}, "START"))
            {
                if (selectedNode != -1) DispatchThread();
                else TraceLog(LOG_WARNING, "No node selected");
            }
            ++gui_i;
            ++gui_i;


            GuiUnlock();
            // raygui ------------------------------------------------------


            DrawFPS(screenWidth - 80, 0);

        EndDrawing();
    }



    GraphDestroy(graph);

    CloseWindow();

    return 0;
}
