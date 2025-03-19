#include <math.h>
#include <stdio.h>

#include "raylib.h"

#include "graph.h"

#define NODE_SIZE 15.0f



void DrawArrow(Vector2 start, Vector2 end, Color color)
{
    Vector2 dir = {end.x - start.x, end.y - start.y};
    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (len == 0) return;
    Vector2 norm = {dir.x / len, dir.y / len};
    Vector2 v1 = {end.x - norm.x * NODE_SIZE, end.y - norm.y * NODE_SIZE};
    Vector2 perp = {-norm.y, norm.x};
    Vector2 offset = {perp.x * NODE_SIZE / 2.0f, perp.y * NODE_SIZE / 2.0f};
    Vector2 v2 = {v1.x + offset.x, v1.y + offset.y};
    Vector2 v3 = {v1.x - offset.x, v1.y - offset.y};
    DrawLineEx(start, end, 2, color);
    DrawTriangle(end, v3, v2, color);
}

void draw_graph(graph_t *g)
{
    for (size_t i = 0; i < g->nodes_i; ++i)
    {
        node_t *n = &g->nodes[i];
        Vector2 from = {n->x, n->y};
        DrawCircleV(from, NODE_SIZE, RED);
        edge_t *e = n->first;
        while (e)
        {
            n = &g->nodes[e->node];
            Vector2 to = {n->x, n->y};
            Vector2 dir = {to.x - from.x, to.y - from.y};
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            Vector2 norm = {dir.x / len, dir.y / len};
            Vector2 offset = {norm.x * NODE_SIZE, norm.y * NODE_SIZE};
            DrawArrow((Vector2){from.x + offset.x, from.y + offset.y}, (Vector2){to.x - offset.x, to.y - offset.y}, BLACK);

            e = e->next;
        }
    }
}



int main(void)
{
    InitWindow(600, 600, "graphs");
    SetTargetFPS(60);
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    graph_t *g = graph_create();

    size_t last_node = -1;
    while (!WindowShouldClose())
    {
        Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            size_t node = graph_add_node(g, mouse.x, mouse.y);
            if (last_node != -1)
            {
                graph_add_edge(g, last_node, node);
            }
            last_node = node;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                draw_graph(g);
            EndMode2D();

            DrawFPS(0, 0);

        EndDrawing();
    }



    graph_destroy(g);

    CloseWindow();

    return 0;
}
