#include <stdio.h>

#include "SDL2/SDL.h"

#include "SDL2/SDL_render.h"
#include "graph.h"
#include "graph_vis.h"
#include "linked_list.h"

#define WIDTH 600
#define HEIGHT 600


void print_graph(graph_t *g)
{
    list_t *nodes = graph_get_nodes(g);
    printf("nodes_size = %zu\n", list_size(nodes));
    iter_t *nodes_iter = iter_create(nodes);
    while (iter_has_next(nodes_iter))
    {
        graph_node_t *n = iter_next(nodes_iter);
        printf("%s -> ", node_get_name(n));
        iter_t *adj_iter = iter_create(node_get_adj(n));
        while (iter_has_next(adj_iter))
        {
            graph_node_t *adj = iter_next(adj_iter);
            printf("%s, ", node_get_name(adj));
        }
        iter_destroy(adj_iter);
        printf("\n");
    }
    iter_destroy(nodes_iter);
}


int main(void)
{
    graph_t *g = graph_create();

    graph_node_t *a = graph_add_node(g, "a");
    graph_node_t *b = graph_add_node(g, "b");
    graph_node_t *c = graph_add_node(g, "c");
    graph_node_t *d = graph_add_node(g, "d");
    graph_node_t *f = graph_add_node(g, "f");

    graph_add_edge(a, b);
    graph_add_edge(a, c);
    graph_add_edge(d, f);

    graph_vis_t *gv = graph_vis_create(g);



    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    Uint64 prev_ticks = SDL_GetTicks64();
    bool quit = false;
    while (!quit)
    {
        // int mx, my;
        // Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;

                        default:
                            break;
                    }
                    break;
                
                default:
                    break;
            }
        }

        Uint64 curr_ticks = SDL_GetTicks64();
        float dt = (curr_ticks - prev_ticks) / 1000.0f;
        prev_ticks = curr_ticks;


        graph_vis_update(gv, dt);

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);


        list_t *gv_nodes = graph_vis_get_nodes(gv);
        iter_t *gv_nodes_iter = iter_create(gv_nodes);
        SDL_FRect rect = { .w = 20, .h = 20 };
        while (iter_has_next(gv_nodes_iter))
        {
            graph_vis_node_t *gv_node = iter_next(gv_nodes_iter);
            rect.x = gv_node->x * WIDTH;
            rect.y = gv_node->y * HEIGHT;
            SDL_RenderFillRectF(renderer, &rect);
        }

        iter_destroy(gv_nodes_iter);


        SDL_RenderPresent(renderer);
        SDL_Delay(30);
    }


    graph_vis_destroy(gv);
    graph_destroy(g);


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}
