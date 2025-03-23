#include <math.h>
#include <pthread.h>
#include <unistd.h>


#define RAYGUI_IMPLEMENTATION
#include "raygui/raygui.h"

#define GUI_LAYOUT_IMPLEMENTATION
#include "raygui/gui_layout.h"

#include "raylib.h"
#include "raymath.h"

#include "styles/style_jungle.h"            // raygui style: jungle
#include "styles/style_candy.h"             // raygui style: candy
#include "styles/style_lavanda.h"           // raygui style: lavanda
#include "styles/style_cyber.h"             // raygui style: cyber
#include "styles/style_terminal.h"          // raygui style: terminal
#include "styles/style_ashes.h"             // raygui style: ashes
#include "styles/style_bluish.h"            // raygui style: bluish
#include "styles/style_dark.h"              // raygui style: dark
#include "styles/style_cherry.h"            // raygui style: cherry
#include "styles/style_sunny.h"             // raygui style: sunny
#include "styles/style_enefete.h"           // raygui style: enefete

#include "graph.h"
#include "list.h"

// TODO: shaders
// TODO: force-directed



static Vector2 mouseScreenPos = {0.0f, 0.0f};
static Vector2 mouseWorldPos = {0.0f, 0.0f};
static Camera2D camera = { 0 };
static GuiLayoutState state;
static Rectangle controlPanelRec;

static pthread_t threadId = { 0 };
static bool threadActive = false;
static pthread_mutex_t stepMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t stepCond = PTHREAD_COND_INITIALIZER;

static const float nodeSize = 20.0f;
static int selectedNode = -1;

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
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(mouseScreenPos, controlPanelRec))
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
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mouseScreenPos, controlPanelRec))
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

        // TODO: remove node instead
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
    if (state.autoActive)
    {
        int delay = 1100000 - state.autoSpeedValue  * 10000;
        usleep(delay);
        if (state.autoActive) return;
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
    Queue *queue = QueueCreate(); // visit their adjacent
    graph->nodes[root].visited = true;
    TraceLog(LOG_INFO, "BFS visited %d (root)", root);
    QueueEnqueue(queue, INT_ELEM(root));
    while (QueueSize(queue) > 0)
    {
        int node = QueueDequeue(queue).i;
        GraphEdge *adjEdge = graph->nodes[node].first;
        while (adjEdge)
        {
            GraphNode *adjNode = &graph->nodes[adjEdge->node];
            if (adjNode->visited)
            {
                adjEdge = adjEdge->next;
                continue;
            }
            ThreadStep();
            adjNode->visited = true;
            adjEdge->visited = true;
            TraceLog(LOG_INFO, "BFS visited %d", adjEdge->node);
            QueueEnqueue(queue, INT_ELEM(adjEdge->node));
            adjEdge = adjEdge->next;
        }
    }
    QueueDestroy(queue);
    TraceLog(LOG_INFO, "BFS finished");
    threadActive = false;
    return NULL;
}


static void DFSThreadRec(GraphEdge *edge)
{
    GraphNode *node = &graph->nodes[edge->node];
    node->visited = true;
    edge->visited = true;
    TraceLog(LOG_INFO, "DFS visited %d", edge->node);
    GraphEdge *adjEdge = node->first;
    while (adjEdge)
    {
        GraphNode *adjNode = &graph->nodes[adjEdge->node];
        if (adjNode->visited)
        {
            adjEdge = adjEdge->next;
            continue;
        }
        ThreadStep();
        DFSThreadRec(adjEdge);
        adjEdge = adjEdge->next;
    }
}

static void *GraphDFSThread(void *arg)
{
    GraphClearVisited(graph);
    TraceLog(LOG_INFO, "DFS started");
    int root = selectedNode;
    selectedNode = -1;
    GraphEdge rootEdge = { .visited = false, .node = root, .next = NULL };
    DFSThreadRec(&rootEdge); // TODO: change dfs to non recursive
    TraceLog(LOG_INFO, "DFS finished");
    threadActive = false;
    return NULL;

}

static void DispatchThread(void)
{
    if (threadActive)
    {
        // TODO: cancel running thread
        TraceLog(LOG_WARNING, "Thread already active");
        return;
    }
    void *(*threadFunction)(void *) = NULL;
    switch (state.algoActive)
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
    const int screenWidth = 1000;
    const int screenHeight = 600;
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screenWidth, screenHeight, "Graphs");
    GuiLoadStyleCyber();
    SetTargetFPS(60);
    camera.zoom = 1.0f;

    state = InitGuiLayout();
    controlPanelRec = state.layoutRecs[0];
    state.modeActive = 1;
    state.autoActive = false;
    state.autoSpeedValue = 50;


    graph = GraphCreate();

    while (!WindowShouldClose())
    {
        mouseScreenPos = GetMousePosition();
        mouseWorldPos = GetScreenToWorld2D(mouseScreenPos, camera);

        switch (GetKeyPressed())
        {
            case KEY_F:
                state.modeActive = 0;
                break;
            case KEY_A:
                state.modeActive = 1;
                break;
            case KEY_M:
                state.modeActive = 2;
                break;
            case KEY_R:
                camera.zoom = 1.0f;
                camera.offset = camera.target = (Vector2){0.0f, 0.0f};
                break;

            default:
                break;
        }


        switch (state.modeActive)
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


            DrawRectangleRec(controlPanelRec, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_DISABLED)));
            
            bool autoActivePrev = state.autoActive;
            GuiLayout(&state);

            if (state.resetCameraPressed)
            {
                camera.zoom = 1.0f;
                camera.offset = camera.target = (Vector2){0.0f, 0.0f};
            }

            if (state.stepPressed)
            {
                pthread_mutex_lock(&stepMutex);
                pthread_cond_signal(&stepCond);
                pthread_mutex_unlock(&stepMutex);
            }

            if (state.startPressed)
            {
                if (selectedNode != -1) DispatchThread();
                else TraceLog(LOG_WARNING, "No node selected");
            }

            if (state.autoActive && !autoActivePrev)
            {
                pthread_mutex_lock(&stepMutex);
                pthread_cond_signal(&stepCond);
                pthread_mutex_unlock(&stepMutex);
            }
            

            DrawFPS(screenWidth - 80, 0);

        EndDrawing();
    }



    GraphDestroy(graph);

    CloseWindow();

    return 0;
}
